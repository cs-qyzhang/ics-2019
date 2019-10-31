/*
 * Copyright (C) 2019 qiuyang Zhang. All rights reserved.
 * 
 * filename    : position.h
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
#ifndef POSITION_H
#define POSITION_H

struct Group;

enum Relation
{
    // node relation
    LEFT,       // 在描述线的走向时使用LEFT和ABOVE两种，LEFT代表-|，ABOVE代表|-
    RIGHT,
    ABOVE,
    BELOW,
    ABOVE_LEFT,
    ABOVE_RIGHT,
    BELOW_LEFT,
    BELOW_RIGHT,

    // node anchor
    CENTER,
    NORTH,
    SOUTH,
    WEST,
    EAST
};

struct Position
{
    Group *relatived;
    Relation relation;
    double xdistance, ydistance;
    std::string label;

    bool connect_position(const Position &, const Position &);
    std::string get_pos();

    Position(Group *relatived = nullptr, Relation relation = BELOW, double xdistance = 0.0, double ydistance = 0.0, std::string label = "") :
        relatived(relatived), relation(relation), xdistance(xdistance), ydistance(ydistance), label(label)
    {
    }

    void set(Group *relatived = nullptr, Relation relation = BELOW, double xdistance = 0.0, double ydistance = 0.0)
    {
        this->relatived = relatived;
        this->relation = relation;
        this->xdistance = xdistance;
        this->ydistance = ydistance;
    }

    std::string rela_str()
    {
        switch (relation)
        {
            case LEFT:
                return "left";
            case RIGHT:
                return "right";
            case ABOVE:
                return "above";
            case BELOW:
                return "below";
            case ABOVE_LEFT:
                return "above left";
            case ABOVE_RIGHT:
                return "above right";
            case BELOW_LEFT:
                return "below left";
            case BELOW_RIGHT:
                return "below right";
            case CENTER:
                return "center";
            case NORTH:
                return "north";
            case SOUTH:
                return "south";
            case WEST:
                return "west";
            case EAST:
                return "east";
            default:
                return "";
        }
    }
};

struct Coord
{
    double x;
    double y;

    void set(double x, double y)
    {
        this->x = x;
        this->y = y;
    }

    std::string str()
    {
        return "(" + std::to_string(x) + "," + std::to_string(y) + ")";
    }

    Coord(double x = 0.0, double y = 0.0) :
        x(x), y(y)
  {
  }

    Coord &operator+(const Coord &a)
    {
        Coord *t = new Coord(this->x + a.x, this->y + a.y);
        return *t;
    }

    Coord &operator-(const Coord &a)
    {
        Coord *t = new Coord(this->x - a.x, this->y - a.y);
        return *t;
    }
};

#endif // POSITION_H
