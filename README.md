# My C++ Custom Code Editor

Welcome to my C++ code editor! It's a work in progress, and I'm excited to keep adding more features.

**Over 53,000 lines of code!**

## Current Features
- **Saving & Loading**
- **Integrated Terminal**
- **Python Integration**
- **PIP Support**
- **HTML Code Execution**
- **Custom Themes**
- **Developer Settings & UI Customization**

This is just the beginning—expect many more features soon!

## Documentation
For basic documentation, visit the [Editor Documentation](https://k754a.github.io/Editor%20Documentation). More details will be added soon!

## Theme's!

Themes are created in  [Style/CStyle.Style](https://github.com/k754a/C-Custom-Code-Editor/blob/main/Style/CStyle.Style), you can change many things in the file, while it is laid out, here is a table of what things do what:

| Section                        | Description                                      | Example Setting                                    |
|--------------------------------|--------------------------------------------------|---------------------------------------------------|
| [Window Corner Rounding](#window-corner-rounding) | Set the rounding of window corners                     | `style.WindowRounding = 15.0`                     |
| [Frame Rounding and Padding](#frame-rounding-and-padding) | Set the frame rounding and padding                      | `style.FrameRounding = 6.0` <br> `style.FramePadding = ImVec2(8, 4)` |
| [Window Background Color](#window-background-color) | Set the window background color to a dark gray          | `colors[1] = ImVec4(0.2, 0.2, 0.2, 1.0)`          |
| [Title Bar Background Color](#title-bar-background-color) | Set the title bar background color (active window)       | `colors[2] = ImVec4(0.3, 0.3, 0.3, 1.0)`          |
| [Scrollbar Colors](#scrollbar-colors)            | Set the scrollbar background and grab colors           | `colors[12] = ImVec4(0.3, 0.3, 0.3, 1.0)` <br> `colors[13] = ImVec4(0.5, 0.5, 0.5, 1.0)` <br> `colors[14] = ImVec4(0.6, 0.6, 0.6, 1.0)` <br> `colors[15] = ImVec4(0.7, 0.7, 0.7, 1.0)` |
| [Text Color](#text-color)                    | Set the text color                                    | `colors[16] = ImVec4(1.0, 1.0, 1.0, 1.0)`          |


## Settings

Settings are stored in [Settings.cpp](https://github.com/k754a/C-Custom-Code-Editor/blob/main/Settings.cpp), you can set custom settings to work on start, I don't have an active saver, but I am creating that now!

| Section                        | Description                                      | Example Setting                                    | 
|--------------------------------|--------------------------------------------------|---------------------------------------------------|
| [settings](#settings) | Show's setting window| `bool settings = false/true;`     |           
| [rendergraph](#rendergraph) | Shows's an cpu and ram usage graph| `bool rendergraph = false/true;`        |       
| [devmode](#devmode) | Show's up new settings, like fps reader| `bool winfpsread = false/true;`       |
| [darkMode](#darkmode) | Will be added at a later time| `bool darkMode = false/true;`  |
| [autoSave](#autoSave)            | Auto saves your doocuments| `bool autoSave = false/true`       | 
| [Better Mouse Image](#Better-Mouse-Image)                    | Renders a colored circle around the mouse|    `bool BetterMouseImage = false/true;`| 
| [CodeEditor](#CodeEditor)                    | Renders a number for each line like in vs code|    `bool CodeEditor = false/true;`|    
| [PagedFileSetting](#PagedFileSetting)                    | Changes rendering of the exploror, you can see screenshots below|    `bool PagedFileSetting = false/true;`|  








## Installation Guide

### New Installation
Download the new installer: [C Write Installer](https://github.com/k754a/C-Custom-Code-Editor/blob/main/Installer/C%20write-INSTALLER.exe). 

*Note: This installer may be flagged as a virus due to the use of PyInstaller. If you prefer, you can manually install the editor.*

### Old Installation
Follow the old installation guide [here](https://k754a.github.io/Editor%20Documentation) if you plan to contribute code or customize the editor.

## Screenshots

![Screenshot](https://raw.githubusercontent.com/k754a/C-Custom-Code-Editor/main/git%20images/Screenshot%202024-08-20%20181731.png)

<div style="display: flex; gap: 10px; align="center">
  <img src="https://raw.githubusercontent.com/k754a/C-Custom-Code-Editor/main/git%20images/Screenshot%202024-08-20%20181754.png" alt="Screenshot" width="30%">
  <img src="https://raw.githubusercontent.com/k754a/C-Custom-Code-Editor/main/git%20images/Screenshot%202024-08-28%20095949.png" alt="Screenshot" width="30%">
</div>

---

Thanks for checking it out!
