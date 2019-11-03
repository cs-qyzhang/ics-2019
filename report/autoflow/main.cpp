/*
 * Copyright (C) 2019 qiuyang Zhang. All rights reserved.
 *
 * filename    : main.cpp
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
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stack>
#include <cassert>
#include <cstring>
#include "group.h"
#include "position.h"

using namespace std;
Group canvas;
const char separator = '-';
Group *last_group = &canvas;
extern double node_distance;
string default_output_file_name = "out.tex";

void set_size(Group *);
void output(string);

enum LineType
{
    LINE_ERROR,
    LINE_BEGIN,
    LINE_END,
    LINE_GROUP_BEGIN,
    LINE_GROUP_END,
    LINE_PROC,
    LINE_INPUT,
    LINE_OUTPUT,
    LINE_IF,
    LINE_ELSE,
    LINE_ELSE_IF,
    LINE_FOR,
    LINE_WHILE
};

enum State
{
    STATE_IF_BEGIN,
    STATE_IF_WAIT,
    STATE_IF_BLOCK,
    STATE_ELSE_BEGIN,
    STATE_ELSE_WAIT,
    STATE_ELSE_BLOCK,
    STATE_FOR_BEGIN,
    STATE_FOR_WAIT,
    STATE_FOR_BLOCK,
    STATE_WHILE_BEGIN,
    STATE_WHILE_WAIT,
    STATE_WHILE_BLOCK,
    STATE_PROC
};

// borrow from stackoverflow
// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

LineType line_type(string &s)
{
    assert(s.empty() == false);
    if (s == "{")
        return LINE_GROUP_BEGIN;
    else if (s == "}")
        return LINE_GROUP_END;
    else if (s.length() >= 2 && s[0] == '"')
    {
        if (s[s.length() - 1] == '"')
            return LINE_PROC;
        else
            return LINE_ERROR;
    }
    else if (s.length() >= 5 && s.substr(0, 5) == "begin")
        return LINE_BEGIN;
    else if (s.length() >= 3 && s.substr(0, 3) == "end")
        return LINE_END;
    else if (s.length() >= 5 && s.substr(0, 5) == "input")
    {
        if (s.length() >= 6 && s[5] != ' ')
            return LINE_PROC;
        else
            return LINE_INPUT;
    }
    else if (s.length() >= 6 && s.substr(0, 6) == "output")
    {
        if (s.length() >= 7 && s[6] != ' ')
            return LINE_PROC;
        else
            return LINE_OUTPUT;
    }
    else if (s.length() >= 4 && s.substr(0, 2) == "if")
    {
        if (s[2] == ' ' || s[2] == '(')
            return LINE_IF;
        else
            return LINE_PROC;
    }
    else if (s.length() == 4 && s == "else")
        return LINE_ELSE;
    else if (s.length() >= 7 && s.substr(0, 3) == "for")
    {
        if (s[3] == ' ' || s[3] == '(')
            return LINE_FOR;
        else
            return LINE_PROC;
    }
    else if (s.length() >= 7 && s.substr(0, 5) == "while")
    {
        if (s[5] == ' ' || s[5] == '(')
            return LINE_WHILE;
        else
            return LINE_PROC;
    }
    else
        return LINE_PROC;
}

string
string_to_label(string &s)
{
    trim(s);
    if (s.length() >= 2 && s[0] == '"')
    {
        if (s[s.length() - 1] != '"')
        {
            // FIXME:
            cerr << "string_to_label: no match quote!" << endl;
            return "";
        }
        return (s = s.substr(1, s.length() - 2));
    }
    return s;
}

string
extract_label(string &s, LineType type)
{
    string local_s = s;

    switch (type)
    {
    case LINE_PROC:
        return string_to_label(local_s);
    case LINE_INPUT:
        local_s = local_s.substr(5);
        trim(local_s);
        return "输入" + string_to_label(local_s);
    case LINE_OUTPUT:
        local_s = local_s.substr(6);
        trim(local_s);
        return "输出" + string_to_label(local_s);
    case LINE_ERROR:
    case LINE_BEGIN:
        return "开始";
    case LINE_END:
        return "结束";
    case LINE_GROUP_BEGIN:
    case LINE_GROUP_END:
    case LINE_IF:
    case LINE_ELSE:
    case LINE_FOR:
    case LINE_WHILE:
    default:
        cerr << "extract_label reached default case!" << endl;
        return "";
    }
}

// TODO:group name
Group *
get_group(string &s, GroupType type, string &prefix)
{
    Group *mid_group, *node;
    size_t p, q;
    string temp, label;

#define get_group_creat_mid_and_node_group(typ, str, node_typ)  \
{   \
    mid_group = new Group;  \
    mid_group->type = typ;  \
    mid_group->name = prefix + #str;  \
    node = new Group;   \
    node->type = NODE_##node_typ; \
    node->name = prefix + #str;   \
    node->label = label;    \
    node->father = mid_group;   \
    node->position.set(last_group, BELOW, 0.0, node_distance);  \
    mid_group->child.push_back(node);   \
    mid_group->position.set(last_group, BELOW, 0.0, node_distance);  \
    last_group = mid_group; \
}

    // TODO: 逗号表达式？
    switch (type)
    {
    case IF_COND:
        p = s.find_first_of('(');
        assert(p != string::npos);
        q = s.find_last_of(')');
        assert(q != string::npos);
        temp = s.substr(p + 1, q - p - 1);
        label = extract_label(temp, LINE_PROC);

        get_group_creat_mid_and_node_group(IF_COND, cond, COND);
        break;
    case FOR_INIT:
        p = s.find_first_of('(');
        assert(p != string::npos);
        q = s.find_first_of(';');
        assert(q != string::npos);
        temp = s.substr(p + 1, q - p - 1);
        label = extract_label(temp, LINE_PROC);

        get_group_creat_mid_and_node_group(FOR_INIT, init, PROC);
        break;
    case FOR_COND:
        p = s.find(';');
        assert(p != string::npos);
        q = s.find(';', p + 1);
        assert(q != string::npos);
        temp = s.substr(p + 1, q - p - 1);
        label = extract_label(temp, LINE_PROC);

        get_group_creat_mid_and_node_group(FOR_COND, cond, COND);
        break;
    case FOR_AFTERTHOUGHT:
        p = s.find_last_of(';');
        assert(p != string::npos);
        q = s.find_last_of(')');
        assert(q != string::npos);
        temp = s.substr(p + 1, q - p - 1);
        label = extract_label(temp, LINE_PROC);

        get_group_creat_mid_and_node_group(FOR_AFTERTHOUGHT, after, PROC);
        break;
    case WHILE_COND:
        p = s.find('(');
        assert(p != string::npos);
        q = s.find_last_of(')');
        assert(q != string::npos);
        temp = s.substr(p + 1, q - p - 1);
        label = extract_label(temp, LINE_PROC);

        get_group_creat_mid_and_node_group(WHILE_COND, cond, COND);
        break;
    default:
        mid_group = nullptr;
        cout << "get_group reached default case!" << endl;
        break;
    }
    return mid_group;
}

string
state_name(State state)
{
    switch (state)
    {
    case STATE_IF_BEGIN:
        return "STATE_IF_BEGIN";
    case STATE_IF_WAIT:
        return "STATE_IF_WAIT";
    case STATE_IF_BLOCK:
        return "STATE_IF_BLOCK";
    case STATE_ELSE_BEGIN:
        return "STATE_ELSE_BEGIN";
    case STATE_ELSE_WAIT:
        return "STATE_ELSE_WAIT";
    case STATE_ELSE_BLOCK:
        return "STATE_ELSE_BLOCK";
    case STATE_FOR_BEGIN:
        return "STATE_FOR_BEGIN";
    case STATE_FOR_WAIT:
        return "STATE_FOR_WAIT";
    case STATE_FOR_BLOCK:
        return "STATE_FOR_BLOCK";
    case STATE_WHILE_BEGIN:
        return "STATE_WHILE_BEGIN";
    case STATE_WHILE_WAIT:
        return "STATE_WHILE_WAIT";
    case STATE_WHILE_BLOCK:
        return "STATE_WHILE_BLOCK";
    case STATE_PROC:
        return "STATE_PROC";
    default:
        return "";
    }
}

// TODO: 把进入新组时修改now_group放到状态机中？
bool
creat_group(istream &src)
{
    int line_count = 0;
    string line;

    Group *now_group = &canvas;

    Group *top_group, *mid_group, *node_group;
    top_group = mid_group = node_group = nullptr;


    stack<State> state;
    State now_state = STATE_PROC;

    int local_cnt_IF, local_cnt_WHILE, local_cnt_FOR, local_cnt_PROC,
        local_cnt_INPUT, local_cnt_OUTPUT, local_cnt_BEGIN, local_cnt_END;

#define clear_local_cnt() \
{   \
    local_cnt_IF = local_cnt_WHILE = local_cnt_FOR = local_cnt_PROC = 0;  \
    local_cnt_INPUT = local_cnt_OUTPUT = local_cnt_BEGIN = local_cnt_END = 0; \
}

#define creat_group_name(x) \
        now_group->name+separator+#x+to_string(local_cnt_##x++)

#define line_type_err(x)    \
{   \
    cerr << x << " in line" << line_count << "! exit." << endl; \
    src.close();    \
    exit(-1);   \
}

#define creat_new_top_grop(typ)  \
{   \
    top_group = new Group(typ, now_group);   \
    top_group->name = creat_group_name(typ); \
    now_group->child.push_back(top_group);  \
    now_group = top_group;                  \
    top_group->position.set(last_group, BELOW, 0.0, node_distance);   \
}

#define creat_new_mid_group(typ)   \
{   \
    mid_group = new Group(typ, now_group);     \
    now_group->child.push_back(mid_group);      \
    mid_group->name = now_group->name + "proc"; \
    now_group = mid_group;                      \
    mid_group->position.set(last_group, BELOW, 0.0, node_distance);   \
}

#define get_new_mid_group(type)  \
{   \
    mid_group = get_group(line, type, top_group->name);    \
    now_group->child.push_back(mid_group);              \
    mid_group->father = now_group;                      \
}

#define creat_new_node_group(typ)  \
{   \
    node_group = new Group(NODE_##typ, now_group);         \
    node_group->name = creat_group_name(typ);              \
    node_group->label = extract_label(line, LINE_##typ);   \
    node_group->position.set(last_group, BELOW, 0.0, node_distance);   \
    last_group = node_group;  \
    now_group->child.push_back(node_group);                 \
}

#define creat_new_if_group()    \
{   \
    /* 新建一个顶层if group */  \
    creat_new_top_grop(IF); \
    /* 设置if中的condition group作为if group的子group */  \
    get_new_mid_group(IF_COND); \
    /* 设置if中的group作为if group的子group
     并将其设为当前的顶层group */   \
    creat_new_mid_group(IF_BRANCH); \
    mid_group->label = "是";  \
}

