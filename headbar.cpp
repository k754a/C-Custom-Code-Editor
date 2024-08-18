#include "headbar.h"
#include "libraries.h"
#include "Settings.h" // Include the header file
#include <thread> // For sleep_for
#include "Python/include/Python.h"
#include <chrono>
#include <mutex>
#include <filesystem>
#include "Terminal.h"
#include <map>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"


#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


#include "imgui.h"








std::vector<GLuint> textureIDs;




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

// Open and read a file, then forward to LoadTextureFromMemory()
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


void FlushTextures() {
    if (!textureIDs.empty()) {
        glDeleteTextures(static_cast<GLsizei>(textureIDs.size()), textureIDs.data());
        textureIDs.clear();
    }
}




void RenderImageInImGui(GLuint texture, int width, int height) {
    ImGui::Begin("Texture Viewer");
    ImGui::Text("Texture Dimensions: %d x %d", width, height);
    ImGui::Image((void*)(intptr_t)texture, ImVec2(width, height));
    ImGui::End();
}



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

std::string currentFilePath; // C file path
int fps;
std::string fpsString;
std::vector<FileNode> fileTree;
std::wstring save = L"";
std::string content;
std::vector<char> bufferContent;
static std::map<std::string, bool> expandedDirectories;

std::string CURRENT = "CURRENT";
std::atomic<int> cout(0);

int currentPage = 0;
const int itemsPerPage = 10;


// Function declarations
void PrintFileContent(const FileNode& node);
void ListFilesRecursively(const std::wstring& directory, FileNode& node);
void OpenFile();
std::string CWstrTostr(const std::wstring& wstr);
void DisplayFile(const FileNode& node, bool isChildVisible = false, bool pagedFileSetting = false);

void Renderbar();
void incrementCout();
std::string ReadFileToString(const std::string& filePath);
void Settingsrender(); // Assuming this is defined elsewhere
void RenderTerminal(float windowWidth, float windowHeight, float terminalHeight); // Assuming this is defined elsewhere

// Function implementations

void incrementCout() {
    while (true) {
        if (cout < 2) {
            //lags to hard so we need this 
            //yo necesidad esto muy muy mucho
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
                    std::wstring subdir = directory + L"\\" + findFileData.cFileName;
                    ListFilesRecursively(subdir, node.children.back());
                }
            }
        } while (FindNextFile(hFind, &findFileData) != 0);

        FindClose(hFind);
    }
    else {
        std::wcerr << L"Error: " << directory.c_str() << std::endl;
    }
}

void OpenFile() {
    BROWSEINFO bi = { 0 };
    bi.lpszTitle = L"Select a folder";
    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

    if (pidl != nullptr) {
        wchar_t szDir[MAX_PATH];
        if (SHGetPathFromIDList(pidl, szDir)) {
            std::wcout << L"Selected folder: " << szDir << std::endl;
            // Get path and files in the path
            FileNode root;
            root.name = szDir;
            root.isDirectory = true;
            ListFilesRecursively(szDir, root);
            fileTree.push_back(root);
            std::wcout << L"Output: " << save << std::endl;
            std::wcout << L"Loaded!" << std::endl;

            std::string savefolder = "Psettings";
            std::string fileP = savefolder + "/CfilePath.FUNCT";
            // Make a folder
            if (_mkdir("Psettings") != 0 && errno != EEXIST) {
                std::cerr << "Error creating directory 'Psettings'" << std::endl;
            }
            // Create save file
            std::ofstream Savefile(fileP);

            // Use a unique name for the converter
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> fileConverter;
            std::string narrow_string = fileConverter.to_bytes(save);
            Savefile << narrow_string;
            Savefile.close();

            // Start a local server to serve the HTML file and its dependencies
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

            // Open the HTML file in the default browser
            std::string htmlFilePath = "http://localhost:8000/index.html"; // Assuming the main HTML file is index.html

            // Another unique converter
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> htmlConverter;
            std::wstring wideHtmlFilePath = htmlConverter.from_bytes(htmlFilePath);
            ShellExecute(0, 0, wideHtmlFilePath.c_str(), 0, 0, SW_SHOW);
        }
        else {
            std::wcerr << "Error getting folder path" << std::endl;
        }
        CoTaskMemFree(pidl);
    }
    else {
        std::wcerr << "Folder selection canceled" << std::endl;
    }
}





