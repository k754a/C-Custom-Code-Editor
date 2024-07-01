#include "shaderclass.h"

//create a sciprt to get the defalt frag and vert, it outputs a string with everything in the textfile
std::string get_file_contents(const char* filename)
{
	std::ifstream in(filename, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}

Shader::Shader(const char* vertexFile, const char* fragmentFile)
{
	std::string vertexCode = get_file_contents(vertexFile);
	std::string fragmentCode = get_file_contents(fragmentFile);

	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();


    //shaders
    ///////////////////////////////////////////////////////////////////////////////////


        //create a refrance to store our vertex shader, (line 10)
        //open gl version of a positive int
    GLuint VertexShader = glCreateShader(GL_VERTEX_SHADER);
    //1 is one screen
    glShaderSource(VertexShader, 1, &vertexSource, NULL);

    //gpu cannot understand, we need to compile it into machine code.
    glCompileShader(VertexShader);

    //same for fragment shader

    GLuint FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    //1 is one screen
    glShaderSource(FragmentShader, 1, &fragmentSource, NULL);

    //gpu cannot understand, we need to compile it into machine code.
    glCompileShader(FragmentShader);



    //we need to wrap up into a shader program
    ID = glCreateProgram();

    //attach the shader to the refrance, (Refrace, shader);
    //attach both vertex and fragment
    glAttachShader(ID, VertexShader);
    glAttachShader(ID, FragmentShader);


    //wrap up shader program
    //link to gpu
    glLinkProgram(ID);

    //there already in program, destroy to prevent issues.
    glDeleteShader(VertexShader);
    glDeleteShader(FragmentShader);

    ///////////////////////////////////////////////////////////////////////////////////

}

void Shader::Activate()
{
    glUseProgram(ID);
}
void Shader::Delete()
{
    glDeleteProgram(ID);
}
