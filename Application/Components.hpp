#pragma once

struct Vector {
    float x, y, z;
};

//struct CustomTransform {
//    Vector position, rotation, scale;
//};

struct Collision {
    float width;
    float height;
    bool isColliding;
};

struct Health
{
    float Value;
};

struct Poison
{
    float TickDamage;
};