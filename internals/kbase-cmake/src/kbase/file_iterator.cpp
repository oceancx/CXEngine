/*
 @ 0xCCCCCCCC
*/

#include "kbase/file_iterator.h"

#include "kbase/error_exception_util.h"

namespace kbase {

FileIterator& FileIterator::operator++()
{
    ENSURE(CHECK, !ReachedEnd()).Require();

    GetNext();
    return *this;
}

FileIterator FileIterator::operator++(int)
{
    FileIterator tmp(*this);
    ++(*this);
    return tmp;
}

// static
bool FileIterator::ShouldSkip(Path::string_view_type pathname) noexcept
{
    return pathname == Path::kCurrentDir || pathname == Path::kParentDir;
}

bool operator==(const FileIterator& lhs, const FileIterator& rhs) noexcept
{
    return lhs.Equal(rhs);
}

bool operator!=(const FileIterator& lhs, const FileIterator& rhs) noexcept
{
    return !(lhs == rhs);
}

}   // namespace kbase
