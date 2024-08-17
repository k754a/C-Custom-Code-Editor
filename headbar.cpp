#include "headbar.h"
#include "libraries.h"
#include "Settings.h"
#include <thread> 
#include "Python/include/Python.h"
#include <chrono>
#include <mutex>
#include <filesystem>
#include "Terminal.h"
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>

// Global variables and structures
struct FileNode {
    std::wstring name;
    bool isDirectory = false; // Initialize here
    std::wstring path;
    std::vector<FileNode> children;
};

struct FileChunk {
    std::vector<FileNode> nodes;
    bool isLoaded = false;
};

std::string currentFilePath; // Current file path
int fps;
std::string fpsString;
std::vector<FileNode> fileTree;
std::wstring save = L"";
std::string content;
std::vector<char> bufferContent;

std::string CURRENT = "CURRENT";
std::atomic<int> cout(0);
#include <cstddef>
// Function declarations
void PrintFileContent(const FileNode& node);
void ListFilesRecursively(const std::wstring& directory, FileNode& node);
void OpenFile();
std::string CWstrTostr(const std::wstring& wstr);
void DisplayFile(const FileNode& node, bool isChildVisible = false);

void Renderbar();
void incrementCout();
std::string ReadFileToString(const std::string& filePath);
void Settingsrender(); // Assuming this is defined elsewhere
void RenderTerminal(float windowWidth, float windowHeight, float terminalHeight); // Assuming this is defined elsewhere

void CreateFileChunks(const std::vector<FileNode>& nodes, size_t chunkSize);
std::string wchar_to_string(const wchar_t* wstr);
std::string double_path_separators(const std::string& path);
std::string readFileContents(const std::string& saveFilePath);
std::vector<char> readFileContent(const std::string& filePath);

// Additional global variables and functions
std::vector<FileChunk> fileChunks;
int currentChunkIndex = 0;
static float terminalHeightPercent = 0.2f;

void CreateFileChunks(const std::vector<FileNode>& nodes, size_t chunkSize) {
    fileChunks.clear();

    for (size_t i = 0; i < nodes.size(); i += chunkSize) {
        FileChunk chunk;
        size_t end = i + chunkSize;

        if (end > nodes.size()) {
            end = nodes.size();
        }

        for (size_t j = i; j < end; ++j) {
            chunk.nodes.push_back(nodes[j]);
        }

        fileChunks.push_back(chunk);
    }
}


std::string readFileContents(const std::string& saveFilePath) {
    std::ifstream file(saveFilePath);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << saveFilePath << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string wchar_to_string(const wchar_t* wstr) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(wstr);
}

