/*
 @ 0xCCCCCCCC
*/

#include "ezio/ignore_sigpipe.h"

#include <signal.h>

namespace ezio {

IgnoreSigPipe::IgnoreSigPipe() noexcept
{
    signal(SIGPIPE, SIG_IGN);
}

}   // namespace ezio
