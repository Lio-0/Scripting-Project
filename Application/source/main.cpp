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

#define MAX_COLUMNS 10



//Function som körs parallelt av tråd
void ConsoleThreadFunction(lua_State* L)
{

	//Läs console input
	std::string input;
	//while (GetConsoleWindow())
	//{
	//	std::cout << "> ";
	//	std::getline(std::cin, input);

	//	//Försök köra strängen som lua kod
	//	if (luaL_dostring(L, input.c_str()) != LUA_OK)
	//	{
	//		DumpError(L);
	//	}
	//}
}


int main()
{
    srand((unsigned int)time(NULL));

	std::cout << "Hello Bergman!" << std::endl;
    // LUA SKIT
	//Rekommenderat att ha ett men går att ha flera om det behövs
	lua_State* L = luaL_newstate();

	////Öppnar standardbibliotek för lua, gör så att kodsträngen går att köra
	luaL_openlibs(L);

    LuaInput input(L);
    Scene scene(L);
    Scene::lua_openscene(L, &scene);
    Camera camera;
    scene.CreateSystem<BehaviourSystem>(L);
    scene.CreateSystem<CameraSystem>(&camera);
    scene.CreateSystem<CollisionSystem>(L, 0, 1);

    luaL_dofile(L, "scripts/initLevel.lua");

	////Skapa tråd
	//std::thread consoleThread(ConsoleThreadFunction, L);

    for (size_t i = 0; i < 10; i++)
    {
        luaL_dofile(L, "scripts/createBlock.lua");
        GameConsole::DumpError(L);
        //Create a new entity.
        //auto entity = scene.CreateEntity();

        ////Each entity starts out with 100 health points.
        //scene.SetComponent<c_Health>(entity, 100.0f);

        //c_Vector pos = { i * 2, 0.0f, i * 2};
        //c_Vector rot = { 0.0f, 0.0f, 0.0f };
        //c_Vector scale = { 1.0f, 1.0f, 1.0f };
        //c_Transform transform = { pos, rot, scale };
        //scene.SetComponent<c_Transform>(entity, transform);
        //scene.SetComponent<c_Visual>(entity, "cube", "grey_texture", true);
    }

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

    //Textures
    Texture2D greyTexture = LoadTexture("assets/grey_texture.png");
    renderer.LoadTexture("grey_texture", greyTexture);
    Texture2D baseTexture = LoadTexture("assets/base_texture.png");
    renderer.LoadTexture("base_texture", baseTexture);

	while (!WindowShouldClose())
	{
        // Update
        //----------------------------------------------------------------------------------

        if (IsKeyDown(KEY_ONE))
        {
            scene.CreateSystem<PoisonSystem>(600);
        }
        if (IsKeyDown(KEY_TWO))
        {
            scene.CreateSystem<CurePoisonSystem>(600);
        }
        if (IsKeyDown(KEY_THREE))
        {
            scene.CreateSystem<DeathSystem>(600);
        }
        if (IsKeyDown(KEY_FOUR))
        {
            scene.CreateSystem<SpawnPoisonSystem>(600);
        }

        if (IsKeyPressed(KEY_FIVE))
        {
            if (luaL_dofile(L, "scripts/createFilur.lua") != LUA_OK)
            {
                GameConsole::DumpError(L);
            }
        }

        scene.UpdateSystems(GetFrameTime());

        // Switch camera projection
        if (IsKeyPressed(KEY_P))
        {
            if (camera.projection == CAMERA_PERSPECTIVE)
            {
                // Create isometric view
                cameraMode = CAMERA_THIRD_PERSON;
                // Note: The target distance is related to the render distance in the orthographic projection
                camera.position = { 0.0f, 2.0f, -100.0f };
                camera.target = { 0.0f, 2.0f, 0.0f };
                camera.up = { 0.0f, 1.0f, 0.0f };
                camera.projection = CAMERA_ORTHOGRAPHIC;
                camera.fovy = 20.0f; // near plane width in CAMERA_ORTHOGRAPHIC
                
                /*CameraYaw(&camera, -135 * DEG2RAD, true);
                CameraPitch(&camera, -45 * DEG2RAD, true, true, false);*/
            }
            else if (camera.projection == CAMERA_ORTHOGRAPHIC)
            {
                // Reset to default view
                cameraMode = CAMERA_THIRD_PERSON;
                camera.position = { 0.0f, 2.0f, 10.0f };
                camera.target = { 0.0f, 2.0f, 0.0f };
                camera.up = { 0.0f, 1.0f, 0.0f };
                camera.projection = CAMERA_PERSPECTIVE;
                camera.fovy = 60.0f;
            }
        }

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();


        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        DrawPlane({ 0.0f, 0.0f, 0.0f }, { 32.0f, 32.0f }, LIGHTGRAY); // Draw ground


        /*{
            auto view = scene.GetRegistry()->view<c_Transform, c_Poison>();

            view.each([](const c_Transform& transform, const c_Poison& poison) {
                DrawSphere(Vector3{ transform.position.x, transform.position.y, transform.position.z }, 0.1f * transform.scale.x, RED);
                DrawSphereWires(Vector3{ transform.position.x, transform.position.y, transform.position.z }, 0.1f * transform.scale.x, 5, 5, BLACK);
                });
        }

        {

            auto view = scene.GetRegistry()->view<c_Transform>(entt::exclude<c_Poison, c_Behaviour>);
            
            view.each([](const c_Transform& transform) {
                DrawSphere(Vector3{ transform.position.x, transform.position.y, transform.position.z }, 0.1f * transform.scale.x, BLUE);
                DrawSphereWires(Vector3{ transform.position.x, transform.position.y, transform.position.z }, 0.1f * transform.scale.x, 5, 5, BLACK);
                });
        }

        {
            auto view = scene.GetRegistry()->view<c_Behaviour, c_Transform>(entt::exclude<c_Vector>);

            view.each([](const c_Behaviour& behaviour, const c_Transform& transform) {
                DrawSphere(Vector3{ transform.position.x, transform.position.y, transform.position.z }, 0.1f, BEIGE);
                DrawSphereWires(Vector3{ transform.position.x, transform.position.y, transform.position.z }, 0.1f, 12, 12, BLACK);
                });
        }*/

        {
            auto view = scene.GetRegistry()->view<c_Behaviour, c_Transform, c_Vector>();

            view.each([](const c_Behaviour& behaviour, const c_Transform& transform, const c_Vector& vector) {
                DrawCylinder(Vector3{ transform.position.x, transform.position.y, transform.position.z }, 0.25f, 0.25f, 1, 32, PINK);
                DrawCylinderWires(Vector3{ transform.position.x, transform.position.y, transform.position.z }, 0.25f, 0.25f, 1, 8, BLACK);
                });
        }

        {
            auto view = scene.GetRegistry()->view<entt::entity>();

            for (size_t i = 0; i < view.size(); i++)
            {
                DrawCylinder(Vector3(0, i, 0) / 100, 0.1f, 0.1f, 0.1f, 20, BLACK);
            }
        }
        scene.DrawScene(renderer);

        EndMode3D();

        // Draw info boxes
        DrawRectangle(5, 5, 310, 115, Fade(SKYBLUE, 0.5f));
        DrawRectangleLines(5, 5, 310, 115, BLUE);

        DrawText("Camera controls: Up, Down, Left, Right, Enter, Right-Shift", 15, 15, 10, BLACK);
        DrawText("Move keys: W, A, S, D, Space, Left-Ctrl", 15, 30, 10, BLACK);
        DrawText("1. Poison System", 15, 45, 10, BLACK);
        DrawText("2. Cure Poison System", 15, 60, 10, BLACK);
        DrawText("3. Death System", 15, 75, 10, BLACK);
        DrawText("4. Spawn Poison System", 15, 90, 10, BLACK);
        DrawText("5. Spawn Entities", 15, 105, 10, BLACK);

        DrawRectangle(600, 5, 195, 100, Fade(SKYBLUE, 0.5f));
        DrawRectangleLines(600, 5, 195, 100, BLUE);

        DrawText("Camera status:", 610, 15, 10, BLACK);
        DrawText(TextFormat("- Projection: %s", (camera.projection == CAMERA_PERSPECTIVE) ? "PERSPECTIVE" :
            (camera.projection == CAMERA_ORTHOGRAPHIC) ? "ORTHOGRAPHIC" : "CUSTOM"), 610, 45, 10, BLACK);
        DrawText(TextFormat("- Position: (%06.3f, %06.3f, %06.3f)", camera.position.x, camera.position.y, camera.position.z), 610, 60, 10, BLACK);
        DrawText(TextFormat("- Target: (%06.3f, %06.3f, %06.3f)", camera.target.x, camera.target.y, camera.target.z), 610, 75, 10, BLACK);
        DrawText(TextFormat("- Up: (%06.3f, %06.3f, %06.3f)", camera.up.x, camera.up.y, camera.up.z), 610, 90, 10, BLACK);

        EndDrawing();
	}

	CloseWindow();

	return 0;
}