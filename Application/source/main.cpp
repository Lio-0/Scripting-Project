#pragma once

#include <iostream>
#include <thread>
#include <string>
#include "lua.hpp"
#include "raylib.h"
#include "raymath.h"
#include "entt.hpp"
#include "LuaInput.hpp"
#include "SceneManager.hpp"
#include "json.hpp"
#include <fstream>
#include "SceneSerializer.hpp"

using json = nlohmann::json;
#define MAX_COLUMNS 10

enum class GameState
{
    Menu,
    Game,
    Editing,
    GameOver
};

void SetupGameScene(lua_State* L, Camera& camera, Scene &scene)
{
    Scene::lua_openscene(L, &scene);
    scene.CreateSystem<GoalSystem>(L);
    scene.CreateSystem<BehaviourSystem>(L);
    scene.CreateSystem<CameraSystem>(&camera, 0);
    scene.CreateSystem<CollisionSystem>(L, 0, 1);
    scene.CreateSystem<CollisionSystem>(L, 2, 0);
    scene.CreateSystem<ButtonSystem>(L);
}

void SetupEditingScene(lua_State* L, Camera& camera, Scene& scene)
{
    Scene::lua_openscene(L, &scene);
    scene.CreateSystem<CameraSystem>(&camera, 1);
    scene.CreateSystem<SelectionSystem>(L, &camera);
    scene.CreateSystem<DraggingSystem>(L, &camera);
    scene.CreateSystem<ButtonSystem>(L);
    scene.CreateSystem<BehaviourSystem>(L);

    if (luaL_dofile(L, "scripts/initEditor.lua") != LUA_OK)
    {
        GameConsole::DumpError(L);
    }
}

void SetupMenuScene(lua_State* L, Camera& camera, Scene& scene)
{
    Scene::lua_openscene(L, &scene);
    scene.CreateSystem<ButtonSystem>(L);
    scene.CreateSystem<BehaviourSystem>(L);

    if (luaL_dofile(L, "scripts/initMenu.lua") != LUA_OK)
    {
        GameConsole::DumpError(L);
    }
}

