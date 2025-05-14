#include "renderer.hpp"
#include "raymath.h"

Renderer::Renderer()
{
    // Initialize any required resources here
}

Renderer::~Renderer()
{
    // Unload all models and textures
    for (auto& [name, model] : m_models)
    {
        UnloadModel(model);
    }
    m_models.clear();
    
    for (auto& [name, texture] : m_textures)
    {
        UnloadTexture(texture);
    }
    m_textures.clear();
}

void Renderer::LoadModel(const std::string& name, Model model)
{
    // If a model with this name already exists, unload it first
    if (m_models.find(name) != m_models.end())
    {
        UnloadModel(m_models[name]);
    }
    m_models[name] = model;
}

void Renderer::LoadTexture(const std::string& name, Texture2D texture)
{
    // If a texture with this name already exists, unload it first
    if (m_textures.find(name) != m_textures.end())
    {
        UnloadTexture(m_textures[name]);
    }
    m_textures[name] = texture;
}

void Renderer::Draw(const std::vector<RenderData>& objects)
{
    
    for (const auto& obj : objects)
    {
        // Find the model and texture
        auto modelIt = m_models.find(obj.modelName);
        auto textureIt = m_textures.find(obj.textureName);
        
        if (modelIt != m_models.end())
        {
            Model model = modelIt->second;
            
            // Apply texture if available
            if (textureIt != m_textures.end())
            {
                // Assuming the model has at least one material and one map
                model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = textureIt->second;
            }
            else
            {
                auto baseText = m_textures.find("base_texture");
                model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = baseText->second;
            }
                
            
            DrawModelEx(model, obj.position, { 0, 1.0f, 0 }, obj.rotation.x, obj.scale, {obj.r, obj.g, obj.b, obj.a});
        }
    }
}