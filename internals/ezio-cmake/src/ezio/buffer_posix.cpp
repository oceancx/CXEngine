/*
 @ 0xCCCCCCCC
*/

#include "ezio/buffer.h"

#include <sys/uio.h>

namespace ezio {

ssize_t ReadFDInVec(int fd, Buffer& buf)
{
    constexpr size_t kExtraBufSize = 65535;
    char extra_buf[kExtraBufSize];

    iovec vec[2];
    vec[0].iov_base = buf.BeginWrite();
    vec[0].iov_len = buf.writable_size();
    vec[1].iov_base = extra_buf;
    vec[1].iov_len = kExtraBufSize;

    auto used_vec_cnt = buf.writable_size() < kExtraBufSize ? 2 : 1;
    auto size_read = readv(fd, vec, used_vec_cnt);
    if (size_read < 0) {
        return -1;
    }

    if (static_cast<size_t>(size_read) <= buf.writable_size()) {
        buf.EndWrite(static_cast<size_t>(size_read));
    } else {
        auto writable = buf.writable_size();
        buf.EndWrite(writable);
        buf.Write(extra_buf, size_read - writable);
    }

    return size_read;
}

}   // namespace ezio
