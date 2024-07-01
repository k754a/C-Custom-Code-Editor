#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

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
    GLFWwindow* window = glfwCreateWindow(800, 800, "TankGame", NULL, NULL);

    //error detection
    if(window == NULL)
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
        glViewport(0, 0, 800,800);

        glClearColor(0.07f, 0.12f, 9.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);

    //only make it end if the window is closed.

    while (!glfwWindowShouldClose(window)) //way more simple than python.
    {
        //this prevents the window from stoping
        
        ///do not do this! without glClear(GL_COLOR_BUFFER_BIT); causes flickering:  glfwSwapBuffers(window);

           glViewport(0, 0, 800,800);

        glClearColor(0.07f, 0.12f, 9.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);

       glfwPollEvents();
    }
    



    //kill
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}