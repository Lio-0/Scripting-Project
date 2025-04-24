#pragma once

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