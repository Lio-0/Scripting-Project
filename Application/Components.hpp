#pragma once
#include <stdlib.h>

struct c_Vector {

    float x, y, z;
};

struct c_Transform {
    c_Vector position, rotation, scale;
};

struct c_Collision {
    float width;
    float height;
    bool isColliding;
};

struct c_Health
{
    float Value;
};

struct c_Poison
{
    float TickDamage;
};

struct c_Behaviour
{
    char ScriptPath[64];
    int LuaTableRef;

    c_Behaviour(const char* path, int luaRef) : LuaTableRef(luaRef)
    {
        memset(ScriptPath, '\0', 64);
        strcpy_s(ScriptPath, path);
    }
};