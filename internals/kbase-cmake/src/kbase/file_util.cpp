/*
 @ 0xCCCCCCCC
*/

#include "kbase/file_util.h"

#include <fstream>

#include "kbase/error_exception_util.h"
#include "kbase/file_iterator.h"
#include "kbase/logging.h"

namespace kbase {

bool IsDirectoryEmpty(const Path& path)
{
    return FileIterator(path, false) == FileIterator();
}

void ReadFileToString(const Path& path, std::string& data)
{
    // It seems the constructor of ifstream from MSVC has an overload function taking
    // wide-character.
    std::ifstream in(path.value(), std::ios::binary);
    if (!in) {
        DLOG(WARNING) << "Create/open file failed for path " << path.AsUTF8();
        return;
    }

    if (!data.empty()) {
        data.clear();
    }

    in.seekg(0, std::ios::end);
    data.resize(static_cast<size_t>(in.tellg()));
    in.seekg(0);
    in.read(&data[0], data.size());
}

std::string ReadFileToString(const Path& path)
{
    std::string data;
    ReadFileToString(path, data);

    return data;
}

void WriteStringToFile(const Path& path, const std::string& data, OpenMode mode)
{
    auto open_mode = (mode == OpenMode::Text) ? std::ios::out : std::ios::out | std::ios::binary;
    std::ofstream out(path.value(), open_mode);
    if (!out) {
        DLOG(WARNING) << "Create/open file faield for path " << path.AsUTF8();
        return;
    }

    out.write(data.data(), data.size());
}

void AppendStringToFile(const Path& path, const std::string& data, OpenMode mode)
{
    auto open_mode = (mode == OpenMode::Text) ? std::ios::app : std::ios::app | std::ios::binary;
    std::ofstream out(path.value(), open_mode);
    if (!out) {
        DLOG(WARNING) << "Create/open file faield for path " << path.AsUTF8();
        return;
    }

    out.write(data.data(), data.size());
}

}   // namespace kbase
