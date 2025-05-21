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
    scene.CreateSystem<BehaviourSystem>(L);
    scene.CreateSystem<CameraSystem>(&camera, 0);
    scene.CreateSystem<CollisionSystem>(L, 0, 1);
    scene.CreateSystem<CollisionSystem>(L, 2, 0);
    scene.CreateSystem<ButtonSystem>(L);
    scene.CreateSystem<GoalSystem>(L);

    if (luaL_dofile(L, "scripts/initLevel.lua") != LUA_OK)
    {
        GameConsole::DumpError(L);
    }
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

json serialize_vector(const c_Vector& v) {
    return { {"x", v.x}, {"y", v.y}, {"z", v.z} };
}

json serialize_transform(const c_Transform& t) {
    return {
        {"position", serialize_vector(t.position)},
        {"rotation", serialize_vector(t.rotation)},
        {"scale",    serialize_vector(t.scale)}
    };
}

json serialize_visual(const c_Visual& v) {
    return {
        {"model", v.modelName},
        {"texture", v.textureName},
        {"rendered", v.isRendered}
    };
}

json serialize_collision(const c_Collision& c) {
    return { {"layer", c.layer} };
}

json serialize_behaviour(const c_Behaviour& b) {
    return { {"script", b.ScriptPath} };
}

json serialize_color(const c_Color& c) {
    return { {"r", c.r}, {"g", c.g}, {"b", c.b}, {"a", c.a} };
}

json serialize_collectible(const c_Collectible& c) {
    return { {"id", c.ID} };
}

json serialize_observer(const c_CollectibleObserver& o) {
    return { {"observing_id", o.observingID} };
}

json serialize_button(const c_Button& b) {
    return {
        {"label", b.label},
        {"posX", b.posX}, {"posY", b.posY},
        {"width", b.width}, {"height", b.height},
        {"textColor", serialize_color(b.textColour)},
        {"textPosX", b.textPosX}, {"textPosY", b.textPosY},
        {"fontSize", b.fontSize},
        {"active", b.active}
    };
}


void SaveScene(entt::registry& registry, const std::string& path) {
    json scene_json = json::array();

    auto view = registry.view<c_Transform>();
    for (auto entity : view) {
        json entity_json;

        entity_json["transform"] = serialize_transform(registry.get<c_Transform>(entity));

        if (registry.any_of<c_Visual>(entity)) {
            entity_json["visual"] = serialize_visual(registry.get<c_Visual>(entity));
        }
        if (registry.any_of<c_Collision>(entity)) {
            entity_json["collision"] = serialize_collision(registry.get<c_Collision>(entity));
        }
        if (registry.any_of<c_Color>(entity)) {
            entity_json["color"] = serialize_color(registry.get<c_Color>(entity));
        }
        if (registry.any_of<c_Clickable>(entity)) {
            entity_json["clickable"] = true;
        }
        if (registry.any_of<c_Selected>(entity)) {
            entity_json["selected"] = true;
        }
        if (registry.any_of<c_Behaviour>(entity)) {
            entity_json["behaviour"] = serialize_behaviour(registry.get<c_Behaviour>(entity));
        }
        if (registry.any_of<c_Collectible>(entity)) {
            entity_json["collectible"] = serialize_collectible(registry.get<c_Collectible>(entity));
        }
        if (registry.any_of<c_CollectibleObserver>(entity)) {
            entity_json["observer"] = serialize_observer(registry.get<c_CollectibleObserver>(entity));
        }
        if (registry.any_of<c_Button>(entity)) {
            entity_json["button"] = serialize_button(registry.get<c_Button>(entity));
        }

        scene_json.push_back(entity_json);
    }

    std::ofstream file(path);
    file << scene_json.dump(4);
    file.close();
}

void PrintLuaStack(lua_State* L) {
    int top = lua_gettop(L);
    std::cout << "[Lua Stack] Top = " << top << "\n";
    for (int i = 1; i <= top; i++) {
        int t = lua_type(L, i);
        std::cout << i << ": " << lua_typename(L, t) << "\n";
    }
}