#define creat_new_for_group()   \
{   \
    creat_new_top_grop(FOR); \
    get_new_mid_group(FOR_INIT); \
    get_new_mid_group(FOR_COND); \
    get_new_mid_group(FOR_AFTERTHOUGHT); \
    creat_new_mid_group(FOR_PROC); \
    mid_group->label = "for";  \
}

#define creat_new_while_group()   \
{   \
    creat_new_top_grop(WHILE); \
    get_new_mid_group(WHILE_COND); \
    creat_new_mid_group(WHILE_PROC); \
    mid_group->label = "while";  \
}

// FIXME:libc++ bug ??
#define is_wait_state() \
        now_state == STATE_IF_WAIT || \
        now_state == STATE_ELSE_WAIT || \
        now_state == STATE_FOR_WAIT || \
        now_state == STATE_WHILE_WAIT

#define is_top_group()  \
        now_group->type == PROC || \
        now_group->type == IF || \
        now_group->type == FOR || \
        now_group->type == WHILE

#define is_mid_group()  \
        now_group->type == IF_COND || \
        now_group->type == IF_BRANCH || \
        now_group->type == FOR_COND || \
        now_group->type == FOR_INIT || \
        now_group->type == FOR_AFTERTHOUGHT || \
        now_group->type == FOR_PROC || \
        now_group->type == WHILE_COND || \
        now_group->type == WHILE_PROC

