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
    stream:problems

    0x560DB0 | void __cdecl InitProblems(void) | ?InitProblems@@YAXXZ
    0x560E10 | void __cdecl DumpProblems(void) | ?DumpProblems@@YAXXZ
    0x561050 | int __cdecl cmpProblem(void const *,void const *) | ?cmpProblem@@YAHPBX0@Z
    0x561100 | void __cdecl RegisterProblem(char *,char *,char *) | ?RegisterProblem@@YAXPAD00@Z
    0x561340 | public: __thiscall problem_t::~problem_t(void) | ??1problem_t@@QAE@XZ
*/

// 0x560E10 | ?DumpProblems@@YAXXZ
ARTS_IMPORT void DumpProblems();

// 0x560DB0 | ?InitProblems@@YAXXZ
ARTS_IMPORT void InitProblems();

// 0x561100 | ?RegisterProblem@@YAXPAD00@Z
ARTS_IMPORT void RegisterProblem(char* arg1, char* arg2, char* arg3);

struct problem_t
{
public:
    // 0x561340 | ??1problem_t@@QAE@XZ | inline
    ARTS_IMPORT ~problem_t();
};

check_size(problem_t, 0x0);
