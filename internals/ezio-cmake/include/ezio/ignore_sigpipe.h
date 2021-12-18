/*
 @ 0xCCCCCCCC
*/

#ifndef EZIO_IGNORE_SIGPIPE_H_
#define EZIO_IGNORE_SIGPIPE_H_

#include "kbase/basic_macros.h"

namespace ezio {

class IgnoreSigPipe {
public:
    IgnoreSigPipe() noexcept;

    ~IgnoreSigPipe() = default;

    DISALLOW_COPY(IgnoreSigPipe);

    DISALLOW_MOVE(IgnoreSigPipe);
};

}   // namespace ezio

#endif  // EZIO_IGNORE_SIGPIPE_H_
