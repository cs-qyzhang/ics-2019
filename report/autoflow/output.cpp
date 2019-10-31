/*
 * Copyright (C) 2019 qiuyang Zhang. All rights reserved.
 * 
 * filename    : output.cpp
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

using namespace std;
extern Group canvas;
extern double node_distance;
const int comment_len = 60;

string
output_indent(int level)
{
    string result;
    for (int i = 0; i < level; ++i)
        result += "  ";
    return result;
}

string
comment(string s, int level)
{
    string result = output_indent(level);
    result += "%";
    int left_len = (comment_len - 10 * level - s.length())/ 2;
    for (int i = 0; i < left_len; ++i)
        result += '-';
    result += " " + s + " ";
    for (size_t i = 0; i < comment_len - left_len - s.length(); ++i)
        result += '-';

    return result;
}

void
output_premble(ostream &tex)
{
    tex << "\\begin{tikzpicture}[x=1pt,y=1pt]" << endl
        << comment("begin", 0) << endl
        ;
}

void
output_end(ostream &tex)
{
    tex << comment("end", 0) << endl
        << "\\end{tikzpicture}"
        ;
}

void
draw_line(vector<Position> &line, ostream &tex, int level)
{
    if (line.empty())
        return;

    Position last;
    bool first = true;

    for (auto x : line)
    {
        if (x.relatived == nullptr)
        {
            // 在描述线的走向时使用LEFT和ABOVE两种，LEFT代表-|，ABOVE代表|-
            // 当x.distance为0时描述线的走向
            if (x.xdistance == 0 && x.ydistance == 0)
                last = x;
            else
            {
                switch (x.relation)
                {
                    case LEFT:
                        tex << " -- ++(";
                        tex << "-" << x.xdistance << ",0)";
                        if (x.label != "")
                            tex << " node[pos=0.5,above]{" << x.label << "}";
                        break;
                    case RIGHT:
                        tex << " -- ++(";
                        tex << x.xdistance << ",0)";
                        if (x.label != "")
                            tex << " node[pos=0.5,above]{" << x.label << "}";
                        break;
                    case ABOVE:
                        tex << " -- ++(";
                        tex <<  "0," << x.ydistance << ")";
                        if (x.label != "")
                            tex << " node[pos=0.5,left]{" << x.label << "}";
                        break;
                    case BELOW:
                        tex << " -- ++(";
                        tex <<  "0,-" << x.ydistance << ")";
                        if (x.label != "")
                            tex << " node[pos=0.5,left]{" << x.label << "}";
                        break;
                    case ABOVE_LEFT:
                    case ABOVE_RIGHT:
                    case BELOW_LEFT:
                    case BELOW_RIGHT:
                    case CENTER:
                    case NORTH:
                    case SOUTH:
                    case WEST:
                    case EAST:
                    default:
                        cerr << "draw_line reached default case!" << endl;
                        break;
                }
            }
        }
        else
        {
            if (first)
            {
                tex << output_indent(level) <<"\\draw[->] ($(";
                first = false;
            }
            else if (last.relation == LEFT || last.relation == RIGHT)
                tex << " -| ($(";
            else
                tex << " |- ($(";

            switch (x.relation)
            {
                case NORTH:
                    tex << x.relatived->name << ".north)+(0," << x.ydistance << ")$)";
                    break;
                case SOUTH:
                    tex << x.relatived->name << ".south)+(0,-" << x.ydistance << ")$)";
                    break;
                case WEST:
                    tex << x.relatived->name << ".west)+(-" << last.xdistance << ",0)$)";
                    break;
                case EAST:
                    tex << x.relatived->name << ".east)+(" << last.xdistance << ",0)$)";
                    break;
                case LEFT:
                case RIGHT:
                case ABOVE:
                case BELOW:
                case ABOVE_LEFT:
                case ABOVE_RIGHT:
                case BELOW_LEFT:
                case BELOW_RIGHT:
                case CENTER:
                default:
                    cerr << "draw_line reached default case!" << endl;
                    break;
            }
            if (last.label != "")
                tex << " node[pos=0.25," << ((last.relation == LEFT || last.relation == RIGHT) ? "above" : "left") << "]{" << last.label << "}";
        }
    }
    tex << ";" << endl;
}

void
output_group(Group *group, ostream &tex, int level)
{
    static bool is_first = true;
    assert(group != nullptr);

#define output_node(shape)  \
{   \
    if (is_first)   \
    {   \
        tex << output_indent(level) \
            << "\\node[" << #shape << "] at (0,0)"  \
            << "(" << group->name << ")"    \
            << "{" << group->label << "};"   \
            << endl;    \
        is_first = false;   \
    }   \
    else    \
    {   \
        switch (group->position.relation)    \
        {   \
            case LEFT:  \
            case RIGHT: \
                tex << output_indent(level) \
                    << "\\node[" << #shape << ",right=" << group->position.xdistance << " of "  \
                    << group->position.relatived->south.relatived->name << ".east]" \
                    << "(" << group->name << ")"    \
                    << "{\\verb|" << group->label << "|};"   \
                    << endl;    \
                break;  \
            case ABOVE: \
            case BELOW: \
                tex << output_indent(level) \
                    << "\\node[" << #shape << ",below=" << group->position.ydistance << " of "  \
                    << group->position.relatived->south.relatived->name << ".south,local bounding box=" \
                    << group->name << "-box]" \
                    << "(" << group->name << ")"    \
                    << "{\\verb|" << group->label << "|};"   \
                    << endl;    \
                break;  \
            case ABOVE_LEFT:    \
            case ABOVE_RIGHT:   \
            case BELOW_LEFT:    \
                tex << output_indent(level) \
                    << "\\node[" << #shape << ",below left=" << group->position.ydistance << " and "  \
                    << group->position.xdistance << " of "  \
                    << group->position.relatived->south.relatived->name << "-box.south west,local bounding box="\
                    << group->name << "-box]"  \
                    << "(" << group->name << ")"    \
                    << "{\\verb|" << group->label << "|};"   \
                    << endl;    \
                break;  \
            case BELOW_RIGHT:   \
                tex << output_indent(level) \
                    << "\\node[" << #shape << ",below right=" << group->position.ydistance << " and "  \
                    << group->position.xdistance << " of "  \
                    << group->position.relatived->south.relatived->name << "-box.south east,local bounding box="\
                    << group->name << "-box]"  \
                    << "(" << group->name << ")"    \
                    << "{\\verb|" << group->label << "|};"   \
                    << endl;    \
                break;  \
            case CENTER:    \
            case NORTH: \
            case SOUTH: \
            case WEST:  \
            case EAST:  \
                break;  \
        }   \
    }   \
}

//#define output_node(shape)  \
//{   \
    //if (is_first)   \
    //{   \
        //tex << output_indent(level) \
            //<< "\\node[" << #shape << "] at (0,0)"  \
            //<< "(" << group->name << ")"    \
            //<< "{" << group->label << "};"   \
            //<< endl;    \
        //is_first = false;   \
    //}   \
    //else    \
    //{   \
        //switch (group->position.relation)    \
        //{   \
            //case LEFT:  \
            //case RIGHT: \
                //tex << output_indent(level) \
                    //<< "\\node[" << #shape << ",right=" << group->position.xdistance << " of "  \
                    //<< group->position.relatived->south.relatived->name << ".east]" \
                    //<< "(" << group->name << ")"    \
                    //<< "{" << group->label << "};"   \
                    //<< endl;    \
                //tex << output_indent(level) \
                    //<< "\\draw[->] (" << group->position.relatived->south.relatived->name << ".east)"\
                    //<< ((group->position.ydistance == 0.0) ? " -- " : " -| ") << "(" << group->name << ".west);"  \
                    //<< endl;    \
                //break;  \
            //case ABOVE: \
            //case BELOW: \
                //tex << output_indent(level) \
                    //<< "\\node[" << #shape << ",below=" << group->position.ydistance << " of "  \
                    //<< group->position.relatived->south.relatived->name << ".south,local bounding box=" \
                    //<< group->name << "-box]" \
                    //<< "(" << group->name << ")"    \
                    //<< "{" << group->label << "};"   \
                    //<< endl;    \
                //tex << output_indent(level) \
                    //<< "\\draw[->] (" << group->position.relatived->south.relatived->name << ".south)"\
                    //<< ((group->position.xdistance == 0.0) ? " -- " : " |- ") << "(" << group->name << ".north);"  \
                    //<< endl;    \
                //break;  \
            //case ABOVE_LEFT:    \
            //case ABOVE_RIGHT:   \
            //case BELOW_LEFT:    \
                //tex << output_indent(level) \
                    //<< "\\node[" << #shape << ",below left=" << group->position.ydistance << " and "  \
                    //<< group->position.xdistance << " of "  \
                    //<< group->position.relatived->south.relatived->name << "-box.south west,local bounding box="\
                    //<< group->name << "-box]"  \
                    //<< "(" << group->name << ")"    \
                    //<< "{" << group->label << "};"   \
                    //<< endl;    \
                //tex << output_indent(level) \
                    //<< "\\draw[->] (" << group->position.relatived->south.relatived->name << ".west)"\
                    //<< " -| " << "(" << group->name << ".north);"  \
                    //<< endl;    \
                //break;  \
            //case BELOW_RIGHT:   \
                //tex << output_indent(level) \
                    //<< "\\node[" << #shape << ",below right=" << group->position.ydistance << " and "  \
                    //<< group->position.xdistance << " of "  \
                    //<< group->position.relatived->south.relatived->name << "-box.south east,local bounding box="\
                    //<< group->name << "-box]"  \
                    //<< "(" << group->name << ")"    \
                    //<< "{" << group->label << "};"   \
                    //<< endl;    \
                //tex << output_indent(level) \
                    //<< "\\draw[->] (" << group->position.relatived->south.relatived->name << ".east)"\
                    //<< " -| " << "(" << group->name << ".north);"  \
                    //<< endl;    \
                //break;  \
            //case CENTER:    \
            //case NORTH: \
            //case SOUTH: \
            //case WEST:  \
            //case EAST:  \
                //break;  \
        //}   \
    //}   \
//}

#define output_coord()  \
{   \
    if (is_first)   \
    {   \
        tex << output_indent(level) \
            << "\\coordinate (" << group->name << ") at (0,0);" \
            << endl;    \
        is_first = false;   \
    }   \
    tex << output_indent(level) \
        << "\\coordinate[" << group->position.rela_str() << "=" << node_distance / 2.0 << "of "  \
        << group->position.relatived->south.relatived->name << "] "  \
        << "(" << group->name << ");"    \
        << endl;    \
}

// TODO: scope中的属性设置
#define output_begin_scope()  \
{   \
    tex << output_indent(level) \
        << "{" \
        << endl;    \
}

#define output_end_scope()  \
{   \
    tex << output_indent(level) \
        << "}" \
        << endl;    \
}

    if (group->src_line)
        tex << output_indent(level)
            << comment("line " + to_string(group->src_line) + ": " + group->src,
                level)
            << endl;

    switch (group->type)
    {
        case CANVAS:
        case WHILE_COND:
        case WHILE_PROC:
        case IF_COND:
        case IF_BRANCH:
        case FOR_INIT:
        case FOR_COND:
        case FOR_AFTERTHOUGHT:
        case FOR_PROC:
        case PROC:
        case FOR:
        case WHILE:
        case IF:
            output_begin_scope();
            for (auto x : group->child)
                output_group(x, tex, level + 1);
            for (auto x : group->line)
                draw_line(x, tex, level + 1);
            output_end_scope();
            break;
        case NODE_PROC:
            output_node(process);
            break;
        case NODE_INPUT:
        case NODE_OUTPUT:
            output_node(io);
            break;
        case NODE_COND:
            output_node(decision);
            //tex << "\\draw[->] (" << group->name << ".west) -- ++(" << group->width() << ",0);" << endl;
            break;
        case COORD:
            output_coord();
            break;
        case NODE_BEGIN:
        case NODE_END:
            output_node(startstop);
            break;
        default:
            cerr << "output_group: reached default case!" << endl;
            break;
    }
}

void
output(string file_name)
{
    output_premble(cout);

    output_group(&canvas, cout, 0);

    output_end(cout);

}
