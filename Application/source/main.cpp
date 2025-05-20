#pragma once

#include <iostream>
#include <thread>
#include <string>
#include "lua.hpp"
#include "raylib.h"
#include "raymath.h"
#include "entt.hpp"
#include "Scene.hpp"
#include "LuaInput.hpp"
#include "SceneManager.hpp"

#define MAX_COLUMNS 10

enum class GameState
{
    Menu,
    Game,
    Editing,
    GameOver
};

void SetUpGameScene(lua_State* L, Camera& camera, Scene &scene)
{
    Scene::lua_openscene(L, &scene);
    scene.CreateSystem<BehaviourSystem>(L);
    scene.CreateSystem<CameraSystem>(&camera, 0);
    scene.CreateSystem<CollisionSystem>(L, 0, 1);
    scene.CreateSystem<CollisionSystem>(L, 2, 0);
    scene.CreateSystem<ButtonSystem>(L);

    luaL_dofile(L, "scripts/initLevel.lua");
    GameConsole::DumpError(L);
}

void SetUpEditingScene(lua_State* L, Camera& camera, Scene& scene)
{
    Scene::lua_openscene(L, &scene);
    scene.CreateSystem<CameraSystem>(&camera, 1);
    scene.CreateSystem<SelectionSystem>(L, &camera);
    scene.CreateSystem<DraggingSystem>(L, &camera);
    scene.CreateSystem<ButtonSystem>(L);

    luaL_dofile(L, "scripts/initEditorMenu.lua");
    GameConsole::DumpError(L);
}

int main()
{
    srand((unsigned int)time(NULL));

    // LUA SKIT
    //Rekommenderat att ha ett men går att ha flera om det behövs
    lua_State* L = luaL_newstate();

    //Camera setup

    Camera camera;

    
    //Öppnar standardbibliotek för lua, gör så att kodsträngen går att köra
    luaL_openlibs(L);
    LuaInput input(L);
    SceneManager sm(L);

    //Scenes
    Scene GameScene(L);
    Scene EditingScene(L);
    Scene MenuScene(L);

    sm.AddScene(&GameScene, "game");
    sm.AddScene(&EditingScene, "editor");
    sm.AddScene(&MenuScene, "manu");

    //Scene Setup functions
    SetUpGameScene(L, camera, GameScene);
    SetUpEditingScene(L, camera, EditingScene);

    //Start game in GameScene for now
    GameState currentState = GameState::Game;
    Scene* currentScene = &GameScene;
    Scene::lua_openscene(L, &GameScene);

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

	DisableCursor();                    // Limit cursor to relative movement inside the window

	SetTargetFPS(60);

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


	while (!WindowShouldClose())
	{
        // Update
        //----------------------------------------------------------------------------------

        if (IsKeyPressed(KEY_TAB)) {
            if (currentScene == &GameScene)
            {
                currentScene = &EditingScene;
                Scene::lua_openscene(L, &EditingScene);
            }  
            else
            {
                currentScene = &GameScene;
                Scene::lua_openscene(L, &GameScene);
            }
               
        }

        currentScene->UpdateSystems(GetFrameTime());


        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);
        

        currentScene->DrawScene(renderer, camera);

        
        // Draw info boxes
        DrawRectangle(5, 5, 310, 115, Fade(SKYBLUE, 0.5f));
        DrawRectangleLines(5, 5, 310, 115, BLUE);

        DrawText("Camera controls: Up, Down, Left, Right, Enter, Right-Shift", 15, 15, 10, BLACK);
        DrawText("Move keys: W, A, S, D, Space, Left-Ctrl", 15, 30, 10, BLACK);
        {
            int collectibleCount = currentScene->GetRegistry()->view<c_Collectible>().size();
            std::string text = std::string("Collectibles: ") + std::to_string(collectibleCount);
            DrawText(text.c_str(), 15, 85, 10, BLACK);

            if (collectibleCount <= 0)
                DrawText("YOU WIN!", screenWidth / 2 - 200, screenHeight / 2 - 50, 100, GREEN);
        }
        DrawText("5. Spawn Entities", 15, 105, 10, BLACK);

        DrawRectangle(600, 5, 195, 100, Fade(SKYBLUE, 0.5f));
        DrawRectangleLines(600, 5, 195, 100, BLUE);

        DrawText("Camera status:", 610, 15, 10, BLACK);
        DrawText(TextFormat("- Position: (%06.3f, %06.3f, %06.3f)", camera.position.x, camera.position.y, camera.position.z), 610, 60, 10, BLACK);
        DrawText(TextFormat("- Target: (%06.3f, %06.3f, %06.3f)", camera.target.x, camera.target.y, camera.target.z), 610, 75, 10, BLACK);
        DrawText(TextFormat("- Up: (%06.3f, %06.3f, %06.3f)", camera.up.x, camera.up.y, camera.up.z), 610, 90, 10, BLACK);
        EndDrawing();
	}

	CloseWindow();

	return 0;
}