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

#pragma once

/*
    agid3d:dderror

    0x532090 | char * __cdecl DDErrorToString(long) | ?DDErrorToString@@YAPADJ@Z
    0x5327B0 | void __cdecl __DDTry(int,char const *,char const *,int) | ?__DDTry@@YAXHPBD0H@Z
    0x532890 | void __cdecl __DDRelease(struct IUnknown *,char const *,char const *,int) | ?__DDRelease@@YAXPAUIUnknown@@PBD1H@Z
    0x795C88 | int DDIgnoreErrors | ?DDIgnoreErrors@@3HA
    0x795C8C | int DDNeedRestore | ?DDNeedRestore@@3HA
*/

struct IUnknown;

// ?DDErrorToString@@YAPADJ@Z
ARTS_IMPORT char* DDErrorToString(ilong arg1);

// ?__DDRelease@@YAXPAUIUnknown@@PBD1H@Z
ARTS_IMPORT void __DDRelease(IUnknown* arg1, const char* arg2, const char* arg3, i32 arg4);

// ?__DDTry@@YAXHPBD0H@Z
ARTS_IMPORT void __DDTry(i32 arg1, const char* arg2, const char* arg3, i32 arg4);

// ?DDIgnoreErrors@@3HA
ARTS_IMPORT extern b32 DDIgnoreErrors;

// ?DDNeedRestore@@3HA
ARTS_IMPORT extern b32 DDNeedRestore;

#define DD_RELEASE(VALUE)                                     \
    do                                                        \
    {                                                         \
        if (VALUE)                                            \
        {                                                     \
            __DDRelease(VALUE, #VALUE, ARTS_FILE, ARTS_LINE); \
            VALUE = nullptr;                                  \
        }                                                     \
    } while (false);

#define DD_TRY(EXPR) __DDTry(EXPR, #EXPR, ARTS_FILE, ARTS_LINE)
