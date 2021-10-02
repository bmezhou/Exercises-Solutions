#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <glad/gl.h>
#include <glad_gl.c>

#include <GLFW/glfw3.h>

#include <linmath.h>

int width, height;

void main()
{
    printf("Hello World\n");

    GLFWwindow* window;

    /* Init GLFW */
    if( !glfwInit() )
        exit( EXIT_FAILURE );

    window = glfwCreateWindow( 400, 400, "Boing (classic Amiga demo)", NULL, NULL );
    if (!window)
    {
        glfwTerminate();
        exit( EXIT_FAILURE );
    }

    glfwSetWindowAspectRatio(window, 1, 1);

    // glfwSetFramebufferSizeCallback(window, reshape);
    // glfwSetKeyCallback(window, key_callback);
    // glfwSetMouseButtonCallback(window, mouse_button_callback);
    // glfwSetCursorPosCallback(window, cursor_position_callback);

    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval( 1 );

    glfwGetFramebufferSize(window, &width, &height);
    // reshape(window, width, height);
    for (; ;)
    {
        /* Swap buffers */
       glfwSwapBuffers(window);
       glfwPollEvents();

       /* Check if we are still running */
       if (glfwWindowShouldClose(window))
           break;       
    }  
    
    glfwTerminate();
    exit( EXIT_SUCCESS );

    return;
}