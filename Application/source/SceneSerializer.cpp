// SceneSerializer.cpp
#include "SceneSerializer.hpp"

#include <fstream>

// Short alias only used here
using json = nlohmann::json;

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

json serialize_goal(const c_Goal& g) {
    return { {"open", g.open} };
}


void SceneSerializer::Save(entt::registry& registry, const std::string& path) {
    json scene_json = json::array();

    auto view = registry.view<c_Transform>();
    for (auto entity : view) {
        json entity_json;

        entity_json["transform"] = serialize_transform(registry.get<c_Transform>(entity));

        if (registry.any_of<c_Vector>(entity)) {
            entity_json["vector"] = serialize_vector(registry.get<c_Vector>(entity));
        }
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
        if (registry.any_of<c_Button>(entity)) {
            entity_json["button"] = serialize_button(registry.get<c_Button>(entity));
        }
        if (registry.any_of<c_Goal>(entity)) {
            entity_json["goal"] = serialize_goal(registry.get<c_Goal>(entity));
        }

        scene_json.push_back(entity_json);
    }

    std::ofstream file(path);
    file << scene_json.dump(4);
    file.close();
}

void SceneSerializer::Load(lua_State* L, const std::string& path) {
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

        if (entity_data.contains("vector")) {
            const auto& v = entity_data["vector"];
            lua_newtable(L);
            lua_pushstring(L, "x"); lua_pushnumber(L, v["x"]); lua_settable(L, -3);
            lua_pushstring(L, "y"); lua_pushnumber(L, v["y"]); lua_settable(L, -3);
            lua_pushstring(L, "z"); lua_pushnumber(L, v["z"]); lua_settable(L, -3);
            call_setcomponent("vector", 1);
        }

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

        if (entity_data.contains("goal")) {
            bool open = entity_data["goal"]["open"];
            lua_pushboolean(L, open);
            call_setcomponent("goal", 1);
        }

    }
}

