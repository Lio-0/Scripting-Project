#pragma once
#include <stdlib.h>

struct c_Vector {

    float x, y, z;
};

struct c_Camera {
    int ID;
    c_Vector positionOffset;
    c_Vector target;
};

//struct c_Velocity {
//
//    float x, y, z;
//};

struct c_Transform {
    c_Vector position, rotation, scale;
};

struct c_Collision {
    int layer;
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

struct c_Visual
{
    std::string modelName;
    std::string textureName;
    bool isRendered;
};