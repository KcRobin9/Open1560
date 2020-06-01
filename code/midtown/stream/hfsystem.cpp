/*
    Open1560 - An Open Source Re-Implementation of Midtown Madness 1 Beta
    Copyright (C) 2020 Brick

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

define_dummy_symbol(stream_hfsystem);

#include "hfsystem.h"

#include "core/minwin.h"
#include "data7/pager.h"
#include "filestream.h"

#include <direct.h>
#include <io.h>

HierFileSystem::HierFileSystem() = default;
HierFileSystem::~HierFileSystem() = default;

// TODO: Use wide char versions of winapi

b32 HierFileSystem::ChangeDir(const char* path)
{
    export_hook(0x560380);

    return _chdir(path) >= 0;
}

static inline constexpr bool IsStdPath(const char* path) noexcept
{
    return (path[0] == '-') && !path[1];
}

Owner<class Stream*> HierFileSystem::CreateOn(const char* path, void* buffer, i32 buffer_len)
{
    export_hook(0x5602A0);

    Ptr<FileStream> result = MakeUnique<FileStream>(buffer, buffer_len, this);

    path = FQN(path);

    i32 error = IsStdPath(path) ? result->Stdout() : result->Create(path);

    if (error >= 0)
    {
        if (LogOpenOn)
            Displayf("::OPEN::%s", path);
    }
    else
    {
        result->Error("CreateOn.FileStream.Create");
        result.reset();
    }

    return result.release();
}

struct HierFileEntry
{
    HANDLE Handle {INVALID_HANDLE_VALUE};
    WIN32_FIND_DATAA Data {};

    void inline FillInfo(FileInfo& info)
    {
        arts_strcpy(info.Path, Data.cFileName);
        info.IsDirectory = (Data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    }
};

check_size(HierFileEntry, 0x144);

Owner<struct FileInfo*> HierFileSystem::FirstEntry(const char* path)
{
    export_hook(0x5603C0);

    if (!QueryOn(path))
        return nullptr;

    CStringBuffer<256> needle;
    needle.assign(path);
    needle.append("/*");

    // TODO: Skip "." and ".." files
    WIN32_FIND_DATAA data {};
    HANDLE handle = FindFirstFileA(FQN(needle), &data);

    if (handle == INVALID_HANDLE_VALUE)
        return nullptr;

    Ptr<FileInfo> result = MakeUnique<FileInfo>();

    Ptr<HierFileEntry> context = MakeUnique<HierFileEntry>();

    context->Handle = handle;
    context->Data = data;
    context->FillInfo(*result);

    result->Context = context.release();

    return result.release();
}

b32 HierFileSystem::GetDir(char* buffer, i32 buffer_len)
{
    export_hook(0x5603A0);

    _getcwd(buffer, buffer_len);

    return true;
}

Owner<struct FileInfo*> HierFileSystem::NextEntry(Owner<struct FileInfo*> info)
{
    export_hook(0x560500);

    HierFileEntry* context = static_cast<HierFileEntry*>(info->Context);

    if (!FindNextFileA(context->Handle, &context->Data))
    {
        FindClose(context->Handle);

        delete context;
        delete info;

        return nullptr;
    }

    context->FillInfo(*info);

    return info;
}

Owner<class Stream*> HierFileSystem::OpenOn(const char* path, b32 read_only, void* buffer, i32 buffer_len)
{
    export_hook(0x560100);

    path = FQN(path);

    Ptr<FileStream> result = MakeUnique<FileStream>(buffer, buffer_len, this);

    i32 error = 0;

    if (read_only && IsStdPath(path))
    {
        error = result->Stdin();
    }
    else
    {
        if (read_only && LogOpenOn)
            Displayf("::OPEN::%s", path);

        error = result->Open(path, read_only);
    }

    if (error < 0)
    {
        result->Error("OpenOn.FileStream.Open");
        result.reset();
    }

    return result.release();
}

b32 HierFileSystem::QueryOn(const char* path)
{
    export_hook(0x560040);

    if (const char* allowed = HierAllowPath)
    {
        if (!IsPhysicalPath(path))
        {
            usize folder_name_len = std::strcspn(path, "/\\:");

            while (*allowed)
            {
                usize allowed_len = std::strlen(allowed);

                if (!std::memcmp(path, allowed, std::min(allowed_len, folder_name_len)))
                    break;

                allowed += allowed_len + 1;
            }
        }

        if (*allowed == '\0')
            return false;

        Warningf("Allowing access to real file: '%s'", path);
    }

    return _access(FQN(path), 4) == 0;
}

b32 HierFileSystem::ValidPath(const char*)
{
    export_hook(0x55FF80);

    return true;
}

// TODO: Avoid this static buffer
static char FQNPathBuffer[128] {};

const char* FQN(const char* path)
{
    export_hook(0x55FF90);

    const char* prefix = HierPrefix;

    if (!prefix || !*prefix)
        return path;

    if (FileSystem::IsPhysicalPath(path))
        return path;

    arts_strcpy(FQNPathBuffer, prefix);

    if (!FileSystem::IsPathSeparator(prefix[std::strlen(prefix) - 1]))
        arts_strcat(FQNPathBuffer, "\\");

    arts_strcat(FQNPathBuffer, path);

    return FQNPathBuffer;
}

void PagerInfo_t::Read(void* buffer, u32 offset, u32 size)
{
    export_hook(0x5605D0);

    if (size == 0)
        size = Size - offset;

    offset += Offset;

    OVERLAPPED overlapped {};
    overlapped.Offset = offset;
    overlapped.OffsetHigh = 0;

    DWORD bytes_read = 0;

    if (!ReadFile(reinterpret_cast<HANDLE>(Handle), buffer, size, &bytes_read, &overlapped) || (bytes_read != size))
        Errorf("PagerInfo_t(%s)::Read - Failed to read 0x%X bytes @ 0x%X: 0x%08X", Name, size, offset, GetLastError());
}
