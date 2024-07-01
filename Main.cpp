#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>


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
    //cords-------------------------|----------------|colors---------------|
    -0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,           0.8f, 0.3f, 0.02f,

    0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,            0.8f, 0.3f, 0.02f,
    0.0f, 0.5f * float(sqrt(3)) * 2 / 3, 0.0f,         1.0f, 0.6f, 0.32f,
    -0.5f / 2, 0.5f * float(sqrt(3)) / 6, 0.0f,        0.9f, 0.43f, 0.17f,
    0.5f / 2, 0.5f * float(sqrt(3)) / 6, 0.0f,         0.9f, 0.43f, 0.17f,
    0.0f, -0.5f * float(sqrt(3)) / 3, 0.0f,             0.8f, 0.3f, 0.02f,
};


GLuint indices[] =
{
    0, 3, 5, 
    3, 2, 4,
    5, 4, 1 
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
    VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
    VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3*sizeof(float)));
    // Unbind all to prevent accidentally modifying them
    VAO1.Unbind();
    VBO1.Unbind();
    EBO1.Unbind();



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
        // Bind the VAO so OpenGL knows to use it
        VAO1.Bind();

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


    //kill
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}