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

struct c_Transform {
    c_Vector position, rotation, scale;
};

struct c_Collision {
    int layer;
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

struct c_Collectible
{
    int ID;
};

struct c_CollectibleObserver
{
    int observingID;
};