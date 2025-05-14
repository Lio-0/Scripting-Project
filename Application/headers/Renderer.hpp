#pragma once
#include "raylib.h"
#include <string>
#include <unordered_map>

struct RenderData
{
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;
    std::string modelName;
    std::string textureName;
    unsigned char r, g, b, a;
};

class Renderer
{
public:
    Renderer();
    ~Renderer();

    void LoadModel(const std::string& name, Model model);
    void LoadTexture(const std::string& name, Texture2D Texture);

    void Draw(const std::vector<RenderData>& objects);

private:
    std::unordered_map<std::string, Model> m_models;
    std::unordered_map<std::string, Texture2D> m_textures;
};
