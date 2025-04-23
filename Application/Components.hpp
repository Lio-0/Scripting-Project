struct Vector {
    float x, y, z;
};

struct Transform {
    float x, y, z, rotation;
};

struct Collision {
    float width;
    float height;
    bool isColliding;
};