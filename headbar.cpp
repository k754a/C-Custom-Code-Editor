#include "headbar.h"
#include <windows.h>
#include <shlobj.h> 
#include <iostream>
#include <string>
//make it gloabl

std::wstring save = L"";
void ListFilesRecursively(const std::wstring& directory, int depth = 0)
{
 
    std::wstring searchPath = directory + L"\\*";

    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile(searchPath.c_str(), &findFileData);

    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (wcscmp(findFileData.cFileName, L".") != 0 && wcscmp(findFileData.cFileName, L"..") != 0)
            {
                if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    // Load folder
                    std::wstring prefix(depth * 2, L' ');
                    prefix += L"        |";
                    std::wcout << prefix << std::endl;
                    save.append(prefix);

                    prefix += L"+>";
                    std::wcout << prefix << findFileData.cFileName << std::endl;
                    save.append(prefix).append(findFileData.cFileName);

                    std::wstring subdir = directory + L"\\" + findFileData.cFileName;
                  
                    ListFilesRecursively(subdir, depth + 1);
                }
                else
                {
                    // Load files
                    std::wstring prefix(depth * 2, L' ');
                    prefix += L"    |";
                    std::wcout << prefix << std::endl;
                    save.append(prefix);

                    prefix += L"+>";
                    std::wcout << prefix << findFileData.cFileName << std::endl;
                    save.append(prefix).append(findFileData.cFileName);
                }
            }
        } while (FindNextFile(hFind, &findFileData) != 0);

        FindClose(hFind);
    }
    else
    {
        std::wcerr << "Error: " << directory.c_str() << std::endl;
    }
}

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

            ListFilesRecursively(szDir);
            std::wcout << L"output:" << save << std::endl;

            std::wcout << L"Loaded!" <<std::endl;
           
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




void Renderbar()
{
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
            if (ImGui::MenuItem("Undo"))
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
            }
            if (ImGui::MenuItem("Object Edit window"))
            {
                // Handle object edit window
            }
            if (ImGui::MenuItem("Close All Windows"))
            {
                // Handle close all windows
            }
            ImGui::Separator();
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}
