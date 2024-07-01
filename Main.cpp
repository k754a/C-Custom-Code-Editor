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





int main()
{
    //Init glfw
    glfwInit();

    //tell what version we are on (not sure if it effects vs code but the tutorial is in vs)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfw profile
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

    GLfloat vertices[] = {

        ///this is drawing a square

        -0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
        0.5, -0.5, 0.0f






    };

    //this basicly says where to draw each triangle, and how many to draw, where 0 corisponds to 0 of vertices, same with 1,2,3,4,5
    GLuint indices[]
    {
        0,1,2,
        2,3,0,

    };




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
    GLuint VBO;
    //open gl doesn't actly know where to find it, so we need a vertex array object 

    GLuint VAO;


    GLuint EBO;



    glGenVertexArrays(1, &VAO); //make sure its before the gen buffers and the bind buffers

    //one because we only have one object
    
    glGenBuffers(1, &EBO);


    //Find VAO to work with
    glBindVertexArray(VAO);

   

    //lets store the vertices

    //the buffer, the size of bites in the data being sent, and the use of the data (buffer, size of bytes, vertices)
    //static is writen too once and used a few times, dynamic is used many times, but changes

    //draw means the vertices will be modded
   

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //link indices array
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    //config so that open gl knows how to read the vbo

    //(pos of vertex atrobutes, values per vertex, if we have the cords as ints (we do not so its GL_FLOAT))
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);//that void is just so its 0 beacause we cant use null
    //we need to use it so lets do that
    //0 becase we have no pos for it
    glEnableVertexAttribArray(0);


    //prevent changing the VBO or VAO or EBO while it runs. Order is very important

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
















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
       
        glBindVertexArray(VAO);

        //(triangles, how many points to draw, then how many indicies (0))
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //glDrawArrays(GL_TRIANGLES, 0 , 6); replace with gldraw elements

        glfwSwapBuffers(window);





        glfwPollEvents();
    }

    //keep this clean
    glDeleteVertexArrays(1, &VAO);
   
  
    glDeleteBuffers(1, &EBO);


    //kill
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}