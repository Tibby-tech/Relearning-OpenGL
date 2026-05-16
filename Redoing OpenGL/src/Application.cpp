// This is the main file that the application will be run from

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

static unsigned int CompileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);

    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);

        int shaderType;
        const char* shaderTypeString{};
        glGetShaderiv(id, GL_SHADER_TYPE, &shaderType);
        if (shaderType == GL_VERTEX_SHADER) {
            shaderTypeString = "vertex shader";
        }
        else if (shaderType == GL_FRAGMENT_SHADER) {
            shaderTypeString = "fragment shader";
        }
        else if (shaderType == GL_GEOMETRY_SHADER) {
            shaderTypeString = "geometry shader";
        }

        std::cout << "Failed to compile " << shaderTypeString << ": " << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

// Because it is static, this function can only be seen from this file.
// Creates a shader with a unique id.
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    // Creates a shader program to link the vertex and fragment shaders together
    unsigned int program = glCreateProgram();

    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

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

    std::string vertexShader =
        "#version 330 core\n"
        "\n"
        "layout (location = 0) in vec4 position;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   gl_Position = position;\n"
        "}\n";

    std::string fragmentShader =
        "#version 330 core\n"
        "\n"
        "out vec4 color;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   color = vec4(1.0, 0.0, 0.0, 1.0);\n"
        "}\n";

    unsigned int shader = CreateShader(vertexShader, fragmentShader);
    // This binds the shader program so that it is used on oncoming draw calls.
    glUseProgram(shader);

    // Unbinds the array buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        // This issues a draw call on the currently bound array buffer
        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}