void incrementCout() {
    while (true) {
        if (cout < 2) {
            cout++;
        }
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

void ListFilesRecursively(const std::wstring& directory, FileNode& node) {
    std::wstring searchPath = directory + L"\\*";
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile(searchPath.c_str(), &findFileData);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (wcscmp(findFileData.cFileName, L".") != 0 && wcscmp(findFileData.cFileName, L"..") != 0) {
                FileNode child;
                child.name = findFileData.cFileName;
                child.path = directory + L"\\" + findFileData.cFileName;
                child.isDirectory = (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
                node.children.push_back(child);

                if (child.isDirectory) {
                    ListFilesRecursively(child.path, node.children.back());
                }
            }
        } while (FindNextFile(hFind, &findFileData) != 0);

        FindClose(hFind);
    }
    else {
        std::wcerr << L"Error: " << directory << std::endl;
    }
}

void OpenFile() {
    BROWSEINFO bi = { 0 };
    bi.lpszTitle = L"Select a folder";
    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

    if (pidl != nullptr) {
        wchar_t szDir[MAX_PATH];
        if (SHGetPathFromIDList(pidl, szDir)) {
            FileNode root;
            root.name = szDir;
            root.isDirectory = true;
            ListFilesRecursively(szDir, root);
            fileTree.push_back(root);

            std::string savefolder = "Psettings";
            std::string fileP = savefolder + "/CfilePath.FUNCT";
            if (_mkdir(savefolder.c_str()) != 0 && errno != EEXIST) {
                std::cerr << "Error creating directory: " << savefolder << std::endl;
            }

            std::ofstream Savefile(fileP);
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> fileConverter;
            Savefile << fileConverter.to_bytes(save);
            Savefile.close();

            std::wstring command = L"python -m http.server --directory \"" + std::wstring(szDir) + L"\" 8000";
            std::vector<wchar_t> cmd(command.begin(), command.end());
            cmd.push_back(0); // Null-terminate the command

            STARTUPINFO si = { sizeof(si) };
            PROCESS_INFORMATION pi;
            if (!CreateProcess(NULL, cmd.data(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
                std::wcerr << L"Failed to start local server" << std::endl;
            }
            else {
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
            }

            std::string htmlFilePath = "http://localhost:8000/index.html";
            std::wstring wideHtmlFilePath = fileConverter.from_bytes(htmlFilePath);
            ShellExecute(0, 0, wideHtmlFilePath.c_str(), 0, 0, SW_SHOW);
        }
        else {
            std::wcerr << L"Error getting folder path" << std::endl;
        }
        CoTaskMemFree(pidl);
    }
    else {
        std::wcerr << L"Folder selection canceled" << std::endl;
    }
}

std::string CWstrTostr(const std::wstring& wstr) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(wstr);
}

float GetCurrentTimeSeconds() {
    return std::chrono::duration<float>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

void DisplayFile(const FileNode& node, bool isChildVisible) {
    const std::string& nodeName = CWstrTostr(node.name);

    if (node.isDirectory) {
        if (ImGui::TreeNode(nodeName.c_str())) {
            // If a directory, display its children
            for (const auto& child : node.children) {
                DisplayFile(child, isChildVisible);
            }
            ImGui::TreePop();
        }
    }
    else {
        // If a file, make it selectable
        if (ImGui::Selectable(nodeName.c_str())) {
            PrintFileContent(node);
        }
    }
}


void PrintFileContent(const FileNode& node) {
    std::ifstream fileStream(CWstrTostr(node.path), std::ios::binary | std::ios::ate);
    if (fileStream.is_open()) {
        std::streamsize fileSize = fileStream.tellg();
        fileStream.seekg(0, std::ios::beg);

        bufferContent.resize(fileSize + 1);
        if (fileStream.read(bufferContent.data(), fileSize)) {
            bufferContent[fileSize] = '\0';
            content.assign(bufferContent.data(), fileSize);
            currentFilePath = CWstrTostr(node.path);
        }
        else {
            std::cerr << "ERROR: Failed to read file " << CWstrTostr(node.path) << std::endl;
        }

        fileStream.close();
    }
    else {
        std::cerr << "ERROR: file_not_found, error 101 " << CWstrTostr(node.path) << std::endl;
        std::cerr << "Cannot find the selected file, sorry! " << CWstrTostr(node.path) << std::endl;
    }
}

std::string ReadFileToString(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}


void Renderbar() {
    float windowHeight = ImGui::GetIO().DisplaySize.y;
    float windowWidth = ImGui::GetIO().DisplaySize.x;
    float terminalHeight = windowHeight * terminalHeightPercent;
    float editorHeight = windowHeight - terminalHeight - 20;
    float editorWidth = windowWidth;

    ImGui::SetNextWindowSize(ImVec2(200, windowHeight - terminalHeight - 20));
    ImGui::SetNextWindowPos(ImVec2(0, 20)); // Lock top position
    ImGui::Begin("Explorer", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    if (PagedFileSetting) {
        if (currentChunkIndex < fileChunks.size()) {
            const auto& currentChunk = fileChunks[currentChunkIndex];
            for (const auto& node : currentChunk.nodes) {
                DisplayFile(node);
            }
        }

        // Navigation buttons
        if (ImGui::Button("Previous") && currentChunkIndex > 0) {
            currentChunkIndex--;
        }
        ImGui::SameLine();
        ImGui::Text("Chunk %d/%d", currentChunkIndex + 1, fileChunks.size());
        ImGui::SameLine();
        if (ImGui::Button("Next") && currentChunkIndex < fileChunks.size() - 1) {
            currentChunkIndex++;
        }
    }
    else {
        // Normal file listing
        for (const auto& node : fileTree) {
            DisplayFile(node);
        }
    }
    
    ImGui::End();

    RenderTerminal(windowWidth, windowHeight, terminalHeight);

    bufferContent.resize(content.size() + bufferContent.size());
    ImGui::SetNextWindowSize(ImVec2(editorWidth + 10, editorHeight));
    ImGui::SetNextWindowPos(ImVec2(200.1f, 20)); // Lock top position
    ImGui::Begin("Editor", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
    std::string line;
    std::stringstream fileBuffer;

    std::string newContent = fileBuffer.str();
    std::vector<char> newContentVec(newContent.begin(), newContent.end());

    bufferContent.insert(bufferContent.end(), newContentVec.begin(), newContentVec.end());

    bufferContent.push_back('\0');

    std::string buf = bufferContent.data();

    bufferContent.resize(buf.length() * buf.length());

    if (!bufferContent.empty()) {
        // Render the text editor
        ImGui::InputTextMultiline("##CodeEditor", bufferContent.data(), bufferContent.size(), ImVec2(-1.0f, -1.0f), ImGuiInputTextFlags_AllowTabInput);
    }
    else {
        ImGui::Text("Please Open A file...");
    }


   

    // Compare buffer content with file content
    if (autoSave) {
        std::ifstream inFile(currentFilePath, std::ios::binary | std::ios::ate);
        if (inFile.is_open()) {
            auto fileSize = inFile.tellg();
            if (fileSize == bufferContent.size()) {
                inFile.seekg(0, std::ios::beg);
                std::vector<char> fileContent(fileSize);
                if (inFile.read(fileContent.data(), fileSize) && bufferContent == fileContent) {
                    inFile.close();
                   
                }
            }
            else {
                inFile.close();
            }

            
            std::ofstream outFile(currentFilePath, std::ios::binary);
            if (outFile.is_open()) {
                outFile.write(bufferContent.data(), bufferContent.size());
                outFile.close();
                std::cout << "Buffer content saved to " << currentFilePath << std::endl;
            }
        }
    }
   
    
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            ImGui::Separator();
            if (ImGui::MenuItem("Open Directory")) {
                OpenFile();
            }

           

            if (ImGui::MenuItem("Save")) {
                std::ofstream outFile(currentFilePath, std::ios::binary); // Open the file in binary mode
                if (outFile.is_open()) {
                    outFile.write(bufferContent.data(), std::strlen(bufferContent.data()));
                    outFile.close();
                    std::cout << "Buffer content saved to " << currentFilePath << std::endl;
                }
                else {
                    std::cerr << "Error: Unable to open file " << currentFilePath << std::endl;
                }
            }


            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) {
                // Handle exit
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Settings")) {
                settings = true;
            }
            if (ImGui::MenuItem("Close All Windows")) {
                settings = false;
                bufferContent.clear();
                content.clear();

                std::ifstream fileStream(CURRENT);
                if (fileStream.is_open()) {
                    std::string line;
                    std::stringstream buffer;

                    while (std::getline(fileStream, line)) {
                        buffer << line << '\n';
                    }

                    bufferContent.resize(content.size() + bufferContent.size() + 1); // +1 for null terminator
                    std::copy(content.begin(), content.end(), bufferContent.begin());
                    bufferContent.back() = '\0';

                    fileStream.close();
                }
            }
            ImGui::Separator();
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Run")) {
            if (ImGui::MenuItem("Run Script")) {
                if (currentFilePath.size() >= 3 && currentFilePath.substr(currentFilePath.size() - 3) == ".py") {
                    std::string doubled_path;
                    wchar_t path[MAX_PATH];
                    if (GetModuleFileName(NULL, path, MAX_PATH)) {
                        std::string path_str = wchar_to_string(path);

                        int count = 0;
                        for (int i = path_str.size() - 1; i >= 0; --i) {
                            if (path_str[i] == '\\' || path_str[i] == '/') {
                                count++;
                                if (count == 3) {
                                    path_str = path_str.substr(0, i);
                                    break;
                                }
                            }
                        }

                        doubled_path = path_str;

                        std::cout << "Modified Path: " << doubled_path << std::endl;
                    }
                    else {
                        std::cerr << "Error retrieving path" << std::endl;
                        ImGui::EndMenu();
                        return;
                    }

                    std::string saveFilePath = doubled_path + "\\script.py";

                    // Clean buffer content by removing null bytes
                    std::string cleanBufferContent;
                    for (char ch : bufferContent) {
                        if (ch != '\0') {
                            cleanBufferContent.push_back(ch);
                        }
                    }

                    std::ofstream outFile(saveFilePath);
                    if (outFile.is_open()) {
                        outFile.write(cleanBufferContent.data(), cleanBufferContent.size());
                        outFile.close();
                        std::cout << "Buffer content saved to " << saveFilePath << std::endl;
                    }
                    else {
                        std::cerr << "Error: Unable to open file " << saveFilePath << std::endl;
                        ImGui::EndMenu();
                        return;
                    }

                    std::string fileContents = readFileContents(saveFilePath);

                    // Execute the saved Python file
                    ExecutePythonCode(fileContents, "output.txt");
                }
                else if (currentFilePath.size() >= 5 && currentFilePath.substr(currentFilePath.size() - 5) == ".html") {
                    // Run the HTML file
                    std::string command = "start " + currentFilePath;
                    system(command.c_str());
                }
                else {
                    std::cerr << "Error: This is not a .py or .html file" << std::endl;
                }
            }
            ImGui::Separator();
            ImGui::EndMenu();
        }

        


        if (settings) {
            Settingsrender();
        }

        if (winfpsread) {
            ImGui::SameLine(ImGui::GetWindowWidth() - ImGui::CalcTextSize("Documentation").x - ImGui::GetStyle().ItemSpacing.x * 2);

            std::thread incrementThread(incrementCout);
            incrementThread.detach();

            if (cout < 2) {
               //got rid of print so slow :(
                cout++;
            }
            else {
                fpsString = "FPS:" + std::to_string(fps);
                fps = ImGui::GetIO().Framerate;
                cout = 0;
            }

            if (ImGui::BeginMenu(fpsString.c_str())) {
                ImGui::EndMenu();
            }
        }

        ImGui::EndMainMenuBar();
    }
}




//this would be wayyyy more cleaner if i could just leave all the funct on the bottom, but nope :(





