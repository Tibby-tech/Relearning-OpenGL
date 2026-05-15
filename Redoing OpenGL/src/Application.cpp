// This is the main file that the application will be run from

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current
    This line makes the OpenGL code operate in the specified window*/
    glfwMakeContextCurrent(window);

    // glewInit() is here because it has to be called after a valid OpenGL rendering context is created
    if (glewInit() != GLEW_OK)
        std::cout << "Error!" << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl;

    float positions[6] = {
        -0.5f, -0.5f,
         0.0f,  0.5f,
         0.5f, -0.5f,
    };

    unsigned int buffer;
    // A buffer is simply an array of bytes, or an array of data.
    // This makes a unique id for the buffer so we can tell OpenGL which buffer to draw when it's time
    glGenBuffers(1, &buffer);
    // Binding in OpenGL is simply selecting a buffer, so this selects the specified buffer, and sets it up to be acted on in future lines of code
    // Since the buffer is simply an array of data, we use GL_ARRAY_BUFFER.
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    // This copies the data from the positions array into the currently bound buffer in GL_ARRAY_BUFFER
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

    // glVertexAttribPointer tells OpenGL how to interpret the array buffer's vertex data when a draw call is run.
    // When we make a shader, we have to match the same data layout on this side (GPU) and the shader side (GPU).
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (const void*)0);

    // Unbinds the 
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}