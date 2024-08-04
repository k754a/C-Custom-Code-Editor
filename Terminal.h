#ifndef TERMINAL_H
#define TERMINAL_H

#include <string>
#include <vector>
#include <atomic>
#include "imgui.h"

// Function declarations
void ExecuteCommand(const std::string& command);
void RenderTerminal(float windowWidth, float windowHeight, float terminalHeight);
void ExecutePythonCode(const std::string& code, const std::string& outputFileName);

// Global variables related to the terminal
extern std::atomic<bool> isPythonRunning;
extern std::vector<std::string> terminalOutput;
extern std::string pythonOutput;

extern bool pip;
extern bool runC;

#endif // TERMINAL_H
