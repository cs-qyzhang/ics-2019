/*
 * Copyright (C) 2019 qiuyang Zhang. All rights reserved.
 * 
 * filename    : set_size.cpp
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
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stack>
#include <cassert>
#include "group.h"
#include "position.h"
#include "line.h"

using namespace std;

struct Padding
{
    double top;
    double bottom;
    double left;
    double right;
};

double node_fix_width = 0.0;
double font_width = 6.0;
double line_height = 12.0;
double node_distance = 30.0;
double if_line_distance = 15.0;
double while_line_distance = 15.0;
double io_arctan = 2.5;  // io的三角形的arctan

Padding proc_padding = {6.0, 6.0, 6.0, 6.0};
Padding io_padding = {6.0, 6.0, 6.0, 6.0};
Padding cond_padding = {3.0, 3.0, 3.0, 3.0};

double calc_text_width(const string &s)
{
    double result = 0.0;
    for (size_t i = 0; i < s.length(); ++i)
    {
        if ((int)s[i] < 0)
        {
            result += 2 * font_width;
            i += 2;
        }
        else
            result += font_width;
    }
    return result;
}

void
set_node_junction(Group *node)
{
    node->east.relatived  = node;
    node->east.relation   = EAST;
    node->west.relatived  = node;
    node->west.relation   = WEST;
    node->south.relatived = node;
    node->south.relation  = SOUTH;
    node->north.relatived = node;
    node->north.relation  = NORTH;
    node->west_coord.set(node->left, node->top + node->height() / 2.0);
    node->east_coord.set(node->left + node->width(), node->top + node->height() / 2.0);
    node->north_coord.set(node->left + node->width() / 2.0, node->top);
    node->south_coord.set(node->left + node->width() / 2.0, node->top + node->height());
}

void
place_mid_group(Group *mid_group, Group *node_group)
{
    if (node_group->has_placed)
        return;

    Group *relatived = node_group->position.relatived;

    // 如果参照对象没有被摆放则首先摆放参照对象
    if (!relatived->has_placed)
    {
        // 如果参照对象已经被要求摆放则说明有循环，FIXME：错误？
        if (relatived->require_placed)
            cerr << "place_mid_group: " + mid_group->name + " 's child "
                + node_group->name + " has circle in require_placed!" << endl;
        else    // 否则先摆放参照对象
            place_mid_group(mid_group, relatived);
    }

#define update_junction_coord() \
{ \
    node_group->west_coord.x += (new_left - node_group->left);  \
    node_group->east_coord.x += (new_left - node_group->left);  \
    node_group->south_coord.x += (new_left - node_group->left);  \
    node_group->north_coord.x += (new_left - node_group->left);  \
    node_group->west_coord.y += (new_top - node_group->top);  \
    node_group->east_coord.y += (new_top - node_group->top);  \
    node_group->south_coord.y += (new_top - node_group->top);  \
    node_group->north_coord.y += (new_top - node_group->top);  \
    node_group->right += (new_left - node_group->left);    \
    node_group->bottom += (new_top - node_group->top);  \
    node_group->left = new_left;    \
    node_group->top = new_top;    \
}
    double new_top, new_left;

    switch (node_group->position.relation)
    {
        case LEFT:
            new_left = relatived->west_coord.x - node_group->width() - node_group->position.xdistance;
            new_top = relatived->west_coord.y - (node_group->east_coord.y - node_group->top) + node_group->position.ydistance;
            update_junction_coord();
            break;
        case RIGHT:
            new_left = relatived->east_coord.x + node_group->position.xdistance;
            new_top = relatived->east_coord.y - (node_group->west_coord.y - node_group->top) + node_group->position.ydistance;
            update_junction_coord();
            break;
        case ABOVE:
            new_left = relatived->north_coord.x - (node_group->south_coord.x - node_group->left) + node_group->position.xdistance;
            new_top = relatived->north_coord.y - node_group->position.ydistance - node_group->height();
            update_junction_coord();
            break;
        case BELOW:
            new_left = relatived->south_coord.x - (node_group->north_coord.x - node_group->left) + node_group->position.xdistance;
            new_top = relatived->south_coord.y + node_group->position.ydistance;
            update_junction_coord();
            break;
        case ABOVE_LEFT:
        case ABOVE_RIGHT:
        case BELOW_LEFT:
            new_left = relatived->left - node_group->position.xdistance - node_group->width();
            new_top = relatived->bottom + node_group->position.ydistance;
            update_junction_coord();
            break;
        case BELOW_RIGHT:
            new_left = relatived->right + node_group->position.xdistance;
            new_top = relatived->bottom + node_group->position.ydistance;
            update_junction_coord();
            break;
        case CENTER:
        case NORTH:
        case SOUTH:
        case WEST:
        case EAST:
        default:
            cerr << "place_mid_group: " + mid_group->name + " 's child "
                + node_group->name + "'relation is wrong!" << endl;
    }
    node_group->has_placed = true;
    mid_group->left = min(node_group->left, mid_group->left);
    mid_group->right = max(node_group->right, mid_group->right);
    mid_group->top = min(node_group->top, mid_group->top);
    mid_group->bottom = max(node_group->bottom, mid_group->bottom);
}

void
set_size(Group *group)
{
    assert(group != nullptr);

    int label_len;
    label_len = group->label.length();
    Group *relative = nullptr;
    
    Position *position;

    vector<Position> pos;

    Group *p, *while_coord;

    int left_index, right_index;
    double width, height;

    switch (group->type)
    {
    case CANVAS:
    case PROC:
    case FOR_INIT:
    case FOR_COND:
    case FOR_AFTERTHOUGHT:
    case FOR_PROC:
    case WHILE_COND:
    case WHILE_PROC:
    case IF_COND:
    case IF_BRANCH:
        if (group->child.empty())
        {
            group->left = 0.0;
            group->right = 0.0;
            group->top = 0.0;
            group->bottom = 0.0;
            set_node_junction(group);
            return;
        }

        for (auto p = group->child.begin(); p != group->child.end(); p++)
            set_size(*p);

        // 摆放该组的参照对象
        relative = group->child[0];
        relative->has_placed = true;

        group->left = relative->left;
        group->right = relative->right;
        group->top = relative->top;
        group->bottom = relative->bottom;

        for (size_t i = 1; i < group->child.size(); ++i)
            place_mid_group(group, group->child[i]);

        pos.clear();

        group->north = group->child[0]->north;
        group->north_coord = group->child[0]->north_coord;
        group->south = group->child[group->child.size() - 1]->south;
        group->south_coord = group->child[group->child.size() - 1]->south_coord;
        group->west = group->child[0]->west;
        group->west_coord = group->child[0]->west_coord;
        group->east = group->child[0]->east;
        group->east_coord = group->child[0]->east_coord;

        for (size_t i = 1; i < group->child.size(); ++i)
        {
            pos.clear();
            if (group->type != WHILE_PROC &&
                group->child[i - 1]->south.relatived->father != nullptr &&
                group->child[i - 1]->south.relatived->father->type == WHILE_PROC)
                continue;

            position = new Position(group->child[i - 1]->south.relatived, SOUTH, 0.0, 0.0);
            pos.push_back(*position);

            position = new Position(nullptr, BELOW, 0.0, node_distance);
            pos.push_back(*position);

            group->line.push_back(pos);
        }
        break;
    // TODO:
    case FOR:

    // TODO:while的最后增加一个点作为儿子结点，使该点作为while组的south接口，
    // 用于后面与while组的连接
    case WHILE:
        if (group->child.empty())
        {
            group->left = 0.0;
            group->right = 0.0;
            group->top = 0.0;
            group->bottom = 0.0;
            set_node_junction(group);
            return;
        }

        //while_coord = new Group(COORD, group, group->name + "coord");
        //while_coord->position.set(group->child[1], BELOW,  0.0,  node_distance / 2.0);
        
        //group->child.push_back(while_coord);
        
        for (auto p = group->child.begin(); p != group->child.end(); p++)
            set_size(*p);

        // 摆放该组的参照对象
        relative = group->child[0];
        relative->has_placed = true;

        group->left = relative->left;
        group->right = relative->right;
        group->top = relative->top;
        group->bottom = relative->bottom;

        pos.clear();
        switch (group->child.size())
        {
            case 1: // while ()死循环
                break;
            case 2:
                place_mid_group(group, group->child[0]);
                place_mid_group(group, group->child[1]);
                //place_mid_group(group, group->child[2]);

                pos.clear();
                position = new Position(group->child[1]->south.relatived,
                        SOUTH, 0.0, 0.0);
                pos.push_back(*position);
                position = new Position(nullptr, BELOW, 0.0, node_distance / 2.0 - 5.0);
                pos.push_back(*position);
                position = new Position(nullptr, LEFT, group->child[1]->south_coord.x -
                    min(group->child[0]->left, group->child[1]->left) + while_line_distance, 0.0);
                pos.push_back(*position);
                position = new Position(nullptr, ABOVE, 0.0, 0.0);
                pos.push_back(*position);
                position = new Position(group->child[0], WEST, 0.0, 0.0);
                pos.push_back(*position);
                group->line.push_back(pos);

                group->left -= while_line_distance;

                pos.clear();
                position = new Position(group->child[0], EAST, 0.0, 0.0);
                pos.push_back(*position);
                position = new Position(nullptr, RIGHT, while_line_distance +
                    max(group->child[0]->right, group->child[1]->right) - group->child[0]->right, 0.0, "no");
                pos.push_back(*position);
                position = new Position(nullptr, BELOW, 0.0, 0.0);
                pos.push_back(*position);
                position = new Position(group->child[1]->south.relatived, SOUTH, 0.0, node_distance / 2.0 + 0.0);
                pos.push_back(*position);
                position = new Position(nullptr, BELOW, 0.0, node_distance / 2.0 - 0.0);
                pos.push_back(*position);
                group->line.push_back(pos);

                group->right += while_line_distance;

                pos.clear();
                position = new Position(group->child[0]->south.relatived, SOUTH, 0.0, 0.0);
                pos.push_back(*position);
                position = new Position(nullptr, BELOW, 0.0, node_distance, "yes");
                pos.push_back(*position);
                group->line.push_back(pos);
                break;
            default:
                break;
        }

        group->north = group->child[0]->north;
        group->north_coord = group->child[0]->north_coord;
        group->south = group->child[group->child.size() - 1]->south;
        group->south_coord = group->child[group->child.size() - 1]->south_coord;
        group->west = group->child[0]->west;
        group->west_coord = group->child[0]->west_coord;
        group->east = group->child[0]->east;
        group->east_coord = group->child[0]->east_coord;

        break;
    case IF:
        if (group->child.empty())
        {
            group->left = 0.0;
            group->right = 0.0;
            group->top = 0.0;
            group->bottom = 0.0;
            set_node_junction(group);
            return;
        }

        for (auto p = group->child.begin(); p != group->child.end(); p++)
            set_size(*p);

        // 摆放该组的参照对象
        relative = group->child[0];
        relative->has_placed = true;

        group->left = relative->left;
        group->right = relative->right;
        group->top = relative->top;
        group->bottom = relative->bottom;

        pos.clear();
        switch (group->child.size())
        {
            case 2:
                place_mid_group(group, group->child[1]);
                // 把线放在右边
                if ((group->child[1]->right - group->child[0]->north_coord.x) <= 
                    (group->child[0]->north_coord.x - group->child[1]->left))
                {
                    position = new Position(group->child[0], EAST, 0.0, 0.0);
                    pos.push_back(*position);

                    position = new Position(nullptr, RIGHT,
                            group->right - relative->right + if_line_distance, 0.0, "no");
                    pos.push_back(*position);

                    position = new Position(nullptr, BELOW, 0.0, 0.0);
                    pos.push_back(*position);

                    position = new Position(group->child[1]->south.relatived, SOUTH, 0.0, node_distance / 2.0);
                    pos.push_back(*position);

                    group->line.push_back(pos);

                    group->right += if_line_distance;
                }
                else
                {
                    position = new Position(group->child[0], WEST, 0.0, 0.0);
                    pos.push_back(*position);

                    position = new Position(nullptr, LEFT,
                            relative->left - group->left + if_line_distance, 0.0, "no");
                    pos.push_back(*position);

                    position = new Position(nullptr, BELOW, 0.0, 0.0);
                    pos.push_back(*position);

                    position = new Position(group->child[1]->south.relatived, SOUTH, 0.0, node_distance / 2.0);
                    pos.push_back(*position);

                    group->line.push_back(pos);

                    group->left -= if_line_distance;
                }

                pos.clear();
                position = new Position(group->child[0]->south.relatived, SOUTH, 0.0, 0.0);
                pos.push_back(*position);

                position = new Position(nullptr, BELOW, 0.0, node_distance, "yes");
                pos.push_back(*position);

                group->line.push_back(pos);

                group->south = group->child[group->child.size() - 1]->south;
                group->south_coord = group->child[group->child.size() - 1]->south_coord;
                break;
            case 3:
                if (group->child[1]->height() < group->child[2]->height())
                {
                    p = group->child[2];
                    while (true)
                    {
                        p->position.relatived = group->child[0];
                        p->position.relation = BELOW;
                        if (!p->child.empty())
                            p = p->child[0];
                        else
                            break;
                    }
                    left_index = 2;
                    right_index = 1;
                }
                else
                {
                    left_index = 1;
                    right_index = 2;
                }
                place_mid_group(group, group->child[left_index]);

                p = group->child[right_index];
                while (true)
                {
                    // 放右边
                    if ((group->child[left_index]->right - group->child[left_index]->north_coord.x) <=
                        (group->child[left_index]->north_coord.x - group->child[left_index]->left))
                    {
                        p->position.set(group->child[0], BELOW_RIGHT,
                            group->child[right_index]->child[0]->left - group->child[right_index]->left
                                + node_distance + max(group->child[left_index]->right - group->child[0]->right, 0.0),
                            node_distance);
                    }
                    else
                    {
                        p->position.set(group->child[0], BELOW_LEFT,
                            group->child[right_index]->right - group->child[right_index]->child[0]->right
                                + node_distance + max(group->child[left_index]->child[0]->left - group->child[left_index]->left, 0.0),
                            node_distance);
                    }
                    if (!p->child.empty())
                        p = p->child[0];
                    else
                        break;
                }
                place_mid_group(group, group->child[right_index]);

                pos.clear();
                position = new Position(group->child[right_index]->south.relatived, SOUTH, 0.0, 0.0);
                pos.push_back(*position);

                position = new Position(nullptr, BELOW, 0.0, 0.0);
                pos.push_back(*position);

                position = new Position(group->child[left_index]->south.relatived, SOUTH, 0.0, node_distance / 2.0);
                pos.push_back(*position);

                group->line.push_back(pos);

                pos.clear();
                // 放右边
                if ((group->child[left_index]->right - group->child[left_index]->north_coord.x) <=
                    (group->child[left_index]->north_coord.x - group->child[left_index]->left))
                {
                    position = new Position(group->child[0]->east.relatived, EAST, 0.0, 0.0);
                    pos.push_back(*position);

                    position = new Position(nullptr, RIGHT, 0.0, 0.0, group->child[right_index]->label);
                    pos.push_back(*position);
                }
                else
                {
                    position = new Position(group->child[0]->west.relatived, WEST, 0.0, 0.0);
                    pos.push_back(*position);

                    position = new Position(nullptr, LEFT, 0.0, 0.0, group->child[right_index]->label);
                    pos.push_back(*position);
                }
                position = new Position(group->child[right_index]->north.relatived, NORTH, 0.0, 0.0);
                pos.push_back(*position);
                group->line.push_back(pos);

                pos.clear();
                position = new Position(group->child[0]->south.relatived, SOUTH, 0.0, 0.0);
                pos.push_back(*position);
                position = new Position(nullptr, BELOW, 0.0, node_distance, group->child[left_index]->label);
                pos.push_back(*position);
                group->line.push_back(pos);

                group->south = group->child[left_index]->south;
                group->south_coord = group->child[left_index]->south_coord;
                break;
            default:
                break;
        }
        pos.clear();

        group->north = group->child[0]->north;
        group->north_coord = group->child[0]->north_coord;
        group->west = group->child[0]->west;
        group->west_coord = group->child[0]->west_coord;
        group->east = group->child[0]->east;
        group->east_coord = group->child[0]->east_coord;
        break;

    case NODE_COND:
        if (node_fix_width)
        {
            // TODO: 结点固定宽度计算高度
        }
        else
        {
            width = cond_padding.left + calc_text_width(group->label) + cond_padding.right;
            height = line_height + cond_padding.top + cond_padding.bottom;

            group->left = 0.0;
            group->top = 0.0;
            group->right = width + height * 2.0;
            group->bottom = height + width / 2.0;
        }
        set_node_junction(group);
        break;
    case NODE_INPUT:
    case NODE_OUTPUT:
        if (node_fix_width)
        {
            // TODO: 结点固定宽度计算高度
        }
        else
        {
            group->left = 0.0;
            group->right = io_padding.left + calc_text_width(group->label) + group->height() / io_arctan + io_padding.right;
            group->top = 0.0;
            group->bottom = line_height + io_padding.top + io_padding.bottom;
        }
        set_node_junction(group);
        break;
    case NODE_BEGIN:
    case NODE_END:
    case NODE_PROC:
    //case NODE_COND:
        if (node_fix_width)
        {
            // TODO: 结点固定长度计算高度
        }
        else
        {
            group->left = 0.0;
            group->right = proc_padding.left + calc_text_width(group->label) + proc_padding.right;
            group->top = 0.0;
            group->bottom = line_height + proc_padding.top + proc_padding.bottom;
        }
        set_node_junction(group);
        break;
    case COORD:
        group->left = 0.0;
        group->right = 0.0;
        group->top = 0.0;
        group->bottom = 0.0;
        set_node_junction(group);
        break;
    default:
        cerr << "set_size reached default case!" << endl;
        break;
    }

}
