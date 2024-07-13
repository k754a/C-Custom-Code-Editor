#include "save.h"


namespace fs = std::experimental::filesystem;


void saveToFile(const std::vector<std::string>& data, const std::string& fileName) {
    //get current directory
    fs::path currentDir = fs::current_path();

    //saves dir is one off of the direcotry of saves.cpp
    fs::path saveDir = currentDir / "saves";

    //create the file if it doesnt exist
    if (!fs::exists(saveDir)) {
        fs::create_directory(saveDir);
    }

    //create a file in the directory
    fs::path filePath = saveDir / fileName;

    // Need to open file to write to it
    std::ofstream outFile(filePath);

    if (!outFile) {
        std::cerr << "Error: Unable to open file " << filePath << " for writing." << std::endl;
        return;
    }

    //save
    for (const auto& line : data) {
        outFile << line << std::endl; // Write each part, then a new line.
    }

    // Close the file
    outFile.close();
    std::cout << "Data successfully saved to " << filePath << std::endl;
}
