#include "headbar.h"
#include "libraries.h"
#include "Settings.h" // Include the header file
#include <thread> // For sleep_for
#include "Python/include/Python.h"

#include <mutex>
#include <filesystem>
#include "Terminal.h"


//**I Used some copilot AI for loading html directorys**\\
// Global variables and structures
struct FileNode {
    std::wstring name;
    bool isDirectory;
    std::wstring path;
    std::vector<FileNode> children;
};

std::string currentFilePath; // C file path
int fps;
std::string fpsString;
std::vector<FileNode> fileTree;
std::wstring save = L"";
std::string content;
std::vector<char> bufferContent;

std::string CURRENT = "CURRENT";
std::atomic<int> cout(0);

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
            _mkdir("Psettings");
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


std::string CWstrTostr(const std::wstring& wstr) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter; // Only one converter here
    return converter.to_bytes(wstr);
}

void DisplayFile(const FileNode& node, bool isChildVisible) {
    static float previousScrollY = 0.0f;
    static float contentHeight = 0.0f;
    static bool initialDisplayDone = false;
    float currentScrollY = ImGui::GetScrollY();
    float viewportHeight = ImGui::GetWindowHeight();
    float maxScrollY = ImGui::GetScrollMaxY();

    if (currentScrollY != previousScrollY) {
        previousScrollY = currentScrollY;
        initialDisplayDone = true;
    }

    float nodeHeight = 20.0f;

    bool isNodeVisible = (currentScrollY <= contentHeight + nodeHeight) && (currentScrollY + viewportHeight >= contentHeight);

    if (initialDisplayDone || isNodeVisible || currentScrollY == 0.0f) {

        contentHeight += nodeHeight;

        const std::string& nodeName = CWstrTostr(node.name);

        if (node.isDirectory) {
            bool childVisible = false;
            if (ImGui::TreeNode(nodeName.c_str())) {
                for (const auto& child : node.children) {
                    DisplayFile(child, childVisible);
                }
                ImGui::TreePop();
            }
        }
        else {
            if (ImGui::Selectable(nodeName.c_str())) {
                PrintFileContent(node);
            }
        }
    }
    else {

        contentHeight += nodeHeight;
    }
}




void PrintFileContent(const FileNode& node) {
    std::ifstream fileStream(CWstrTostr(node.path), std::ios::binary | std::ios::ate);
    if (fileStream.is_open()) {
        std::streamsize fileSize = fileStream.tellg();
        fileStream.seekg(0, std::ios::beg);

        bufferContent.resize(fileSize + 1); // Increase buffer size by 1 for null terminator
        if (fileStream.read(bufferContent.data(), fileSize)) {
            bufferContent[fileSize] = '\0'; // Add null terminator at the end
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


void Renderbar() {
    float windowHeight = ImGui::GetIO().DisplaySize.y;
    float windowWidth = ImGui::GetIO().DisplaySize.x;
    float terminalHeight = windowHeight * terminalHeightPercent;
    float editorHeight = windowHeight - terminalHeight - 20;
    float editorWidth = windowWidth;

    ImGui::SetNextWindowSize(ImVec2(200, windowHeight - terminalHeight - 20));
    ImGui::SetNextWindowPos(ImVec2(0, 20)); // Lock top position
    ImGui::Begin("Explorer", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    for (const auto& node : fileTree) {
        DisplayFile(node);
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





