#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include<glad/glad.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>

std::string get_file_contents(const char* filename);

class Shader
{
	//give it a public id
	public:
		GLuint ID;
		Shader(const char* vertexFile, const char* fragmentFile);


			void Activate();
			void Delete();

};


#endif//this makes sure c++ does not open the file twice, causing errors