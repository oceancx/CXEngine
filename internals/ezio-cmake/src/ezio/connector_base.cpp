/*
 @ 0xCCCCCCCC
*/

#include "ezio/connector_base.h"

namespace ezio {

// This looks so damn weird...
constexpr std::chrono::milliseconds ConnectorBase::kInitialRetryDelay;
constexpr std::chrono::seconds ConnectorBase::kMaxRetryDelay;

ConnectorBase::ConnectorBase(ezio::EventLoop* loop, const ezio::SocketAddress& addr)
    : loop_(loop),
      remote_addr_(addr),
      weakly_bound_(false),
      connecting_(false),
      retry_delay_(kInitialRetryDelay)
{}

void ConnectorBase::WeaklyBind(const std::shared_ptr<void>& obj)
{
    bound_object_ = obj;
    weakly_bound_ = true;
}

}   // namespace ezio
