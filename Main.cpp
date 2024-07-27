#include "libraries.h"
#include "headbar.h"
#include "LuaFunct.h"
// Need to tell the code I'm using C for Lua
extern "C" {
#include "Lua/include/lua.h"
#include "Lua/include/lauxlib.h"
#include "Lua/include/lualib.h"
}

// Link
#ifdef _WIN32
#pragma comment(lib, "Lua/lua54.lib")
#endif // _WIN32





// Global var
GLFWwindow* window;

// Init and config GLFW
void initGLFW() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
}

void initImgui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Initialize the style
    ImGuiStyle& style = ImGui::GetStyle();

    io.Fonts->AddFontFromFileTTF("Fonts/Inter-Regular.ttf", 16.0f);

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    std::ifstream in("Style/CStyle.Style");

    std::stringstream buffer;

    //this grabs everything in 1 line
    buffer << in.rdbuf();

    //set string to the line
    std::string contents = buffer.str();
    in.close();

    in.close();

    // Register ImGui functions
    registerImGuiFunctions(L);

    const char* script = contents.c_str();

    if (luaL_dostring(L, script) != LUA_OK) {
        std::cerr << "Error: " << lua_tostring(L, -1) << std::endl;
    }

    lua_close(L);
}
int main() {
    // Init GLFW
    initGLFW();

    // Generate a window
    window = glfwCreateWindow(800, 800, "Test win", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Load glad, and check if it has errors
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Init ImGui
    initImgui();

    static float editorHeightPercent = 0.7f;
    static float terminalHeightPercent = 0.2f;

    // Set the size
    glViewport(0, 0, 800, 800);

    int windowWidth, windowHeight;

    while (!glfwWindowShouldClose(window)) {
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
        glViewport(0, 0, windowWidth, windowHeight);

        // Clear the screen
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // UI win
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // From headbar.h
        Renderbar();

        // Inspector window

        // Terminal window

        ImGui::End();

        // Render
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
