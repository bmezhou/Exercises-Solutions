#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <glad/gl.h>
#include <glad_gl.c>
#include "linmath.h"

#include <GLFW/glfw3.h>


#include "shader_s.h"

#define SHADBG 0
#define USEPBO 1

int width, height;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

void dataUpdateFunc(unsigned char * imgDat, int width, int height, int nrChannels, int frameIdx);

#if SHADBG
static void GLAPIENTRY MessageCallback( GLenum source, 
    GLenum type, 
    GLuint id, 
    GLenum severity,
    GLsizei length, 
    GLchar const * message,
    void const * usrParam);
#endif

int main()
{
    /* printf("Hello World\n"); */
    clock_t start, end;
    double timeElapsed = 0;

    GLFWwindow* window;

    /* Init GLFW */
    if( !glfwInit() )
        exit( EXIT_FAILURE );
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow( 640, 480, "Texture test", NULL, NULL );
    if (!window)
    {
        glfwTerminate();
        exit( EXIT_FAILURE );
    }

    // glfwSetWindowAspectRatio(window, 1, 1);

    // glfwSetFramebufferSizeCallback(window, reshape);
    // glfwSetKeyCallback(window, key_callback);
    // glfwSetMouseButtonCallback(window, mouse_button_callback);
    // glfwSetCursorPosCallback(window, cursor_position_callback);

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    /* GLEW init... */
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval( 1 );

    glfwGetFramebufferSize(window, &width, &height);

#if SHADBG
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
#endif

    /* Compile shader */
    Shader ourShader("texture.vs", "texture.fs");
    /* Setup vertex data */

    float vertices[] = {
        /* Position   |  Texture coord  |  Vertex color */
        /* First triangle */
        1.00f,  1.00f,   1.0f, 1.0f,       // Top right
        1.00f, -1.00f,   1.0f, 0.0f,       // Bottom right
       -1.00f, -1.00f,   0.0f, 0.0f,       // Bottom left
        /* Second triangle */
        1.00f,  1.00f,   1.0f, 1.0f,       // Top right
       -1.00f, -1.00f,   0.0f, 0.0f,       // Bottom left
       -1.00f,  1.00f,   0.0f, 1.0f,       // Top left
    };
    
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer( GL_ARRAY_BUFFER, VBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW );

    /* Position attribute */
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    int width, height, nrChannels;

    width  = 640;
    height = 480;
    nrChannels = 3;

    unsigned char* imgDat = (unsigned char*)malloc(sizeof(unsigned char) 
        * width * height * nrChannels);

    GLuint texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
        GL_UNSIGNED_BYTE, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    // Generate pixel buffer
    GLuint PBO;

    glGenBuffers(1, &PBO);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PBO);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, width * height * nrChannels, 0, GL_STREAM_DRAW);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    printf("Main render loop\n");
    // reshape(window, width, height);
    int frameIdx = 0;

    float tmpF = 0;
    float tmpI = 0;

    float inc  = 1;

    for (; !glfwWindowShouldClose(window); )
    {

        processInput(window);

        tmpF = tmpI / 255.0;
        tmpI += inc;
        
        if (tmpI >= 255)
        {
            inc = -1;
        }
        else if (tmpI <= 0)
        {
            inc = 1;
        }

        glClearColor(tmpF, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        start = clock();

        glBindTexture(GL_TEXTURE_2D, texture);

    
        /* Swap buffers */
#if USEPBO
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PBO);
        glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, 0 );
#else
        glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)imgDat );
#endif        
        /* Use shader */
        ourShader.use();

        glBindVertexArray(VAO);
        glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();

        frameIdx++;

#if USEPBO
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PBO);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, width * height * nrChannels, 0, GL_STREAM_DRAW);

        GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);

        if (ptr){
            dataUpdateFunc(ptr, width, height, nrChannels, frameIdx);
            glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
        }
#else
        dataUpdateFunc(imgDat, width, height, nrChannels, frameIdx);
#endif

        end = clock();
        
        /* clock_t is not correct... */
        timeElapsed += (double) (end - start)/CLOCKS_PER_SEC;
        
        if (frameIdx % 10 == 0){
            printf("Frame rate: %f fps; ", 10.0f/timeElapsed);
            printf("Total number of frames %d\n", frameIdx);
            timeElapsed = 0;
        }    
    }  
    
    glfwTerminate();
    exit( EXIT_SUCCESS );

    return 0;
}


/* */
void dataUpdateFunc(unsigned char * imgDat, int width, int height, int nrChannels, int frameIdx)
{
    int i, j, k;

    for (i = 0; i < height; i++){
        for (j = 0; j < width; j++){
            for (k = 0; k < nrChannels; k++){
                imgDat[i * width * nrChannels + j * nrChannels + k] = 
                    0 % 255 + 0 % 255 + 0 % 255 + (frameIdx ) % 255;
                    /* frameIdx % 255 + j % 255 + i % 255 + (k * 80) % 255; */
            }
        }
    }

    return;
}


/* Process all input events */
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);

    return;
}

/* */
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);

    return;
}

/* */
#if SHADBG
static void GLAPIENTRY MessageCallback( GLenum source, 
    GLenum type, 
    GLuint id, 
    GLenum severity,
    GLsizei length, 
    GLchar const * message,
    void const * usrParam)
{
    fprintf(stderr, "GL_CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
    (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
    type, severity, message);
}
#endif