// Function to get current time in seconds
float GetCurrentTimeSeconds() {
    return std::chrono::duration<float>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

void DisplayFile(const FileNode& node, bool isChildVisible, bool pagedFileSetting) {
    static float previousScrollY = 0.0f;
    static float contentHeight = 0.0f;
    static float viewportHeight = 0.0f;
    static bool initialDisplayDone = false;
    static float lastUpdateTime = GetCurrentTimeSeconds();
    static constexpr float updateInterval = 0.5f; // 0.5 seconds

    float currentScrollY = ImGui::GetScrollY();
    viewportHeight = ImGui::GetWindowHeight();

    if (pagedFileSetting) {
        // Handle pagination
        int startIdx = currentPage * itemsPerPage;
        int endIdx = (startIdx + itemsPerPage < static_cast<int>(node.children.size())) ? (startIdx + itemsPerPage) : static_cast<int>(node.children.size());

        // Render immediately
        {
            const std::string& nodeName = CWstrTostr(node.name);

            if (node.isDirectory) {
                bool childVisible = false;
                if (ImGui::TreeNode(nodeName.c_str())) {
                    // Recursively render child nodes with pagination
                    for (int i = startIdx; i < endIdx; ++i) {
                        DisplayFile(node.children[i], childVisible, pagedFileSetting);
                    }
                    ImGui::TreePop();
                }
            }
            else {
                // Render file node
                if (ImGui::Selectable(nodeName.c_str())) {
                    PrintFileContent(node);
                }
            }
        }

        // Throttle updates
        float currentTime = GetCurrentTimeSeconds();
        if (currentTime - lastUpdateTime >= updateInterval) {
            lastUpdateTime = currentTime; // Update the last update time

            // Update contentHeight based on the node's visibility
            float nodeHeight = 20.0f;  // Height of each node item

            // Check if scroll position has changed
            if (currentScrollY != previousScrollY) {
                previousScrollY = currentScrollY;
                initialDisplayDone = true;
            }

            // Determine if the current node is within the viewport
            bool isNodeVisible = (currentScrollY <= contentHeight + viewportHeight) &&
                (currentScrollY + viewportHeight >= contentHeight);

            if (initialDisplayDone || isNodeVisible || currentScrollY == 0.0f) {
                if (ImGui::GetCursorPosY() + nodeHeight >= viewportHeight && !initialDisplayDone) {
                    // If the cursor is near the bottom, update contentHeight for rendering
                    contentHeight += nodeHeight;
                }
            }
            else {
                // Skip rendering for nodes not visible
                contentHeight += nodeHeight;
            }
        }
    }
    else {
        // Render without pagination
        const std::string& nodeName = CWstrTostr(node.name);

        if (node.isDirectory) {
            bool childVisible = false;
            if (ImGui::TreeNode(nodeName.c_str())) {
                // Recursively render child nodes without pagination
                for (const auto& child : node.children) {
                    DisplayFile(child, childVisible, pagedFileSetting);
                }
                ImGui::TreePop();
            }
        }
        else {
            // Render file node
            if (ImGui::Selectable(nodeName.c_str())) {
                PrintFileContent(node);
            }
        }
    }
}





void PrintFileContent(const FileNode& node) {
    std::ifstream fileStream(CWstrTostr(node.path), std::ios::binary | std::ios::ate);
    if (fileStream.is_open()) {
        std::streamsize fileSize = fileStream.tellg();
        fileStream.seekg(0, std::ios::beg);

        std::vector<char> rawBuffer(fileSize);
        if (fileStream.read(rawBuffer.data(), fileSize)) {
            bufferContent.clear();
            for (char c : rawBuffer) {
                if (c != '\0') {  // Filter out null bytes
                    bufferContent.push_back(c);
                }
            }
            bufferContent.push_back('\0');  // Add null terminator at the end

            content.assign(bufferContent.data(), bufferContent.size() - 1);
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

static float terminalHeightPercent = 0.2f;

std::string wchar_to_string(const wchar_t* wstr) {
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, &str[0], size_needed, NULL, NULL);
    return str;
}

std::string double_path_separators(const std::string& path) {
    std::string result;
    for (char ch : path) {
        if (ch == '/' || ch == '\\') {
            result += ch;
            result += ch; // Add the separator twice
        }
        else {
            result += ch;
        }
    }
    return result;
}

std::string readFileContents(const std::string& saveFilePath) {
    std::ifstream file(saveFilePath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << saveFilePath << std::endl;
        return "";
    }

    std::string contents((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());
    file.close();
    return contents;
}

std::vector<char> readFileContent(const std::string& filePath) {
    std::ifstream inFile(filePath, std::ios::binary);
    if (!inFile.is_open()) {
        std::cerr << "Error: Unable to open file " << filePath << std::endl;
        return {};
    }
    return std::vector<char>(std::istreambuf_iterator<char>(inFile), {});
}





std::string CWstrTostr(const std::wstring& wstr) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.to_bytes(wstr);
}

void DotAtCursor()
{
    ImGuiIO& io = ImGui::GetIO();


    ImVec2 mousePos = io.MousePos;


    float dotRadius = 7;


    ImVec4 dotColor = ImVec4(1.0f, 0.0f, 0.0f, 0.7f);



    ImDrawList* drawList = ImGui::GetWindowDrawList();


    drawList->AddCircleFilled(mousePos, dotRadius, ImColor(dotColor));
}

bool ret;
GLuint texture;
int texWidth, texHeight;

int my_image_width = 1;
int my_image_height = 1;
GLuint my_image_texture = 1;
float textHeight;
#include <algorithm>
ImVec2 imageSize;





void Renderbar() {
    FlushTextures();
    float windowHeight = ImGui::GetIO().DisplaySize.y;
    float windowWidth = ImGui::GetIO().DisplaySize.x;
    float terminalHeight = windowHeight * terminalHeightPercent;
    float editorHeight = windowHeight - terminalHeight - 20;
    float editorWidth = windowWidth;

    ImGui::SetNextWindowSize(ImVec2(200, windowHeight - terminalHeight - 20));
    ImGui::SetNextWindowPos(ImVec2(0, 20)); // Lock top position
    ImGui::Begin("Explorer", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

    if (PagedFileSetting) {
        // Pagination logic
        for (const auto& node : fileTree) {
            // Render files/directories with pagination
            // Calculate start and end index for current page
            int startIdx = currentPage * itemsPerPage;
            int endIdx = (startIdx + itemsPerPage < static_cast<int>(node.children.size())) ? (startIdx + itemsPerPage) : static_cast<int>(node.children.size());

            if (node.isDirectory) {
                if (ImGui::TreeNode(CWstrTostr(node.name).c_str())) {
                    // Render only items on the current page
                    for (int i = startIdx; i < endIdx; ++i) {
                        DisplayFile(node.children[i], true); // Pass true to ensure child nodes are visible
                    }
                    ImGui::TreePop();
                }
            }
        }

        // Pagination controls
        size_t totalItems = 0; // Change to size_t
        for (const auto& node : fileTree) {
            totalItems += node.children.size();
        }
        int totalPages = (totalItems + itemsPerPage - 1) / itemsPerPage;

        ImGui::Text("Page %d of %d", currentPage + 1, totalPages);
        ImGui::SameLine();

        if (ImGui::Button("Previous")) {
            if (currentPage > 0) {
                currentPage--;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Next")) {
            if (currentPage < totalPages - 1) {
                currentPage++;
            }
        }
    }
    else {
        // Render all files and directories without pagination
        for (const auto& node : fileTree) {
            DisplayFile(node, true); // Pass true to ensure all children are rendered
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

    // Ensure OpenGL context is active before this

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
            ret = LoadTextureFromFile("C:\\Users\\K754a\\source\\repos\\Project2\\Project2\\Images\\open.png", &my_image_texture, &my_image_width, &my_image_height);
            IM_ASSERT(ret);  // Ensure the texture loading succeeded
            textHeight = ImGui::GetTextLineHeight();

            // Adjust the image size to match the text height
            ImVec2 imageSizea(textHeight, textHeight);

            ImGui::Image((void*)(intptr_t)my_image_texture, imageSizea);
            ImGui::SameLine();  // Aligns the text to the right of the image
            if (ImGui::MenuItem("Open Directory")) {
                OpenFile();
            }

            ret = LoadTextureFromFile("C:\\Users\\K754a\\source\\repos\\Project2\\Project2\\Images\\save.png", &my_image_texture, &my_image_width, &my_image_height);
            IM_ASSERT(ret);  // Ensure the texture loading succeeded
            textHeight = ImGui::GetTextLineHeight();

            // Adjust the image size to match the text height
            ImVec2 imageSize(textHeight, textHeight);

            ImGui::Image((void*)(intptr_t)my_image_texture, imageSize);
            ImGui::SameLine();  // Aligns the text to the right of the image
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

            ret = LoadTextureFromFile("C:\\Users\\K754a\\source\\repos\\Project2\\Project2\\Images\\close.png", &my_image_texture, &my_image_width, &my_image_height);
            IM_ASSERT(ret);  // Ensure the texture loading succeeded
            textHeight = ImGui::GetTextLineHeight();

            // Adjust the image size to match the text height
            ImVec2 imageSizeb(textHeight, textHeight);

            ImGui::Image((void*)(intptr_t)my_image_texture, imageSizeb);

            ImGui::SameLine();  // Aligns the text to the right of the image
            if (ImGui::MenuItem("Exit")) {
                // Handle exit
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit")) {

            ret = LoadTextureFromFile("C:\\Users\\K754a\\source\\repos\\Project2\\Project2\\Images\\settings.png", &my_image_texture, &my_image_width, &my_image_height);
            IM_ASSERT(ret);  // Ensure the texture loading succeeded
            textHeight = ImGui::GetTextLineHeight();

            // Adjust the image size to match the text height
            ImVec2 imageSizeb(textHeight, textHeight);

            ImGui::Image((void*)(intptr_t)my_image_texture, imageSizeb);
            ImGui::SameLine();  // Aligns the text to the right of the image
            if (ImGui::MenuItem("Settings")) {
                Settingsrender();
            }

            ret = LoadTextureFromFile("C:\\Users\\K754a\\source\\repos\\Project2\\Project2\\Images\\call.png", &my_image_texture, &my_image_width, &my_image_height);
            IM_ASSERT(ret);  // Ensure the texture loading succeeded
            textHeight = ImGui::GetTextLineHeight();

            // Adjust the image size to match the text height
            ImVec2 imageSize(textHeight, textHeight);


            ImGui::Image((void*)(intptr_t)my_image_texture, imageSize);
            ImGui::SameLine();  // Aligns the text to the right of the image
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

       

       



        // ImageButton with action trigger
        if (ImGui::BeginMenu("Run")) {
            // Combine Image and Text in a single item
            ImGui::Image((void*)(intptr_t)my_image_texture, imageSize);
            ImGui::SameLine();  // Aligns the text to the right of the image
            if (ImGui::MenuItem("Run Script")) {
                // Action when menu item is clicked
                std::cout << "RUN" << std::endl;
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





        ImGui::EndMainMenuBar();
    }

    if (BetterMouseImage) {
        DotAtCursor();
    }

    if (settings) {
        Settingsrender();
    }

    if (winfpsread) {
        ImGui::SameLine();
        ImGui::Text("FPS: %d", fps);
    }



 

}





//this would be wayyyy more cleaner if i could just leave all the funct on the bottom, but nope :(



