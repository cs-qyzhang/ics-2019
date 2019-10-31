/*
 * Copyright (C) 2019 qiuyang Zhang. All rights reserved.
 * 
 * filename    : line.h
 * author      : qiuyang Zhang
 * created     : 2019/04/04
 * description : 
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef LINE_H
#define LINE_H

#include "position.h"
#include <vector>

enum PointType
{
    POINT_POSITION,
    POINT_COORD
};

struct Point
{
    PointType type;
    Position *position;
    Coord *coord;
};

typedef std::vector<std::vector<Position>> Line;

#endif // LINE_H
