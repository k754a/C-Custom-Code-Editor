#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

//lets use IMgui

#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"

//menubar
#include"headbar.h"

//global var

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
 
    //FIND VERSION
    IMGUI_CHECKVERSION();
    //create contex
    ImGui::CreateContext();

    //var for inputs and outputs
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    //dark mode 
    //i might make a button to change that

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    //version of GLSL
    ImGui_ImplOpenGL3_Init("#version 330");

    ImGuiStyle& style = ImGui::GetStyle();

    // round
    style.WindowRounding = 10;

    // change the wins background color!
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f); // gray color

    // rouding and padding
    style.FrameRounding = 4.0f;
    style.FramePadding = ImVec2(4, 2);

    //this gets the background color of the window, and then sets it so that the top bar does not change color!
    ImVec4 backgroundColor = style.Colors[ImGuiCol_WindowBg];
    style.Colors[ImGuiCol_TitleBgActive] = backgroundColor;



    //lets change the font!
    io.Fonts->AddFontFromFileTTF("Fonts/Inter-Regular.ttf", 16.0f);


    

   
}

char buffer[99999^9999];//never run out


int main() {


    // Init GLFW
    initGLFW();

   
    
    //generate a window
    window = glfwCreateWindow(800, 800, "Test win", NULL, NULL);
    if (window == NULL) {
        //if we cant create a window
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    //load glad, and check if it has errors
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
  
    //init imgui

    initImgui();

    static float editorHeightPercent = 0.7f;
    static float terminalHeightPercent = 0.2f;

    //set the size
    glViewport(0, 0, 800, 800);

    int windowWidth, windowHeight;
   
   
    while (!glfwWindowShouldClose(window)) {
        //render imgui
   
       glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);

        // Clear the screen
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //Ui win

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //from headbar.h
        Renderbar();
        //lets create the win

        float windowHeight = ImGui::GetIO().DisplaySize.y;
        float windowWidth = ImGui::GetIO().DisplaySize.x;

        // Calculate the heights
        float terminalHeight = windowHeight * terminalHeightPercent;
        float editorHeight = windowHeight - terminalHeight - 20; 
        float editorWidth = windowWidth;

        // Editor window
        ImGui::SetNextWindowSize(ImVec2(editorWidth, editorHeight));
        ImGui::SetNextWindowPos(ImVec2(200.1f, 20)); // Lock top position
        ImGui::Begin("Editor", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
        ImGui::InputTextMultiline("##CodeEditor", buffer, IM_ARRAYSIZE(buffer), ImVec2(-1.0f, -1.0f), ImGuiInputTextFlags_AllowTabInput);
        ImGui::End();

        // Inspector window
        

        // Terminal window
        ImGui::SetNextWindowSize(ImVec2(windowWidth, terminalHeight));
        ImGui::SetNextWindowPos(ImVec2(0, windowHeight - terminalHeight));
        ImGui::Begin("Terminal", nullptr, ImGuiWindowFlags_NoCollapse);

        // Update terminal height if it has been resized
        terminalHeight = ImGui::GetWindowHeight();
        terminalHeightPercent = terminalHeight / windowHeight;

        ImGui::End();




        //render

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());



      
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //clean up
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
