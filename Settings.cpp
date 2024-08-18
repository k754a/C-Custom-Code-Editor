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

    std::vector<GLuint> textureIDs;
    // Function to load a texture from memory
    bool LoadTextureFromMemory(const void* data, size_t data_size, GLuint* out_texture, int* out_width, int* out_height) {
        // Load image from memory
        int image_width = 0;
        int image_height = 0;
        unsigned char* image_data = stbi_load_from_memory((const unsigned char*)data, (int)data_size, &image_width, &image_height, NULL, STBI_rgb_alpha);
        if (image_data == NULL) {
            std::cerr << "Failed to load image from memory." << std::endl;
            return false;
        }

        // Create an OpenGL texture identifier
        GLuint image_texture;
        glGenTextures(1, &image_texture);
        glBindTexture(GL_TEXTURE_2D, image_texture);

        // Setup filtering parameters for display
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Upload pixels into texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Free image data
        stbi_image_free(image_data);

        *out_texture = image_texture;
        *out_width = image_width;
        *out_height = image_height;

        // Store the texture ID
        textureIDs.push_back(image_texture);

        return true;
    }

    bool LoadTextureFromFile(const char* filename, GLuint* texture, int* width, int* height) {
        // Load image file
        int imgWidth, imgHeight, imgChannels;
        unsigned char* imgData = stbi_load(filename, &imgWidth, &imgHeight, &imgChannels, STBI_rgb_alpha);
        if (!imgData) {
            std::cerr << "Failed to load texture: " << filename << std::endl;
            return false;
        }

        // Generate and bind texture
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Use mipmaps for better quality
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Upload texture data
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgData);
        glGenerateMipmap(GL_TEXTURE_2D); // Generate mipmaps

        // Free image data
        stbi_image_free(imgData);

        *texture = textureID;
        *width = imgWidth;
        *height = imgHeight;

        // Store the texture ID
        textureIDs.push_back(textureID);

        return true;
    }


    // Function to render an image in ImGui
    void RenderImageInImGui(GLuint texture, int width, int height) {
        ImGui::Begin("Texture Viewer");
        ImGui::Text("Texture Dimensions: %d x %d", width, height);
        ImGui::Image((void*)(intptr_t)texture, ImVec2(static_cast<float>(width), static_cast<float>(height)));
        ImGui::End();
    }

    void FlushTextures() {
        if (!textureIDs.empty()) {
            glDeleteTextures(static_cast<GLsizei>(textureIDs.size()), textureIDs.data());
            textureIDs.clear();
        }
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
    FlushTextures();
    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
    ImVec4 bgcolor = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
    bgcolor.w = 0.9f;
    ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = bgcolor;

    ImGui::SetWindowFocus("Settings");
    if (ImGui::Begin("Settings", &settings, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize)) {
        if (ImGui::BeginTabBar("SettingsTabBar")) {



         
           


            if (ImGui::BeginTabItem("Editor")) {
                ImGui::Separator();
                ret = LoadTextureFromFile(".\\Images\\Autosave.png", &my_image_texture, &my_image_width, &my_image_height);
                IM_ASSERT(ret);  // Ensure the texture loading succeeded
                textHeight = ImGui::GetTextLineHeight();

                // Adjust the image size to match the text height
                ImVec2 imageSizea(textHeight, textHeight);

                ImGui::Image((void*)(intptr_t)my_image_texture, imageSizea);
                ImGui::SameLine();  // Aligns the text to the right of the image
                ImGui::Text("Auto-Save");
                ImGui::SameLine(); 
                ImGui::Checkbox("##AutoSaveCheckbox", &autoSave);  

                if (autoSave)
                {
                    ImGui::Text("Auto-Save is enabled.");
                }
                else
                {
                    ImGui::Text("Auto-Save is disabled.");
                }

                ImGui::Separator();


                ret = LoadTextureFromFile(".\\Images\\Pages.png", &my_image_texture, &my_image_width, &my_image_height);
                IM_ASSERT(ret);  // Ensure the texture loading succeeded
                textHeight = ImGui::GetTextLineHeight();

                // Adjust the image size to match the text height
                ImVec2 imageSizeb(textHeight, textHeight);

                ImGui::Image((void*)(intptr_t)my_image_texture, imageSizeb);
                ImGui::SameLine();  // Aligns the text to the right of the image
               
                ImGui::Text("Paged file Setting");
                ImGui::SameLine();
                ImGui::Checkbox("##Pagesetting", &PagedFileSetting);
               

                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("This setting controls whether files are paged.");
                }

                ImGui::SameLine();
                if (ImGui::SmallButton("What's This?")) {
                    std::string docPath = GetCppFilePath("Paged File Setting.html");
                    OpenHTMLFile(docPath);
                }

                ImGui::Separator();




                ret = LoadTextureFromFile(".\\Images\\warningR.png", &my_image_texture, &my_image_width, &my_image_height);
                IM_ASSERT(ret);  // Ensure the texture loading succeeded
                textHeight = ImGui::GetTextLineHeight();

                // Adjust the image size to match the text height
                ImVec2 imageSizec(textHeight, textHeight);

                ImGui::Image((void*)(intptr_t)my_image_texture, imageSizec);
                ImGui::SameLine();  // Aligns the text to the right of the image

                ImGui::TextColored(warningc, "Warning");
                ImGui::SameLine();
                ImGui::Image((void*)(intptr_t)my_image_texture, imageSizec);
                ImGui::TextWrapped("Changing any of the settings below could cause issues or instability.");

                ImGui::Separator();

                ret = LoadTextureFromFile(".\\Images\\trash.png", &my_image_texture, &my_image_width, &my_image_height);
                IM_ASSERT(ret);  // Ensure the texture loading succeeded
                textHeight = ImGui::GetTextLineHeight();

                // Adjust the image size to match the text height
                ImVec2 imageSized(textHeight, textHeight);

                ImGui::Image((void*)(intptr_t)my_image_texture, imageSized);
                ImGui::SameLine();  // Aligns the text to the right of the image
                if (ImGui::Button("Clear Global Memory")) {
                    if (std::remove(".\\Psettings\\CfilePath.FUNCT") == 0) {
                        std::cout << "File deleted successfully.\n";
                        settings = false;
                    }
                    else {
                        std::cout << "File does not exist.\n";
                    }
                }
                ret = LoadTextureFromFile(".\\Images\\Dev.png", &my_image_texture, &my_image_width, &my_image_height);
                IM_ASSERT(ret);  // Ensure the texture loading succeeded
                textHeight = ImGui::GetTextLineHeight();

                // Adjust the image size to match the text height
                ImVec2 imageSizee(textHeight, textHeight);

                ImGui::Image((void*)(intptr_t)my_image_texture, imageSizee);
                ImGui::SameLine();
                ImGui::Text("Developer Mode");
                ImGui::SameLine();
          
                ImGui::Checkbox("##DeveloperMode", &devmode);




                ret = LoadTextureFromFile(".\\Images\\textsize.png", &my_image_texture, &my_image_width, &my_image_height);
                IM_ASSERT(ret);  // Ensure the texture loading succeeded
                textHeight = ImGui::GetTextLineHeight();

                // Adjust the image size to match the text height
                ImVec2 imageSizef(textHeight, textHeight);

                ImGui::Image((void*)(intptr_t)my_image_texture, imageSizef);
                ImGui::SameLine();
                ImGui::InputFloat("TEXT SIZE", &GFSSCALE, 0.1f, 1.0f);
                GFS(GFSSCALE);

                ImGui::EndTabItem();



                ret = LoadTextureFromFile(".\\Images\\mouse.png", &my_image_texture, &my_image_width, &my_image_height);
                IM_ASSERT(ret);  // Ensure the texture loading succeeded
                textHeight = ImGui::GetTextLineHeight();

                // Adjust the image size to match the text height
                ImVec2 imageSizeg(textHeight, textHeight);

                ImGui::Image((void*)(intptr_t)my_image_texture, imageSizeg);
				ImGui::SameLine();

                ImGui::Text("MouseTrack");
                ImGui::SameLine();

                ImGui::Checkbox("##MouseTrack", &BetterMouseImage);
            

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


                    ret = LoadTextureFromFile(".\\Images\\FPSSHOT.png", &my_image_texture, &my_image_width, &my_image_height);
                    IM_ASSERT(ret);  // Ensure the texture loading succeeded
                    textHeight = ImGui::GetTextLineHeight();

                    // Adjust the image size to match the text height
                    ImVec2 imageSizeg(textHeight, textHeight);

                    ImGui::Image((void*)(intptr_t)my_image_texture, imageSizeg);
                    ImGui::SameLine();


                    ImGui::Text("Enable Window FPS reader");
                    ImGui::SameLine();

                    ImGui::Checkbox("##winfpsread", &winfpsread);

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
