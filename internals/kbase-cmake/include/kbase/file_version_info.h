/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_FILE_VERSION_INFO_H_
#define KBASE_FILE_VERSION_INFO_H_

#include <memory>
#include <vector>

#include <Windows.h>

#include "kbase/basic_macros.h"
#include "kbase/basic_types.h"
#include "kbase/path.h"

namespace kbase {

class FileVersionInfo {
public:
    ~FileVersionInfo() = default;

    DISALLOW_COPY(FileVersionInfo);

    DISALLOW_MOVE(FileVersionInfo);

    static std::unique_ptr<FileVersionInfo> CreateForFile(const Path& file);

    static std::unique_ptr<FileVersionInfo> CreateForModule(HMODULE module);

    VS_FIXEDFILEINFO* fixed_file_info() const noexcept
    {
        return info_block_;
    }

    std::wstring comments() const
    {
        return GetValue(L"Comments");
    }

    std::wstring internal_name() const
    {
        return GetValue(L"InternalName");
    }

    std::wstring product_name() const
    {
        return GetValue(L"ProductName");
    }

    std::wstring company_name() const
    {
        return GetValue(L"CompanyName");
    }

    std::wstring legal_copyright() const
    {
        return GetValue(L"LegalCopyright");
    }

    std::wstring product_version() const
    {
        return GetValue(L"ProductVersion");
    }

    std::wstring file_description() const
    {
        return GetValue(L"FileDescription");
    }

    std::wstring legal_trademarks() const
    {
        return GetValue(L"LegalTrademarks");
    }

    std::wstring private_build() const
    {
        return GetValue(L"PrivateBuild");
    }

    std::wstring file_version() const
    {
        return GetValue(L"FileVersion");
    }

    std::wstring original_filename() const
    {
        return GetValue(L"OriginalFilename");
    }

    std::wstring special_build() const
    {
        return GetValue(L"SpecialBuild");
    }

private:
    using VersionData = std::vector<byte>;

    explicit FileVersionInfo(VersionData&& data);

    // Returns the value associated with the `name`;
    // Returns an empty string otherwise.
    std::wstring GetValue(const wchar_t* name) const;

private:
    VersionData data_;
    VS_FIXEDFILEINFO* info_block_;
    unsigned short lang_;
    unsigned short code_page_;
};

}   // namespace kbase

#endif  // KBASE_FILE_VERSION_INFO_H_