void LoadScene(lua_State* L, const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open scene file: " << path << "\n";
        return;
    }

    json scene_json;
    file >> scene_json;
    file.close();

    for (const auto& entity_data : scene_json) {
        // Get the scene table
        lua_getglobal(L, "scene");
        if (!lua_istable(L, -1)) {
            std::cerr << "Error: 'scene' is not a table\n";
            lua_pop(L, 1);
            continue;
        }

        // Get CreateEntity function
        lua_getfield(L, -1, "CreateEntity");
        if (!lua_isfunction(L, -1)) {
            std::cerr << "Error: 'scene.CreateEntity' is not a function\n";
            lua_pop(L, 2); // pop function and scene table
            continue;
        }

        // Push scene table as first argument (self)
        lua_pushvalue(L, -2);

        // Call CreateEntity(scene)
        if (lua_pcall(L, 1, 1, 0) != LUA_OK) {
            std::cerr << "Error calling CreateEntity: " << lua_tostring(L, -1) << "\n";
            lua_pop(L, 1);
            continue;
        }

        int entity = (int)lua_tointeger(L, -1);
        lua_pop(L, 2); // pop entity and scene table

        // Helper lambda with proper stack handling
        auto call_setcomponent = [&](const std::string& type, int nargs) {
            // Save the top of the stack before any component data arguments
            int top_before_args = lua_gettop(L) - nargs;

            // Get scene table
            lua_getglobal(L, "scene");

            // Get SetComponent function
            lua_getfield(L, -1, "SetComponent");
            if (!lua_isfunction(L, -1)) {
                std::cerr << "Error: 'scene.SetComponent' is not a function\n";
                lua_pop(L, 2); // Pop scene and non-function
                return;
            }


            // Push entity ID
            lua_pushinteger(L, entity);

            // Push component type
            lua_pushstring(L, type.c_str());

            // Push component data arguments
            for (int i = 1; i <= nargs; i++) {
                // Calculate the correct stack index for each component data argument
                lua_pushvalue(L, top_before_args + i);
            }

            // Push 'self' parameter (scene table)
            lua_pushvalue(L, -2);
            
            // Call function with nargs + 3 parameters (self + entity + type + component data)
            if (lua_pcall(L, nargs + 3, 0, 0) != LUA_OK) {
                std::cerr << "Error calling SetComponent for " << type << ": " << lua_tostring(L, -1) << "\n";
                lua_pop(L, 2); // Pop error message and scene table
                return;
            }

            // Pop scene table
            lua_pop(L, 1);

            // Pop the component data arguments
            lua_settop(L, top_before_args);
            };

        if (entity_data.contains("transform")) {
            const auto& t = entity_data["transform"];

            lua_newtable(L);
            // Position
            lua_pushstring(L, "position");
            lua_newtable(L);
            lua_pushstring(L, "x"); lua_pushnumber(L, t["position"]["x"]); lua_settable(L, -3);
            lua_pushstring(L, "y"); lua_pushnumber(L, t["position"]["y"]); lua_settable(L, -3);
            lua_pushstring(L, "z"); lua_pushnumber(L, t["position"]["z"]); lua_settable(L, -3);
            lua_settable(L, -3);

            // Rotation
            lua_pushstring(L, "rotation");
            lua_newtable(L);
            lua_pushstring(L, "x"); lua_pushnumber(L, t["rotation"]["x"]); lua_settable(L, -3);
            lua_pushstring(L, "y"); lua_pushnumber(L, t["rotation"]["y"]); lua_settable(L, -3);
            lua_pushstring(L, "z"); lua_pushnumber(L, t["rotation"]["z"]); lua_settable(L, -3);
            lua_settable(L, -3);

            // Scale
            lua_pushstring(L, "scale");
            lua_newtable(L);
            lua_pushstring(L, "x"); lua_pushnumber(L, t["scale"]["x"]); lua_settable(L, -3);
            lua_pushstring(L, "y"); lua_pushnumber(L, t["scale"]["y"]); lua_settable(L, -3);
            lua_pushstring(L, "z"); lua_pushnumber(L, t["scale"]["z"]); lua_settable(L, -3);
            lua_settable(L, -3);

            call_setcomponent("transform", 1);
        }

        if (entity_data.contains("visual")) {
            const auto& v = entity_data["visual"];
            lua_pushstring(L, v["model"].get<std::string>().c_str());
            lua_pushstring(L, v["texture"].get<std::string>().c_str());
            lua_pushboolean(L, v["rendered"]);
            call_setcomponent("visual", 3);
        }

        if (entity_data.contains("collision")) {
            lua_pushinteger(L, entity_data["collision"]["layer"]);
            call_setcomponent("collision", 1);
        }

        if (entity_data.contains("color")) {
            const auto& c = entity_data["color"];
            lua_newtable(L);
            lua_pushstring(L, "r"); lua_pushinteger(L, c["r"]); lua_settable(L, -3);
            lua_pushstring(L, "g"); lua_pushinteger(L, c["g"]); lua_settable(L, -3);
            lua_pushstring(L, "b"); lua_pushinteger(L, c["b"]); lua_settable(L, -3);
            lua_pushstring(L, "a"); lua_pushinteger(L, c["a"]); lua_settable(L, -3);
            call_setcomponent("color", 1);
        }

        if (entity_data.contains("clickable")) {
            call_setcomponent("clickable", 0);
        }

        if (entity_data.contains("selected")) {
            call_setcomponent("selected", 0);
        }

        if (entity_data.contains("behaviour")) {
            std::string script = entity_data["behaviour"]["script"];
            lua_pushstring(L, script.c_str());
            call_setcomponent("behaviour", 1);
        }

        if (entity_data.contains("collectible")) {
            lua_pushinteger(L, entity_data["collectible"]["id"]);
            call_setcomponent("collectible", 1);
        }

        if (entity_data.contains("observer")) {
            lua_pushinteger(L, entity_data["observer"]["observing_id"]);
            call_setcomponent("collectible_observer", 1);
        }

        if (entity_data.contains("button")) {
            const auto& b = entity_data["button"];
            lua_newtable(L);
            lua_pushstring(L, "label"); lua_pushstring(L, b["label"].get<std::string>().c_str()); lua_settable(L, -3);
            lua_pushstring(L, "posX"); lua_pushinteger(L, b["posX"]); lua_settable(L, -3);
            lua_pushstring(L, "posY"); lua_pushinteger(L, b["posY"]); lua_settable(L, -3);
            lua_pushstring(L, "width"); lua_pushinteger(L, b["width"]); lua_settable(L, -3);
            lua_pushstring(L, "height"); lua_pushinteger(L, b["height"]); lua_settable(L, -3);
            lua_pushstring(L, "textPosX"); lua_pushinteger(L, b["textPosX"]); lua_settable(L, -3);
            lua_pushstring(L, "textPosY"); lua_pushinteger(L, b["textPosY"]); lua_settable(L, -3);
            lua_pushstring(L, "fontSize"); lua_pushinteger(L, b["fontSize"]); lua_settable(L, -3);
            lua_pushstring(L, "active"); lua_pushboolean(L, b["active"]); lua_settable(L, -3);

            // textColour
            const auto& tc = b["textColor"];
            lua_pushstring(L, "textColour");
            lua_newtable(L);
            lua_pushstring(L, "r"); lua_pushinteger(L, tc["r"]); lua_settable(L, -3);
            lua_pushstring(L, "g"); lua_pushinteger(L, tc["g"]); lua_settable(L, -3);
            lua_pushstring(L, "b"); lua_pushinteger(L, tc["b"]); lua_settable(L, -3);
            lua_pushstring(L, "a"); lua_pushinteger(L, tc["a"]); lua_settable(L, -3);
            lua_settable(L, -3); // set textColour

            call_setcomponent("button", 1);
        }
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

        sm.UpdateScene(L, GetFrameTime());

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (sm.GetCurrentScene() == &EditingScene)
        {
            BeginMode3D(camera);
            DrawGrid(100, 5.0f);
            EndMode3D();
        }

        sm.DrawScene(renderer, camera);

        

        // Draw info boxes
        DrawRectangle(5, 5, 310, 115, Fade(SKYBLUE, 0.5f));
        DrawRectangleLines(5, 5, 310, 115, BLUE);

        DrawText("Camera controls: Up, Down, Left, Right, Enter, Right-Shift", 15, 15, 10, BLACK);
        DrawText("Move keys: W, A, S, D, Space, Left-Ctrl", 15, 30, 10, BLACK);

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