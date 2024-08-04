#include "Terminal.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>

std::atomic<bool> isPythonRunning(false);
std::vector<std::string> terminalOutput;
std::string pythonOutput;  // Definition of pythonOutput

bool pip = false;
bool runC = true;

static char inputBuffer[256] = "";

// Function implementations
void ExecutePythonCode(const std::string& code, const std::string& outputFileName) {
    const std::string tempFileName = "temp_script.py";
    std::ofstream tempFile(tempFileName);
    if (!tempFile.is_open()) {
        std::cerr << "Failed to open temporary Python file" << std::endl;
        return;
    }
    tempFile << code;
    tempFile.close();

    const std::string command = "python " + tempFileName + " > " + outputFileName + " 2>&1";

    int result = std::system(command.c_str());
    if (result != 0) {
        std::cerr << "Python script execution failed with error code " << result << std::endl;
    }

    terminalOutput.push_back("");
    std::ifstream outputFile(outputFileName);
    if (outputFile.is_open()) {
        std::string line;
        while (std::getline(outputFile, line)) {
            std::cout << line << std::endl;
            terminalOutput.push_back(line);
        }
        outputFile.close();
    }
    else {
        std::cerr << "Failed to open output file" << std::endl;
    }

    terminalOutput.push_back("");
    if (std::remove(tempFileName.c_str()) != 0) {
        std::cerr << "Failed to remove temporary file" << std::endl;
    }
}

void ExecuteCommand(const std::string& command) {
    if (command == "clear") {
        terminalOutput.clear();
    }
    else if (command.rfind("python ", 0) == 0) {
        std::string pythonCommand = command.substr(7);
        terminalOutput.push_back("> " + command);
        ExecutePythonCode(pythonCommand, "output.txt");
        terminalOutput.push_back("> " + pythonOutput);
    }
    else if (command.rfind("Rpip", 0) == 0) {
        if (!pip) {
            terminalOutput.push_back("Loaded " + command);
            pip = true;
        }
        else {
            terminalOutput.push_back("UnLoaded " + command);
            pip = false;
        }
    }
    else if (command.rfind("Kill", 0) == 0) {
        runC = false;
    }
    else if (command.rfind("run", 0) == 0) {
        std::string pythonCommand = command.substr(4);
        if (pythonCommand.length() < 1) {
            terminalOutput.push_back("Not valid " + command);
        }
        else {
            terminalOutput.push_back("> " + command);

            std::ifstream file(pythonCommand);
            if (file.is_open()) {
                std::stringstream buffer;
                buffer << file.rdbuf();
                std::string scriptContent = buffer.str();
                runC = true;
                ExecutePythonCode(scriptContent, "output.txt");
            }
            else {
                std::cerr << "Failed to open file: " + pythonCommand << std::endl;
            }
        }
    }
    else {
        if (pip) {
            std::string python_path = ".\\Python\\python.exe";
            std::string temp_file = "output.txt";
            std::string full_command = "\"" + python_path + "\" -m " + command + " > " + temp_file + " 2>&1";

            int result = std::system(full_command.c_str());

            if (result != 0) {
                std::cerr << "ERROR: COMMAND UNKNOWN, error 104, code: " << result << std::endl;
                terminalOutput.push_back("ERROR: COMMAND UNKNOWN, error 104");
                return;
            }

            std::ifstream file(temp_file);
            if (file.is_open()) {
                std::ostringstream oss;
                oss << file.rdbuf();
                std::string out = oss.str();
                terminalOutput.push_back(out);
                file.close();
            }
            else {
                std::cerr << "Failed to open output file" << std::endl;
            }
        }
        else {
            terminalOutput.push_back("> " + command);
            terminalOutput.push_back("ERROR: COMMAND UNKNOWN, error 104");
        }
    }
}

void RenderTerminal(float windowWidth, float windowHeight, float terminalHeight) {
    ImGui::SetNextWindowSize(ImVec2(windowWidth, terminalHeight));
    ImGui::SetNextWindowPos(ImVec2(0, windowHeight - terminalHeight));
    ImGui::Begin("Terminal", nullptr, ImGuiWindowFlags_NoCollapse);

    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetTextLineHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);
    for (const auto& line : terminalOutput) {
        ImGui::TextUnformatted(line.c_str());
    }
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }
    ImGui::EndChild();

    if (ImGui::InputText("Input", inputBuffer, sizeof(inputBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
        std::string command(inputBuffer);
        ExecuteCommand(command);
        inputBuffer[0] = '\0'; // Clear the input buffer
    }

    ImGui::End();
}
