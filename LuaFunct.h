#ifndef IMGUI_FUNCT_H
#define IMGUI_FUNCT_H

#include "imgui.h"


extern "C" {
#include "Lua/include/lua.h"
#include "Lua/include/lauxlib.h"
#include "Lua/include/lualib.h"
}

// Link
#ifdef _WIN32
#pragma comment(lib, "Lua/lua54.lib")
#endif // _WIN32


//*** This is cobbled together through forum posts and chatgpt ***\\
// I would of loved to code it myself, but i really dont understand anything about lua :(
// If you find any bugs or ways to make it better i would really appreciate some code or some advice!



//  push ImVec4 to Lua stack
void pushImVec4(lua_State* L, const ImVec4& vec) {
    lua_newtable(L);
    lua_pushnumber(L, vec.x);
    lua_setfield(L, -2, "x");
    lua_pushnumber(L, vec.y);
    lua_setfield(L, -2, "y");
    lua_pushnumber(L, vec.z);
    lua_setfield(L, -2, "z");
    lua_pushnumber(L, vec.w);
    lua_setfield(L, -2, "w");
}

//  retrieve ImVec4 from Lua stack
ImVec4 getImVec4(lua_State* L, int index) {
    lua_getfield(L, index, "x");
    float x = lua_tonumber(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, index, "y");
    float y = lua_tonumber(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, index, "z");
    float z = lua_tonumber(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, index, "w");
    float w = lua_tonumber(L, -1);
    lua_pop(L, 1);
    return ImVec4(x, y, z, w);
}

//  push ImVec2 to Lua stack
void pushImVec2(lua_State* L, const ImVec2& vec) {
    lua_newtable(L);
    lua_pushnumber(L, vec.x);
    lua_setfield(L, -2, "x");
    lua_pushnumber(L, vec.y);
    lua_setfield(L, -2, "y");
}

//  retrieve ImVec2 from Lua stack
ImVec2 getImVec2(lua_State* L, int index) {
    lua_getfield(L, index, "x");
    float x = lua_tonumber(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, index, "y");
    float y = lua_tonumber(L, -1);
    lua_pop(L, 1);
    return ImVec2(x, y);
}

// Lua for ImGui::GetStyle()
int lua_getStyle(lua_State* L) {
    ImGuiStyle* style = &ImGui::GetStyle();
    lua_newtable(L);
    lua_pushnumber(L, style->WindowRounding);
    lua_setfield(L, -2, "WindowRounding");
    pushImVec2(L, style->FramePadding);
    lua_setfield(L, -2, "FramePadding");

    lua_newtable(L);
    for (int i = 0; i < ImGuiCol_COUNT; i++) {
        pushImVec4(L, style->Colors[i]);
        lua_rawseti(L, -2, i + 1);
    }
    lua_setfield(L, -2, "Colors");

    lua_setglobal(L, "style");  

    return 1;
}

// Lua settings for like rounding and colors
int lua_setStyle(lua_State* L) {
    lua_getglobal(L, "style");
    ImGuiStyle* style = &ImGui::GetStyle();

    lua_getfield(L, -1, "WindowRounding");
    style->WindowRounding = lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "FramePadding");
    style->FramePadding = getImVec2(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "Colors");
    for (int i = 0; i < ImGuiCol_COUNT; i++) {
        lua_rawgeti(L, -1, i + 1);
        style->Colors[i] = getImVec4(L, -1);
        lua_pop(L, 1);
    }

    return 0;
}

// color constants ( so like if i want it to change color or not when its selected
void pushImGuiColorConstants(lua_State* L) {
    lua_newtable(L);
    lua_pushnumber(L, ImGuiCol_WindowBg);
    lua_setfield(L, -2, "WindowBg");
    lua_pushnumber(L, ImGuiCol_TitleBgActive);
    lua_setfield(L, -2, "TitleBgActive");
    // Add other ImGuiCol constants as needed
    lua_setglobal(L, "ImGuiCol");
}

// set some imgui functions in lua
void registerImGuiFunctions(lua_State* L) {
    lua_register(L, "getStyle", lua_getStyle);
    lua_register(L, "setStyle", lua_setStyle);
    pushImGuiColorConstants(L); 
}

#endif 