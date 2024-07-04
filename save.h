#ifndef Save_H
#define Save_H

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include <fstream>
#include <vector>
#include <string>
#include <iostream>
//needed to create new files
#include <experimental/filesystem>





	//savefile function
	void saveToFile(const std::vector<std::string>& data, const std::string& filePath);


#endif#pragma once
