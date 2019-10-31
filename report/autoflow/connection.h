/*
 * Copyright (C) 2019 qiuyang Zhang. All rights reserved.
 * 
 * filename    : connection.h
 * author      : qiuyang Zhang
 * created     : 2019/04/02
 * description : 组之间的联系
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
#ifndef CONNECTION_H
#define CONNECTION_H

#include "position.h"

struct Connection
{
    Position a, b;
    std::string label;
};

#endif // CONNECTION_H