#define pop_state() \
{   \
    last_group = now_group; \
    now_state = state.top();    \
    state.pop();    \
    now_group = now_group->father;  \
}

    clear_local_cnt();

    while (getline(src, line))
    {
        line_count++;
        trim(line);

        LineType type = line_type(line);

change_state_again:
        switch (now_state)
        {
        case STATE_IF_BEGIN:
            switch (type)
            {
            case LINE_GROUP_BEGIN:
                now_state = STATE_IF_BLOCK;
                break;
            case LINE_PROC:
            case LINE_END:      // FIXME
                now_state = STATE_IF_WAIT;
                creat_new_node_group(PROC);
                break;
            case LINE_INPUT:
                now_state = STATE_IF_WAIT;
                creat_new_node_group(INPUT);
                break;
            case LINE_OUTPUT:
                now_state = STATE_IF_WAIT;
                creat_new_node_group(OUTPUT);
                break;
            case LINE_IF:
                state.push(STATE_IF_WAIT);
                now_state = STATE_IF_BEGIN;
                creat_new_if_group();
                break;
            case LINE_FOR:
                state.push(STATE_IF_WAIT);
                now_state = STATE_FOR_BEGIN;
                creat_new_for_group();
                break;
            case LINE_WHILE:
                state.push(STATE_IF_WAIT);
                now_state = STATE_WHILE_BEGIN;
                creat_new_while_group();
                break;
            case LINE_ERROR:
            case LINE_BEGIN:
            case LINE_GROUP_END:
            case LINE_ELSE:
            case LINE_ELSE_IF:
            default:
                cerr << "state fsm: case STATE_IF_BEGIN!" << line_count << endl;
                return false;
            }
            break;
        case STATE_IF_WAIT: // TODO：所有的WAIT状态的GROUP_BEGIN 和 GROUP_END 需要处理
            switch (type)
            {
            case LINE_ELSE:
                now_state = STATE_ELSE_BEGIN;
                //last_group = now_group;
                now_group = now_group->father;
                creat_new_mid_group(IF_BRANCH);
                mid_group->label = "否";
                break;
            case LINE_ELSE_IF:  // FIXME:没有处理else if的条件，没有产生cond node
                now_state = STATE_IF_BEGIN;
                //last_group = now_group;
                now_group = now_group->father;
                creat_new_mid_group(IF_BRANCH);
                mid_group->label = "else if";
                break;
            case LINE_PROC:
            case LINE_END:      // FIXME
            case LINE_INPUT:
            case LINE_OUTPUT:
            case LINE_GROUP_END:
            case LINE_IF:   // 开始一个新的 if
            case LINE_FOR:
            case LINE_WHILE:
                assert(now_group->type == IF_BRANCH);
                now_group = now_group->father;
                pop_state();
                goto change_state_again;
                break;
            case LINE_ERROR:
            case LINE_BEGIN:
            case LINE_GROUP_BEGIN:
            default:
                cerr << "state fsm: case STATE_IF_WAIT!" << line_count << endl;
                return false;
            }
            break;
        case STATE_IF_BLOCK:
            switch (type)
            {
            case LINE_GROUP_BEGIN:
                state.push(now_state);
                now_state = STATE_PROC;
                creat_new_top_grop(PROC);
                break;
            case LINE_PROC:
            case LINE_END:
                creat_new_node_group(PROC);
                break;
            case LINE_INPUT:
                creat_new_node_group(INPUT);
                break;
            case LINE_OUTPUT:
                creat_new_node_group(OUTPUT);
                break;
            case LINE_GROUP_END:
                now_state = STATE_IF_WAIT;
                break;
            case LINE_IF:
                state.push(now_state);
                now_state = STATE_IF_BEGIN;
                creat_new_if_group();
                break;
            case LINE_FOR:
                state.push(now_state);
                now_state = STATE_FOR_BEGIN;
                creat_new_for_group();
                break;
            case LINE_WHILE:
                state.push(now_state);
                now_state = STATE_WHILE_BEGIN;
                creat_new_while_group();
                break;
            case LINE_ELSE:
            case LINE_ELSE_IF:
            case LINE_ERROR:
            case LINE_BEGIN:
            default:
                cerr << "state fsm: case STATE_IF_BLOCK!" << line_count << endl;
                return false;
            }
            break;
        case STATE_ELSE_BEGIN:
            switch (type)
            {
            case LINE_GROUP_BEGIN:
                now_state = STATE_ELSE_BLOCK;
                break;
            case LINE_PROC:
            case LINE_END:      // FIXME
                now_state = STATE_ELSE_WAIT;
                creat_new_node_group(PROC);
                break;
            case LINE_INPUT:
                now_state = STATE_ELSE_WAIT;
                creat_new_node_group(INPUT);
                break;
            case LINE_OUTPUT:
                now_state = STATE_ELSE_WAIT;
                creat_new_node_group(OUTPUT);
                break;
            case LINE_IF:
                state.push(STATE_ELSE_WAIT);
                now_state = STATE_IF_BEGIN;
                creat_new_if_group();
                break;
            case LINE_FOR:
                state.push(STATE_ELSE_WAIT);
                now_state = STATE_FOR_BEGIN;
                creat_new_for_group();
                break;
            case LINE_WHILE:
                state.push(STATE_ELSE_WAIT);
                now_state = STATE_WHILE_BEGIN;
                creat_new_while_group();
                break;
            case LINE_ERROR:
            case LINE_BEGIN:
            case LINE_GROUP_END:
            case LINE_ELSE:
            case LINE_ELSE_IF:
            default:
                cerr << "state fsm: case STATE_ELSE_BEGIN!" << line_count << endl;
                return false;
            }
            break;
        case STATE_ELSE_WAIT:
            switch (type)
            {
            case LINE_PROC:
            case LINE_END:      // FIXME
            case LINE_INPUT:
            case LINE_OUTPUT:
            case LINE_IF:   // 开始一个新的 if
            case LINE_FOR:
            case LINE_WHILE:
            case LINE_GROUP_END:
                assert(now_group->type == IF_BRANCH);
                now_group = now_group->father;
                pop_state();
                goto change_state_again;
                break;
            case LINE_ELSE_IF:
            case LINE_ERROR:
            case LINE_BEGIN:
            case LINE_GROUP_BEGIN:
            case LINE_ELSE:
            default:
                cerr << "state fsm: case STATE_ELSE_WAIT!" << line_count << endl;
                return false;
            }
            break;
        case STATE_ELSE_BLOCK:
            switch (type)
            {
            case LINE_GROUP_BEGIN:
                state.push(now_state);
                now_state = STATE_PROC;
                creat_new_top_grop(PROC);
                break;
            case LINE_PROC:
            case LINE_END:
                creat_new_node_group(PROC);
                break;
            case LINE_INPUT:
                creat_new_node_group(INPUT);
                break;
            case LINE_OUTPUT:
                creat_new_node_group(OUTPUT);
                break;
            case LINE_GROUP_END:
                assert(now_group->type == IF_BRANCH);
                now_group = now_group->father;
                pop_state();
                break;
            case LINE_IF:
                state.push(now_state);
                now_state = STATE_IF_BEGIN;
                creat_new_if_group();
                break;
            case LINE_FOR:
                state.push(now_state);
                now_state = STATE_FOR_BEGIN;
                creat_new_for_group();
                break;
            case LINE_WHILE:
                state.push(now_state);
                now_state = STATE_WHILE_BEGIN;
                creat_new_while_group();
                break;
            case LINE_ELSE:
            case LINE_ELSE_IF:
            case LINE_ERROR:
            case LINE_BEGIN:
            default:
                cerr << "state fsm: case STATE_ELSE_BLOCK!" << line_count << endl;
                return false;
            }
            break;
        case STATE_FOR_BEGIN:
            switch (type)
            {
            case LINE_GROUP_BEGIN:
                now_state = STATE_FOR_BLOCK;
                break;
            case LINE_PROC:
            case LINE_END:      // FIXME
                now_state = STATE_FOR_WAIT;
                creat_new_node_group(PROC);
                break;
            case LINE_INPUT:
                now_state = STATE_FOR_WAIT;
                creat_new_node_group(INPUT);
                break;
            case LINE_OUTPUT:
                now_state = STATE_FOR_WAIT;
                creat_new_node_group(OUTPUT);
                break;
            case LINE_IF:
                state.push(STATE_FOR_WAIT);
                now_state = STATE_IF_BEGIN;
                creat_new_if_group();
                break;
            case LINE_FOR:
                state.push(STATE_FOR_WAIT);
                now_state = STATE_FOR_BEGIN;
                creat_new_for_group();
                break;
            case LINE_WHILE:
                state.push(STATE_FOR_WAIT);
                now_state = STATE_WHILE_BEGIN;
                creat_new_while_group();
                break;
            case LINE_ERROR:
            case LINE_BEGIN:
            case LINE_GROUP_END:
            case LINE_ELSE:
            case LINE_ELSE_IF:
            default:
                cerr << "state fsm: case STATE_FOR_BEGIN!" << line_count << endl;
                return false;
            }
            break;
        case STATE_FOR_WAIT:
            switch (type)
            {
            case LINE_PROC:
            case LINE_END:      // FIXME
            case LINE_INPUT:
            case LINE_OUTPUT:
            case LINE_IF:   // 开始一个新的 if
            case LINE_FOR:
            case LINE_WHILE:
                assert(now_group->type == FOR_PROC);
                now_group = now_group->father;
                pop_state();
                goto change_state_again;
                break;
            case LINE_ELSE:
            case LINE_ELSE_IF:
            case LINE_ERROR:
            case LINE_BEGIN:
            case LINE_GROUP_BEGIN:
            case LINE_GROUP_END:
            default:
                cerr << "state fsm: case STATE_FOR_WAIT!" << line_count << endl;
                return false;
            }
            break;
        case STATE_FOR_BLOCK:
            switch (type)
            {
            case LINE_GROUP_BEGIN:
                state.push(now_state);
                now_state = STATE_PROC;
                creat_new_top_grop(PROC);
                break;
            case LINE_PROC:
            case LINE_END:
                now_state = STATE_FOR_BLOCK;
                creat_new_node_group(PROC);
                break;
            case LINE_INPUT:
                now_state = STATE_FOR_BLOCK;
                creat_new_node_group(INPUT);
                break;
            case LINE_OUTPUT:
                now_state = STATE_FOR_BLOCK;
                creat_new_node_group(OUTPUT);
                break;
            case LINE_GROUP_END:
                assert(now_group->type == WHILE_PROC);
                now_group = now_group->father;
                pop_state();
                break;
            case LINE_IF:
                state.push(now_state);
                now_state = STATE_IF_BEGIN;
                creat_new_if_group();
                break;
            case LINE_FOR:
                state.push(now_state);
                now_state = STATE_FOR_BEGIN;
                creat_new_for_group();
                break;
            case LINE_WHILE:
                state.push(now_state);
                now_state = STATE_WHILE_BEGIN;
                creat_new_while_group();
                break;
            case LINE_ELSE:
            case LINE_ELSE_IF:
            case LINE_ERROR:
            case LINE_BEGIN:
            default:
                cerr << "state fsm: case STATE_FOR_BLOCK!" << line_count << endl;
                return false;
            }
            break;
        case STATE_WHILE_BEGIN:
            switch (type)
            {
            case LINE_GROUP_BEGIN:
                now_state = STATE_WHILE_BLOCK;
                break;
            case LINE_PROC:
            case LINE_END:      // FIXME
                now_state = STATE_WHILE_WAIT;
                creat_new_node_group(PROC);
                break;
            case LINE_INPUT:
                now_state = STATE_WHILE_WAIT;
                creat_new_node_group(INPUT);
                break;
            case LINE_OUTPUT:
                now_state = STATE_WHILE_WAIT;
                creat_new_node_group(INPUT);
                break;
            case LINE_IF:
                state.push(STATE_WHILE_WAIT);
                now_state = STATE_IF_BEGIN;
                creat_new_if_group();
                break;
            case LINE_FOR:
                state.push(STATE_WHILE_WAIT);
                now_state = STATE_FOR_BEGIN;
                creat_new_for_group();
                break;
            case LINE_WHILE:
                state.push(STATE_WHILE_WAIT);
                now_state = STATE_WHILE_BEGIN;
                creat_new_while_group();
                break;
            case LINE_ERROR:
            case LINE_BEGIN:
            case LINE_GROUP_END:
            case LINE_ELSE:
            case LINE_ELSE_IF:
            default:
                cerr << "state fsm: case STATE_WHILE_BEGIN!" << line_count << endl;
                return false;
            }
            break;
        case STATE_WHILE_WAIT:
            switch (type)
            {
            case LINE_PROC:
            case LINE_END:      // FIXME
            case LINE_INPUT:
            case LINE_OUTPUT:
            case LINE_IF:   // 开始一个新的 if
            case LINE_FOR:
            case LINE_WHILE:
                assert(now_group->type == WHILE_PROC);
                now_group = now_group->father;
                pop_state();
                goto change_state_again;
                break;
            case LINE_ELSE:
            case LINE_ELSE_IF:
            case LINE_ERROR:
            case LINE_BEGIN:
            case LINE_GROUP_BEGIN:
            case LINE_GROUP_END:
            default:
                cerr << "state fsm: case STATE_WHILE_WAIT!" << line_count << endl;
                return false;
            }
            break;
        case STATE_WHILE_BLOCK:
            switch (type)
            {
            case LINE_GROUP_BEGIN:
                state.push(now_state);
                now_state = STATE_PROC;
                creat_new_top_grop(PROC);
                break;
            case LINE_PROC:
            case LINE_END:
                creat_new_node_group(PROC);
                break;
            case LINE_INPUT:
                creat_new_node_group(INPUT);
                break;
            case LINE_OUTPUT:
                creat_new_node_group(OUTPUT);
                break;
            case LINE_GROUP_END:
                assert(now_group->type == WHILE_PROC);
                now_group = now_group->father;
                pop_state();
                break;
            case LINE_IF:
                state.push(now_state);
                now_state = STATE_IF_BEGIN;
                creat_new_if_group();
                break;
            case LINE_FOR:
                state.push(now_state);
                now_state = STATE_FOR_BEGIN;
                creat_new_for_group();
                break;
            case LINE_WHILE:
                state.push(now_state);
                now_state = STATE_WHILE_BEGIN;
                creat_new_while_group();
                break;
            case LINE_ELSE:
            case LINE_ELSE_IF:
            case LINE_ERROR:
            case LINE_BEGIN:
            default:
                cerr << "state fsm: case STATE_WHILE_BLOCK!" << line_count << endl;
                return false;
            }
            break;
        case STATE_PROC:
            switch (type)
            {
            case LINE_GROUP_BEGIN:
                state.push(now_state);
                creat_new_top_grop(PROC);
                break;
            case LINE_PROC:
                creat_new_node_group(PROC);
                break;
            case LINE_END:
                creat_new_node_group(END);
                break;
            case LINE_BEGIN:
                creat_new_node_group(BEGIN);
                break;
            case LINE_INPUT:
                creat_new_node_group(INPUT);
                break;
            case LINE_OUTPUT:
                creat_new_node_group(OUTPUT);
                break;
            case LINE_GROUP_END:
                pop_state();
                break;
            case LINE_IF:
                state.push(now_state);
                now_state = STATE_IF_BEGIN;
                creat_new_if_group();
                break;
            case LINE_FOR:
                state.push(now_state);
                now_state = STATE_FOR_BEGIN;
                creat_new_for_group();
                break;
            case LINE_WHILE:
                state.push(now_state);
                now_state = STATE_WHILE_BEGIN;
                creat_new_while_group();
                break;
            case LINE_ELSE:
            case LINE_ELSE_IF:
            case LINE_ERROR:
            default:
                cerr << "state fsm: case STATE_PROC!" << line_count << endl;
                return false;
            }
            break;
        default:
            cerr << "state fsm: reach default case!" << line_count << endl;
            return false;
        }

        //cout << state_name(now_state) << endl;
    }
    return true;
}

void
recursive_print_group(Group *group, int level)
{
    for (int i = 0; i < level; ++i)
        cout << "    ";
    cout << group->name << " " << group->label << " " << group->height() << " " << group->width()
         << " " << group->north_coord.str() << " " << group->south_coord.str() << " "
         << group->left << " " << group->right << " "
         //<< ((group->position.relatived != nullptr) ? group->position.relatived->name : "") << " "
         //<< ((group->position.relatived != nullptr) ? group->position.relatived->label : "") << " "
         << group->south.relatived->name << " "
         << group->line.size() << " "
         << endl;
    for (auto i : group->child)
        recursive_print_group(i, level + 1);
}

int
main(int argc, char **argv)
{

    canvas.father = nullptr;
    canvas.type = CANVAS;
    canvas.name = "0";
    canvas.position.set(&canvas, CENTER, 0.0, 0.0);

    ifstream src_file;
    fstream tex_file;

    src_file.open(argv[1]);
    if (!src_file)
    {
        cerr << "Can't open srcfile " << argv[1] << "!" << endl;
        return -1;
    }
    creat_group(src_file);

    set_size(&canvas);
    //recursive_print_group(&canvas, 0);

    output("");
    return 0;
}
