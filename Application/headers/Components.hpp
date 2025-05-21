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

struct c_Clickable {};

struct c_Selected {};

struct c_Color
{
    unsigned char r, g, b, a;
};

struct c_Button 
{
    std::string label;
    int posX;
    int posY;
    int width;
    int height;
    c_Color textColour;
    int textPosX;
    int textPosY;
    int fontSize;
    bool active;
};

struct c_Goal
{
    bool open;
};

struct c_Reset {};