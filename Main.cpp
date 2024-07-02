#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

//lets use stb
#include<stb/stb_image.h>

//custom texture class
#include"Texture.h"

//My notes are not very good. If you want more, I would suggest checking out the documentation. It's extensive but thorough: https://registry.khronos.org/OpenGL-Refpages/gl4/
//I learned from https://www.youtube.com/watch?v=XpBGwZNyUh0&list=PLPaoO-vpZnumdcb4tZc4x5Q-v7CkrQ6M-&index=1, very good lessons.
//remember cmath lol
#include <cmath>

//open gl does not provide us with defalts for vertex fragment shaders, so i need to write my own

///------------was moved to shaders/ default.vert & default.frag
//Fragment Shader source code


#include "shaderclass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"



//we use glfloat instead of std::float because glfloat does not change per system or across devices.
    //unlike c# for a float[] you use an = sign

    //for this array ever 3 values will repersent one cord, so like 1, 1, -1, is one and 0.5, -0.32, 0.1 are ones too

    //cords system is normiliszed so that the y lowermost is -1 and highest is 1, same with x

    //screen//
    ///////////////////////////
    //      1
    //      ^
    //      |
    //-1 -- + -- 1 (X)
    //      |
    //      v
    //     -1
    //     (Y)



GLfloat vertices[] =
{
    //cords-------------------------|----------------colors---------------|------------texturemap----------|
    -0.5f, -0.5f , 0.0f,                           1.0f, 0.0f, 0.0f,          0.0f, 0.0f,

    -0.5f, 0.5f, 0.0f,                             0.0f, 1.0f, 0.0f,          0.0f, 1.0f,
    0.5f, 0.5f , 0.0f,                             0.0f, 0.0f, 1.0f,          1.0f, 1.0f,
    0.5f , -0.5f , 0.0f,                           1.0f, 1.0f, 1.0f,          1.0f, 0.0f,

};




GLuint indices[] =
{
    0,2,1,
    0,3,2  
};

int main()
{
    //Init glfw
    glfwInit();

    //tell what version we are on (not sure if it effects vs code but the tutorial is in vs)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfw profile
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    

    


    //create the window :)
    //takes in 5 values high, width, name, fullscree y/n, not very important rn;
    GLFWwindow* window = glfwCreateWindow(800, 800, "PixeLite", NULL, NULL);

    //error detection
    if (window == NULL)
    {
        std::cout << "Error Did not make window" << std::endl;
        glfwTerminate();
        return 0;
    }

    //tell it if we want to use the window
    glfwMakeContextCurrent(window);


    //opengl
    gladLoadGL();
       

    //__________________NOTE______________________\\
    For textures, the best optimized ones are powers of 2 so like 1 x 1 is better than 1 x 4
    //1x1
    //2x2
    //4x4
    //8x8
    //16x16
    //32x32
    //64x64
    //128x128...
    
    





    //area we want gl to render :) from bottom to top
    glViewport(0, 0, 800, 800);

    

    //we want to create a buffer, a buffer is like a batch of tasks sent from the gpu to the cpu, that is slow so thats why we send it in a big batch.

    //vertex buffer object
    Shader shaderProgram("default.vert", "default.frag");
    VAO VAO1;
    VAO1.Bind();

    VBO VBO1(vertices, sizeof(vertices));
    // Generates Element Buffer Object and links it to indices
    EBO EBO1(indices, sizeof(indices));

    // Links VBO to VAO
    VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
    VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3*sizeof(float)));
    VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    // Unbind all to prevent accidentally modifying them
    VAO1.Unbind();
    VBO1.Unbind();
    EBO1.Unbind();

    //Coding Uniforms
    GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");


    Texture texture("example.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    texture.texUnit(shaderProgram, "tex0", 0);













    glClearColor(0.9f, 0.12f, 1.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);

    //only make it end if the window is closed.

    while (!glfwWindowShouldClose(window)) //way more simple than python.
    {
        //this prevents the window from stoping

        ///do not do this without glClear(GL_COLOR_BUFFER_BIT); this causes flickering:  glfwSwapBuffers(window);

        glViewport(0, 0, 800, 800);

        glClearColor(0.05f, 0.05f, 0.2f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT);



        //draw
       
        shaderProgram.Activate();
        //lets use 1f cause we only have one float
        //this changes sizes in the game, so like -0.5 makes it half smaller, 0 is normal, and 0.5 is 0.5 * larger
        glUniform1f(uniID, 0);

        //give object a texture:
        //glBindTexture(GL_TEXTURE_2D, texture);

        // Bind the VAO so OpenGL knows to use it
        VAO1.Bind();
        texture.Bind();
   

        //number of indices
        size_t numElements = sizeof(indices) / sizeof(indices[0]);
        //(triangles, how many points to draw, then how many indicies (0))
        glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_INT, 0);
        //glDrawArrays(GL_TRIANGLES, 0 , 6); replace with gldraw elements

        glfwSwapBuffers(window);





        glfwPollEvents();
    }

    //keep this clean
    VAO1.Delete();
    VBO1.Delete();
    EBO1.Delete();
    shaderProgram.Delete();
    texture.Delete();

    //kill
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}