/*
 @ 0xCCCCCCCC
*/

#include "kbase/file_version_info.h"

#include "kbase/error_exception_util.h"
#include "kbase/string_format.h"

#pragma comment(lib, "version.lib")

namespace {

struct LangCodePage {
    WORD language;
    WORD code_page;
};

}   // namespace

namespace kbase {

// static
std::unique_ptr<FileVersionInfo> FileVersionInfo::CreateForFile(const Path& file)
{
    DWORD info_size = GetFileVersionInfoSizeW(file.value().c_str(), nullptr);
    ENSURE(THROW, info_size != 0)(LastError()).Require();

    VersionData version_data(info_size);
    BOOL rv = GetFileVersionInfoW(file.value().c_str(), 0, info_size, version_data.data());
    ENSURE(THROW, rv != 0)(LastError()).Require();

    return std::unique_ptr<FileVersionInfo>(new FileVersionInfo(std::move(version_data)));
}

// static
std::unique_ptr<FileVersionInfo> FileVersionInfo::CreateForModule(HMODULE module)
{
    constexpr DWORD kMaxPath = MAX_PATH + 1;
    wchar_t file_name[kMaxPath] {0};

    DWORD rv = GetModuleFileNameW(module, file_name, kMaxPath);
    ENSURE(THROW, rv != 0)(LastError()).Require();

    return CreateForFile(Path(file_name));
}

FileVersionInfo::FileVersionInfo(VersionData&& data)
    : data_(std::move(data)), info_block_(nullptr), lang_(0U), code_page_(0U)
{
    // Set up VS_FIXEDFILEINFO struct.
    UINT dummy_size = 0U;
    VerQueryValueW(data_.data(), L"\\", reinterpret_cast<LPVOID*>(&info_block_), &dummy_size);

    // Retrieve translation language and code page.
    LangCodePage* translation = nullptr;
    VerQueryValueW(data_.data(),
                   L"\\VarFileInfo\\Translation",
                   reinterpret_cast<LPVOID*>(&translation),
                   &dummy_size);

    lang_ = translation->language;
    code_page_ = translation->code_page;
}

std::wstring FileVersionInfo::GetValue(const wchar_t* name) const
{
    LangCodePage lang_codepage[2] {
        {lang_, code_page_},
        {GetUserDefaultLangID(), code_page_}
    };

    for (const auto& lang_cp : lang_codepage) {
        auto sub_block = StringPrintf(L"\\StringFileInfo\\%04x%04x\\%s", lang_cp.language,
                                      lang_cp.code_page, name);
        LPVOID value_ptr = nullptr;
        UINT dummy_size = 0U;

        VerQueryValueW(data_.data(), sub_block.c_str(), &value_ptr, &dummy_size);

        if (value_ptr) {
            return std::wstring(static_cast<const wchar_t*>(value_ptr));
        }
    }

    return std::wstring();
}

}   // namespace kbase
