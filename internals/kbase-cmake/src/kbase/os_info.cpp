/*
 @ 0xCCCCCCCC
*/

#include "kbase/os_info.h"

namespace kbase {

// static
OSInfo* OSInfo::GetInstance()
{
    static OSInfo instance;
    return &instance;
}

}   // namespace kbase
