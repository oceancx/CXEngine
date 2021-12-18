/*
 @ 0xCCCCCCCC
*/

#include "ezio/tcp_client.h"

#include <functional>

#include "kbase/error_exception_util.h"
#include "kbase/string_format.h"

#include "ezio/event_loop.h"

namespace ezio {

using namespace std::placeholders;

TCPClient::TCPClient(EventLoop* loop, const SocketAddress& remote_addr, std::string name)
    : loop_(loop),
      name_(std::move(name)),
      state_(State::Disconnected),
      next_conn_id_(0),
      auto_reconnect_(false),
      connector_(MakeConnector(loop, remote_addr)),
      alive_token_(std::make_shared<AliveToken>())
{
    connector_->set_on_new_connection(std::bind(&TCPClient::HandleConnection, this, _1, _2));
    connector_->WeaklyBind(alive_token_);
}

TCPClient::~TCPClient()
{
    alive_token_ = nullptr;

    TCPConnectionPtr conn;

    {
        std::lock_guard<std::mutex> lock(conn_mutex_);
        conn = std::move(conn_);
    }

    // The connector is either still connecting, or doing nothing at all.
    if (!conn) {
        loop_->RunTask([c = std::shared_ptr<Connector>(std::move(connector_))] {
            c->Cancel();
        });

        return;
    }

    // Active connection exists.
    loop_->RunTask(std::bind(&TCPConnection::MakeTeardown, conn));
}

void TCPClient::Connect()
{
    auto desired = State::Disconnected;
    if (state_.compare_exchange_strong(desired, State::Connecting,
                                       std::memory_order_acq_rel, std::memory_order_relaxed)) {
        ENSURE(CHECK, !connector_->connecting()).Require();
        loop_->RunTask([this] {
            connector_->Connect();
        });
    }
}

void TCPClient::Disconnect()
{
    auto desired = State::Connected;
    if (state_.compare_exchange_strong(desired, State::Disconnecting,
                                       std::memory_order_acq_rel, std::memory_order_relaxed)) {
        TCPConnectionPtr conn;

        {
            std::lock_guard<std::mutex> lock(conn_mutex_);
            conn = conn_;
        }

        conn->Shutdown();
    }
}

void TCPClient::Cancel()
{
    if (state_.load(std::memory_order_acquire) == State::Connecting) {
        loop_->RunTask([this] {
            if (connector_->connecting()) {
                connector_->Cancel();
            }

            // The connection could be established at this time, so we reset to disconnected
            // only if we are in connecting.
            auto desired = State::Connecting;
            state_.compare_exchange_strong(desired, State::Disconnected, std::memory_order_acq_rel,
                                           std::memory_order_relaxed);
        });
    }
}

void TCPClient::HandleConnection(ScopedSocket&& sock, const SocketAddress& local_addr)
{
    ENSURE(CHECK, loop_->BelongsToCurrentThread()).Require();

    const auto& peer_addr = connector_->remote_addr();
    auto conn_name = kbase::StringFormat("{0}-{1}#{2}", name_, peer_addr.ToHostPort(),
                                         next_conn_id_);
    ++next_conn_id_;

    auto conn = std::make_shared<TCPConnection>(loop_,
                                                std::move(conn_name),
                                                std::move(sock),
                                                local_addr,
                                                peer_addr);

    conn->set_on_connection(on_connection_);
    conn->set_on_message(on_message_);
    conn->set_on_close(std::bind(&TCPClient::HandleDisconnection, this, _1));

    {
        std::lock_guard<std::mutex> lock(conn_mutex_);
        conn_ = std::move(conn);
    }

    conn_->MakeEstablished();
}

void TCPClient::HandleDisconnection(const TCPConnectionPtr& conn)
{
    ENSURE(CHECK, loop_->BelongsToCurrentThread()).Require();
    ENSURE(CHECK, loop_ == conn->event_loop()).Require();

    {
        std::lock_guard<std::mutex> lock(conn_mutex_);
        conn_ = nullptr;
    }

    state_.store(State::Disconnected, std::memory_order_release);

    loop_->QueueTask(std::bind(&TCPConnection::MakeTeardown, conn));

    if (auto_reconnect() && state_.load(std::memory_order_acquire) != State::Disconnected) {
        connector_->Connect();
    }
}

}   // namespace ezio
