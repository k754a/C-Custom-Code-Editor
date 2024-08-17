// Settings.cpp
#include "Settings.h"
#include "imgui.h"
#include "imgui_internal.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <windows.h>
#include <shlobj.h> 
#include <locale>
#include <codecvt>
#include "stb/stb_image.h"
#include <direct.h> // For mkdir
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

bool settings = false;
ImVec4 warningc = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
bool devmode = false, rendergraph = false;
int c = 0;
bool winfpsread = false;
bool darkMode = true;
bool autoSave = false;
bool BetterMouseImage = false;

static bool ret;
static GLuint texture;
static int texWidth, texHeight;

static int my_image_width = 1;
static int my_image_height = 1;
static GLuint my_image_texture = 1;
static float textHeight;
#include <algorithm>
static ImVec2 imageSize;

bool PagedFileSetting = false;

constexpr int MAX_HISTORY_SIZE = 100;
std::vector<float> cpuHistory(MAX_HISTORY_SIZE, 0.0f);
std::vector<float> ramHistory(MAX_HISTORY_SIZE, 0.0f);
int historyIndex = 0;

namespace {
    // Function to load a texture from memory
    bool LoadTextureFromMemory(const void* data, size_t data_size, GLuint* out_texture, int* out_width, int* out_height)
    {
        int image_width = 0;
        int image_height = 0;
        unsigned char* image_data = stbi_load_from_memory((const unsigned char*)data, (int)data_size, &image_width, &image_height, NULL, 4);
        if (image_data == NULL)
            return false;

        GLuint image_texture;
        glGenTextures(1, &image_texture);
        glBindTexture(GL_TEXTURE_2D, image_texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
        stbi_image_free(image_data);

        *out_texture = image_texture;
        *out_width = image_width;
        *out_height = image_height;

        return true;
    }

    // Function to load a texture from a file
    bool LoadTextureFromFile(const char* filename, GLuint* texture, int* width, int* height) {
        int imgWidth, imgHeight, imgChannels;
        unsigned char* imgData = stbi_load(filename, &imgWidth, &imgHeight, &imgChannels, STBI_rgb_alpha);
        if (!imgData) {
            std::cerr << "Failed to load texture: " << filename << std::endl;
            return false;
        }

        glGenTextures(1, texture);
        glBindTexture(GL_TEXTURE_2D, *texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgData);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(imgData);

        *width = imgWidth;
        *height = imgHeight;

        return true;
    }

    // Function to render an image in ImGui
    void RenderImageInImGui(GLuint texture, int width, int height) {
        ImGui::Begin("Texture Viewer");
        ImGui::Text("Texture Dimensions: %d x %d", width, height);
        ImGui::Image((void*)(intptr_t)texture, ImVec2(static_cast<float>(width), static_cast<float>(height)));
        ImGui::End();
    }
}

void UpdateUsageData() {
    float cpuUsage = 50.0f;
    float ramUsage = 30.0f;

    cpuHistory[historyIndex] = cpuUsage;
    ramHistory[historyIndex] = ramUsage;

    historyIndex = (historyIndex + 1) % MAX_HISTORY_SIZE;
}

std::string GetCppFilePath(std::string htmlname) {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string exePath(buffer);
    std::string cppFilePath = exePath.substr(0, exePath.find_last_of("\\/"));

    cppFilePath = cppFilePath.substr(0, cppFilePath.find_last_of("\\/"));
    cppFilePath = cppFilePath.substr(0, cppFilePath.find_last_of("\\/"));

    return cppFilePath + "\\Project2\\Docs\\" + htmlname;
}

void OpenHTMLFile(const std::string& filePath) {
    std::string command;

#ifdef _WIN32

    command = "start \"\" \"" + filePath + "\"";
#else

    command = "xdg-open \"" + filePath + "\"";
#endif

    system(command.c_str());
}

void GFS(float scale) {
    ImGuiIO& io = ImGui::GetIO();
    io.FontGlobalScale = scale;
}

static float GFSSCALE = 1.0f;
void Settingsrender() {
    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
    ImVec4 bgcolor = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
    bgcolor.w = 0.9f;
    ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = bgcolor;

    ret = LoadTextureFromFile("C:\\Users\\K754a\\source\\repos\\Project2\\Project2\\Images\\open.png", &my_image_texture, &my_image_width, &my_image_height);
    IM_ASSERT(ret);  // Ensure the texture loading succeeded
    textHeight = ImGui::GetTextLineHeight();

    // Adjust the image size to match the text height
    ImVec2 imageSizea(textHeight, textHeight);

    ImGui::Image((void*)(intptr_t)my_image_texture, imageSizea);
    ImGui::SameLine();  // Aligns the text to the right of the image
    ImGui::SetWindowFocus("Settings");
    if (ImGui::Begin("Settings", &settings, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize)) {
        if (ImGui::BeginTabBar("SettingsTabBar")) {




           
            if (ImGui::BeginTabItem("Editor")) {
                ImGui::Separator();
                ImGui::Checkbox("Auto-Save", &autoSave);

                if (autoSave)
                {
                    ImGui::Text("Auto-Save is enabled.");
                }
                else
                {
                    ImGui::Text("Auto-Save is disabled.");
                }

                ImGui::Separator();

                ImGui::Checkbox("Paged file Setting", &PagedFileSetting);

                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("This setting controls whether files are paged.");
                }

                ImGui::SameLine();
                if (ImGui::SmallButton("What's This?")) {
                    std::string docPath = GetCppFilePath("Paged File Setting.html");
                    OpenHTMLFile(docPath);
                }

                ImGui::Separator();
                ImGui::TextColored(warningc, "Warning");
                ImGui::TextWrapped("Changing any of the settings below could cause issues or instability.");

                ImGui::Separator();
                if (ImGui::Button("[x] Clear Global Memory")) {
                    if (std::remove(".\\Psettings\\CfilePath.FUNCT") == 0) {
                        std::cout << "File deleted successfully.\n";
                        settings = false;
                    }
                    else {
                        std::cout << "File does not exist.\n";
                    }
                }

                ImGui::Checkbox("Developer Mode", &devmode);

                ImGui::InputFloat("TEXT SIZE", &GFSSCALE, 0.1f, 1.0f);
                GFS(GFSSCALE);

                ImGui::EndTabItem();
                ImGui::Checkbox("MouseTrack", &BetterMouseImage);

                ImGui::SameLine();
                if (ImGui::SmallButton("What's This?")) {
                    std::string docPath = GetCppFilePath("Mouse Track.html");
                    OpenHTMLFile(docPath);
                }

                if (BetterMouseImage) {
                    ImGui::Text("Mouse Track is enabled.");

                }
                else {
                    ImGui::Text("Mouse Track is disabled.");
                }

            }

            if (ImGui::BeginTabItem("Current Files")) {
                ImGui::Text("This is where you'd list current files.");
                ImGui::EndTabItem();
            }

            if (devmode) {
                if (ImGui::BeginTabItem("Developer")) {
                    if (ImGui::Button("Enable system resource display")) {
                        rendergraph = true;
                    }

                    if (rendergraph) {
                        if (c < 60) {
                            c++;
                        }
                        else {
                            UpdateUsageData();
                            c = 0;
                        }
                        ImGui::PlotLines("CPU Usage", cpuHistory.data(), static_cast<int>(cpuHistory.size()), 0, nullptr, 0.0f, 100.0f, ImVec2(0, 80));
                        ImGui::PlotLines("RAM Usage", ramHistory.data(), static_cast<int>(ramHistory.size()), 0, nullptr, 0.0f, 100.0f, ImVec2(0, 80));
                    }

                    ImGui::Checkbox("Enable Window FPS reader", &winfpsread);

                    if (ImGui::Button("Open Style Folder")) {
                        system("start .\\Style");
                    }

                    ImGui::EndTabItem();
                }
            }

            if (!devmode) {
                if (ImGui::BeginTabItem("Debug")) {
                    ImGui::Text("Save statistics data? This is if you have an issue.");
                    if (ImGui::Button("Yes")) {
                        std::ofstream outFile("DEBUGTEXT.LOG");
                        outFile << "NOTE: This is not shared with anyone, if you have issues this could help if something is wrong" << std::endl;
                        outFile << "Computer Name: EXAMPLE_COMPUTER_NAME" << std::endl;
                        outFile << "CPU Model: EXAMPLE_CPU_MODEL" << std::endl;
                        outFile << "RAM Info: EXAMPLE_RAM_INFO" << std::endl;
                        outFile << "Storage Info: EXAMPLE_STORAGE_INFO" << std::endl;
                        outFile << "CPU usage history: [";
                        for (auto val : cpuHistory) outFile << val << ", ";
                        outFile << "]" << std::endl;
                        outFile << "RAM usage history: [";
                        for (auto val : ramHistory) outFile << val << ", ";
                        outFile << "]" << std::endl;
                        outFile.close();
                        std::cout << "File created and written successfully." << std::endl;
                    }
                    ImGui::EndTabItem();
                }
            }

            if (ImGui::BeginTabItem("Documentation")) {
                ImGui::Text("This is the link to the documentation below!");
                if (ImGui::Button("[Link] Editor Documentation")) {
                    system("start https://k754a.github.io/Editor%20Documentation");
                }
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();
    }
}
