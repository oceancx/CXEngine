/*
 @ 0xCCCCCCCC
*/

#ifndef EZIO_COMMON_EVENT_HANDLERS_H_
#define EZIO_COMMON_EVENT_HANDLERS_H_

#include <functional>
#include <memory>

#include "ezio/chrono_utils.h"

namespace ezio {

class Buffer;
class TCPConnection;

using TCPConnectionPtr = std::shared_ptr<TCPConnection>;

using ConnectionEventHandler = std::function<void(const TCPConnectionPtr&)>;
using MessageEventHandler = std::function<void(const TCPConnectionPtr&, Buffer&, TimePoint)>;
using CloseEventHandler = std::function<void(const TCPConnectionPtr&)>;

}   // namespace ezio

#endif  // EZIO_COMMON_EVENT_HANDLERS_H_
