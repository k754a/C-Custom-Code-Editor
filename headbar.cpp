#include "headbar.h"
#include <windows.h>
#include <shlobj.h> 
#include <iostream>
#include <string>
#include <locale>
#include <codecvt>

#include <vector>
//savefile
// 
#include <fstream>
#include <direct.h> // For mkdir

//make it gloabl
//file strct
struct FileNode {
    std::wstring name;
    bool isDirectory;
    std::vector<FileNode> children;
};

//filenode
std::vector<FileNode> fileTree;


std::wstring save = L"";
void ListFilesRecursively(const std::wstring& directory, FileNode& node) {
    std::wstring searchPath = directory + L"\\*";

    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile(searchPath.c_str(), &findFileData);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (wcscmp(findFileData.cFileName, L".") != 0 && wcscmp(findFileData.cFileName, L"..") != 0) {
                FileNode child;
                child.name = findFileData.cFileName;
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
//display file


void OpenFile()
{
    BROWSEINFO bi = { 0 };
    bi.lpszTitle = L"Select a folder";
    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

    if (pidl != nullptr)
    {
        wchar_t szDir[MAX_PATH];
        if (SHGetPathFromIDList(pidl, szDir))
        {
            std::wcout << L"Selected folder: " << szDir << std::endl;
            //get path and files in the path!
            FileNode root;
            root.name = szDir;
            root.isDirectory = true;
            ListFilesRecursively(szDir, root);
            fileTree.push_back(root);
            std::wcout << L"output:" << save << std::endl;

            std::wcout << L"Loaded!" <<std::endl;

            std::string savefolder = "Psettings";
            std::string fileP = savefolder + "/CfilePath.FUNCT";
            //make a folder
            _mkdir("Psettings");//this is not what im spost to do, but oops lol
            //create save file
            std::ofstream Savefile(fileP);
            //write it it
            
           std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
           std::string narrow_string = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(save);

            Savefile << narrow_string;

            Savefile.close();
           
        }
        else
        {
            std::wcerr << "Error getting folder path" << std::endl;
        }

        CoTaskMemFree(pidl);
    }
    else
    {
        std::wcerr << "Folder selection canceled" << std::endl;
    }
}

//this is so much simpler than copying and pasting
std::string CWstrTostr(const std::wstring& wstr) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(wstr);
}

void DisplayFile(const FileNode& node) {
    std::string nodeName = CWstrTostr(node.name);
    if (node.isDirectory) {
        if (ImGui::TreeNode(nodeName.c_str())) {
            for (const auto& child : node.children) {
                DisplayFile(child);
            }
            ImGui::TreePop();
        }
    }
    else {
        ImGui::Text("%s", nodeName.c_str());
    }
}

auto settings = false;//auto sets it to a bool
static float terminalHeightPercent = 0.2f;
void Renderbar()
{
    float windowHeight = ImGui::GetIO().DisplaySize.y;
    float windowWidth = ImGui::GetIO().DisplaySize.x;
    
    ///fixed Explorer not resizing to terminal 
    float terminalHeight = windowHeight * terminalHeightPercent;
    ///load main 
    ImGui::SetNextWindowSize(ImVec2(200, windowHeight - terminalHeight - 20));
    ImGui::SetNextWindowPos(ImVec2(0, 20)); // Lock top position
    ImGui::Begin("Explorer", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

    //prevent errors with no files
    for (const auto& node : fileTree) {
        DisplayFile(node);
    }
    ImGui::End();

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            ImGui::Separator();
            if (ImGui::MenuItem("Open Directory"))
            {
               
                OpenFile();

                
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit"))
            {
                // Handle exit
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Settings"))
            {
               
                settings = true;
            }
            /*if (ImGui::MenuItem("Undo"))
            {
                // Handle undo
            }
            if (ImGui::MenuItem("Redo"))
            {
                // Handle redo
            }
            if (ImGui::MenuItem("New"))
            {
                // Handle new
            }*/
           
            if (ImGui::MenuItem("Close All Windows"))
            {
                // Handle close all windows
            }
            ImGui::Separator();
            ImGui::EndMenu();
        }

        //this is for the settings window
        if (settings)
        {
          
            //quality of life will make it so you can turn it off

         
            ImVec4 bgcolor = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
            bgcolor.w = 0.5f; // color value

            // set the window transparnet
            ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = bgcolor;

            //window with a close button: 
            // 
            //thanks stack! https://shorturl.at/dIPMf
            if (ImGui::Begin("Settings", &settings,  ImGuiWindowFlags_NoCollapse))
            {

                if (ImGui::BeginTabBar("SettingsTabBar"))
                {
                    // Create the first tab
                    if (ImGui::BeginTabItem("Editor"))
                    {
                        ImGui::Text("NULL");
                        ImGui::EndTabItem();
                    }

                    // Create the second tab
                    if (ImGui::BeginTabItem("Project"))
                    {
                        ImGui::Text("NULL");
                        ImGui::EndTabItem();
                    }

                    if (ImGui::BeginTabItem("Debug"))
                    {
                        ImGui::Text("NULL");
                        ImGui::EndTabItem();
                    }

                    if (ImGui::BeginTabItem("Documentation"))
                    {
                        ImGui::Text("NULL");
                        ImGui::EndTabItem();
                    }

                    // End the tab bar
                    ImGui::EndTabBar();
                }
               
            }

          
            ImGui::End();
        }
        ImGui::EndMainMenuBar();
    }
}




//this would be wayyyy more cleaner if i could just leave all the funct on the bottom, but nope :(



