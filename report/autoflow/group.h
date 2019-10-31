/*
 * Copyright (C) 2019 qiuyang Zhang. All rights reserved.
 * 
 * filename    : group.h
 * author      : qiuyang Zhang
 * created     : 2019/04/02
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
#ifndef GROUP_H
#define GROUP_H

#include <vector>
#include "position.h"
#include "line.h"

enum GroupType
{
    // 最高层次
    CANVAS,

    // 高层次组
    PROC,
    FOR,
    FOR_INIT,
    FOR_COND,
    FOR_AFTERTHOUGHT,
    FOR_PROC,
    WHILE,
    WHILE_COND,
    WHILE_PROC,
    IF,
    IF_COND,
    IF_BRANCH,

    // 基本组
    NODE_PROC,
    NODE_INPUT,
    NODE_OUTPUT,
    NODE_COND,
    COORD,
    NODE_BEGIN,
    NODE_END
};

struct Group
{
    // 相对位置
    Position position;

    // 用于set_size函数，为当前group画布的上下左右的下标
    double left, right, top, bottom;

    // 相对于top_left或上层group
    Coord west_coord, east_coord, south_coord, north_coord;

    // 连接Group的东南西北的坐标，当Group为基本组时不需要
    Position west, east, south, north;

    GroupType type;
    std::string label;
    std::string name;

    std::vector<Group *> child;
    Group *father;
    std::vector<Position> connection;

    // 用于set_size函数
    bool has_placed;
    bool require_placed;

    Line line;

    int src_line;
    std::string src;

    Group(GroupType type = PROC, Group *father = nullptr,  std::string name = "",
        std::string label = "", int src_line = 0, std::string src = ""):
        type(type), father(father), name(name), label(label), has_placed(false),
        require_placed(false), src_line(src_line), src(src)
    {
    }

    double height()
    {
        return bottom - top;
    }

    double width()
    {
        return right - left;
    }
};

#endif // GROUP_H