int main()
{
    srand((unsigned int)time(NULL));

    // LUA SKIT
    //Rekommenderat att ha ett men g�r att ha flera om det beh�vs
    lua_State* L = luaL_newstate();

    //Camera setup
    Camera camera;
    
    //�ppnar standardbibliotek f�r lua, g�r s� att kodstr�ngen g�r att k�ra
    luaL_openlibs(L);
    LuaInput input(L);
    SceneManager sm(L);

    //Scenes
    Scene GameScene(L);
    Scene EditingScene(L);
    Scene MenuScene(L);

    sm.AddScene(&GameScene, "game");
    sm.AddScene(&EditingScene, "editor");
    sm.AddScene(&MenuScene, "menu");

    //Scene Setup functions
    SetupGameScene(L, camera, GameScene);
    SetupEditingScene(L, camera, EditingScene);
    SetupMenuScene(L, camera, MenuScene);

    //Start game in GameScene for now
    GameState currentState = GameState::Game;
    sm.LoadScene(L, "menu");

	const int screenWidth = 800 * 2;
	const int screenHeight = 450 * 2;

	InitWindow(screenWidth, screenHeight, "Jonas Jump");

    // Define the camera to look into our 3d world (position, target, up vector)
    camera = { 0 };
    camera.position = { 0.0f, 2.0f, 4.0f };    // Camera position
    camera.target = { 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = { 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 60.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    int cameraMode = CAMERA_FIRST_PERSON;

    Vector3 lookDirection = { 0, 0, 1.0f };

	SetTargetFPS(60);
    Image logo = LoadImage("assets/jonas_logo.png");
    Image c_logo = LoadImage("assets/al_logo.png");
    Texture2D title_texture = LoadTextureFromImage(logo);
    Texture2D logo_texture = LoadTextureFromImage(c_logo);

    UnloadImage(logo);
    UnloadImage(c_logo);

    //Models
    Renderer renderer;
    Model cubeModel = LoadModel("assets/cube.obj");
    renderer.LoadModel("cube", cubeModel);
    Model ratModel = LoadModel("assets/rat.obj");
    renderer.LoadModel("rat", ratModel);
    Model orbModel = LoadModel("assets/orb.obj");
    renderer.LoadModel("orb", orbModel);
    Model goalLockedModel = LoadModel("assets/goal_locked.obj");
    renderer.LoadModel("goal_locked", goalLockedModel);
    Model goalOpenModel = LoadModel("assets/goal_open.obj");
    renderer.LoadModel("goal_open", goalOpenModel);
    Mesh sphere = GenMeshSphere(500.0f, 50, 50);
    Model skybox = LoadModelFromMesh(sphere);
    renderer.LoadModel("skybox", skybox);

    //Textures
    Texture2D greyTexture = LoadTexture("assets/grey_texture.png");
    renderer.LoadTexture("grey_texture", greyTexture);
    Texture2D baseTexture = LoadTexture("assets/base_texture.png");
    renderer.LoadTexture("base_texture", baseTexture);
    Texture2D orbTexture = LoadTexture("assets/orb_texture.png");
    renderer.LoadTexture("orb_texture", orbTexture);
    Texture2D goalLockedTexture = LoadTexture("assets/goal_locked.png");
    renderer.LoadTexture("goal_locked", goalLockedTexture);
    Texture2D goalOpenTexture = LoadTexture("assets/goal_open.png");
    renderer.LoadTexture("goal_open", goalOpenTexture);
    Texture2D panorama = LoadTexture("assets/skybox.png");
    renderer.LoadTexture("skybox_texture", panorama);
    Texture2D rockTexture = LoadTexture("assets/rock.png");
    renderer.LoadTexture("rock_texture", rockTexture);

	while (!WindowShouldClose())
	{
        // Update
        //----------------------------------------------------------------------------------
        {
            auto view = sm.GetCurrentScene()->GetRegistry()->view<c_Selected>();
            for (auto [entity] : view.each()) {
                if (IsKeyPressed(KEY_O)) {
                    luaL_dofile(L, "scripts/obstacle.lua");
                    lua_pushvalue(L, -1);
                    int luaTableRef = luaL_ref(L, LUA_REGISTRYINDEX);

                    lua_pushinteger(L, (int)entity);
                    lua_setfield(L, -2, "ID");

                    lua_pushstring(L, "scripts/obstacle.lua");
                    lua_setfield(L, -2, "path");

                    lua_getfield(L, -1, "OnCreate");
                    lua_pushvalue(L, -2);
                    lua_pcall(L, 1, 0, 0);

                    sm.GetCurrentScene()->SetComponent<c_Behaviour>((int)entity, "scripts/obstacle.lua", luaTableRef);
                }
                if (IsKeyPressed(KEY_DELETE) && IsMouseButtonUp(MOUSE_BUTTON_LEFT))
                {
                    sm.GetCurrentScene()->RemoveEntity((int)entity);
                }
            }
        }


        if (IsKeyPressed(KEY_L))
        {
            sm.GetCurrentScene()->Reset();
        }

        sm.UpdateScene(L, GetFrameTime());

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (sm.GetCurrentScene() == &MenuScene)
        {
            DrawTexture(title_texture, screenWidth / 2 - title_texture.width / 2, screenHeight / 8, WHITE);
            DrawTexture(logo_texture, screenWidth - logo_texture.width - 50, screenHeight - logo_texture.height - 50, WHITE);
        }

        if (sm.GetCurrentScene() == &EditingScene)
        {
            BeginMode3D(camera);
            DrawGrid(100, 5.0f);
            EndMode3D();
        }

        sm.DrawScene(renderer, camera);
        EndDrawing();
	}

	CloseWindow();

	return 0;
}