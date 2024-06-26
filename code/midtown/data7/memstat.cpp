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

define_dummy_symbol(data7_memstat);

#include "memstat.h"

#include "memory/allocator.h"

// ?getMem@@YAHXZ
ARTS_EXPORT /*static*/ isize getMem()
{
    return ALLOCATOR.GetHeapUsed();
}

static constexpr const i32 MAX_CHECK = 16;

static isize MemStatValues[MAX_CHECK] {};
static const char* MemStatNames[MAX_CHECK] {};

void BeginMemStat(const char* name)
{
    if (EnableMemStat)
    {
        ArAssert(beginStackCount < MAX_CHECK, "MemStat overflow");

        MemStatValues[beginStackCount] = getMem();
        MemStatNames[beginStackCount] = name;

        ++beginStackCount;
    }
}

void EndMemStat()
{
    if (EnableMemStat)
    {
        ArAssert(beginStackCount, "MemStat underflow");

        --beginStackCount;

        isize prev = MemStatValues[beginStackCount];
        isize current = getMem();

        Warningf("%*sMemStat: '%s' %zdK before, %zdK after, %zdK difference", beginStackCount, "",
            MemStatNames[beginStackCount], prev >> 10, current >> 10, (current - prev) >> 10);
    }
}
