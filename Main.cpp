#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>
#include"Texture.h"
#include <cmath>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "shaderclass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "MenuBar.h"
#include <fstream>
#include "Save.h"
#include <sstream>
#include <string>
#include <vector>

// OpenGL documentation: https://registry.khronos.org/OpenGL-Refpages/gl4/
// Tutorials: https://www.youtube.com/watch?v=XpBGwZNyUh0&list=PLPaoO-vpZnumdcb4tZc4x5Q-v7CkrQ6M-&index=1

std::string fileName = ".CoRF";

std::string floatToString(GLfloat value); // Function prototype

GLfloat vertices[] = {
    // Coordinates             // Colors             // Texture Coordinates
    -0.5f, -0.5f , 0.0f,       1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
    -0.5f, 0.5f, 0.0f,         0.0f, 1.0f, 0.0f,    0.0f, 1.0f,
    0.5f, 0.5f , 0.0f,         0.0f, 0.0f, 1.0f,    1.0f, 1.0f,
    0.5f , -0.5f , 0.0f,       1.0f, 1.0f, 1.0f,    1.0f, 0.0f
};

GLuint indices[] = {
    0, 2, 1,
    0, 3, 2
};

int main() {
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // Create window
    GLFWwindow* window = glfwCreateWindow(800, 800, "R Engine", NULL, NULL);
    if (window == NULL) {
        std::cout << "Error: Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    gladLoadGL();

    // Set viewport
    glViewport(0, 0, 800, 800);

    Shader shaderProgram("shaders/default.vert", "shaders/default.frag");
    VAO VAO1;
    VAO1.Bind();
    VBO VBO1(vertices, sizeof(vertices));
    EBO EBO1(indices, sizeof(indices));

    VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
    VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    VAO1.Unbind();
    VBO1.Unbind();
    EBO1.Unbind();

    GLuint uniIDx = glGetUniformLocation(shaderProgram.ID, "scalex");
    GLuint uniIDy = glGetUniformLocation(shaderProgram.ID, "scaley");
    glUniform1f(uniIDx, 1.0f);
    glUniform1f(uniIDy, 1.0f);

    Texture texture("example.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    texture.texUnit(shaderProgram, "tex0", 0);
    texture.Bind();

    glClearColor(0.9f, 0.12f, 1.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Variables
    bool drawTriangle = true;
    bool slider = false;
    bool customImg = false;
    bool LcustomImg = false;
    bool Devmode = false;
    bool CConfig = false;

    float x = 1.0f;
    float y = 1.0f;

    std::vector<std::string> data = {};
    std::vector<std::string> savedata = { "./saves/example" };

    std::string inputPath = "example.png";
    char buffer[256] = "obj";
    char winbuffer[30] = "";
    char CBuffer[600] = "";

    std::vector<std::string> buttonNames = { "example" };
    float selectedButton = 0;
    int currentbuttonindex = 0;

    while (!glfwWindowShouldClose(window)) {
        glViewport(0, 0, 800, 800);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        shaderProgram.Activate();
        glUniform1f(uniIDx, x / 100);
        glUniform1f(uniIDy, y / 100);
        VAO1.Bind();

        size_t numElements = sizeof(indices) / sizeof(indices[0]);
        if (drawTriangle) {
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_INT, 0);
        }

        ImGui::Begin(buttonNames[currentbuttonindex].c_str());

        if (ImGui::InputText("Obj Name", winbuffer, IM_ARRAYSIZE(winbuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
            if (strlen(winbuffer) > 0 && std::find(buttonNames.begin(), buttonNames.end(), winbuffer) == buttonNames.end()) {
                std::string filePath = "saves/" + buttonNames[currentbuttonindex] + ".CoRF";
                if (std::remove(filePath.c_str()) != 0) {
                    std::cerr << "Error deleting file: " << filePath << std::endl;
                }
                else {
                    std::cout << "File successfully deleted: " << filePath << std::endl;
                }
                buttonNames[currentbuttonindex] = winbuffer;
            }

            std::vector<std::string> nbs = { buttonNames[currentbuttonindex] };
            std::string cts_nbs;
            for (const auto& str : nbs) {
                cts_nbs += str;
            }

            data = { buttonNames[currentbuttonindex], std::to_string(x), std::to_string(y), std::to_string(Devmode), std::to_string(customImg), inputPath };
            for (int i = 0; i < 32; ++i) {
                data.push_back(floatToString(vertices[i]));
            }
            data.emplace_back("//indices");
            saveToFile(data, cts_nbs + fileName);
        }

        ImGui::Checkbox("Render?", &drawTriangle);

        if (!slider) {
            ImGui::InputFloat("X Size", &x);
            ImGui::InputFloat("Y Size", &y);
            if (x <= 0.0009) x = 1;
            if (y <= 0.0009) y = 1;
        }
        else {
            ImGui::SliderFloat("X", &x, 1.0f, 100.0f);
            ImGui::SliderFloat("Y", &y, 1.0f, 100.0f);
        }

        ImGui::Checkbox("Slider or Float", &slider);
        ImGui::Checkbox("Custom Image", &customImg);

        if (customImg && ImGui::InputText("Texture Path", buffer, IM_ARRAYSIZE(buffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
            inputPath = buffer;
            texture = Texture(inputPath.c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
            texture.texUnit(shaderProgram, "tex0", 0);
            texture.Bind();
            glClearColor(0.9f, 0.12f, 1.17f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glfwSwapBuffers(window);

            std::vector<std::string> nbs = { buttonNames[currentbuttonindex] };
            std::string cts_nbs;
            for (const auto& str : nbs) {
                cts_nbs += str;
            }

            data = { buttonNames[currentbuttonindex], std::to_string(x), std::to_string(y), std::to_string(Devmode), std::to_string(customImg), inputPath };
            for (int i = 0; i < 32; ++i) {
                data.push_back(floatToString(vertices[i]));
            }
            data.emplace_back("//indices");
            saveToFile(data, cts_nbs + fileName);
        }

        ImGui::Checkbox("Developer Mode", &Devmode);

        if (Devmode) {
            ImGui::Checkbox("Custom Configuration", &CConfig);

            if (CConfig) {
                if (ImGui::InputText("Configurations", CBuffer, IM_ARRAYSIZE(CBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
                    std::ofstream outFile("configs.conf");
                    outFile << CBuffer;
                    outFile.close();
                }
            }

            if (ImGui::Button("Print Log")) {
                std::ifstream infile("log.txt");
                std::string line;
                while (std::getline(infile, line)) {
                    std::cout << line << std::endl;
                }
            }

            if (ImGui::Button("Load Saved Files")) {
                std::ifstream infile("./saves/example/obj.CoRF");
                std::string line;
                savedata.clear();
                while (std::getline(infile, line)) {
                    savedata.push_back(line);
                }
                for (const auto& item : savedata) {
                    std::cout << item << std::endl;
                }
                std::cout << "x: " << x << std::endl;
                std::cout << "y: " << y << std::endl;
                std::cout << "Developer Mode: " << Devmode << std::endl;
                std::cout << "Custom Image: " << customImg << std::endl;
                std::cout << "Input Path: " << inputPath << std::endl;
            }

            if (ImGui::Button("Load Saved File")) {
                data = readFromFile("./saves/example/obj.CoRF");
                buttonNames.push_back(data[0]);
            }

            if (ImGui::Button("Add object")) {
                if (strlen(buffer) > 0) {
                    buttonNames.push_back(buffer);
                }
            }
        }

        ImGui::End();

        for (size_t i = 0; i < buttonNames.size(); ++i) {
            if (ImGui::Button(buttonNames[i].c_str())) {
                currentbuttonindex = i;
                std::vector<std::string> nb = { buttonNames[currentbuttonindex] };
                std::string cts_nb;
                for (const auto& str : nb) {
                    cts_nb += str;
                }
                data = { buttonNames[currentbuttonindex], std::to_string(x), std::to_string(y), std::to_string(Devmode), std::to_string(customImg), inputPath };
                for (int i = 0; i < 32; ++i) {
                    data.push_back(floatToString(vertices[i]));
                }
                data.emplace_back("//indices");
                saveToFile(data, cts_nb + fileName);
            }
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    VAO1.Delete();
    VBO1.Delete();
    EBO1.Delete();
    texture.Delete();
    shaderProgram.Delete();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

std::string floatToString(GLfloat value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}
