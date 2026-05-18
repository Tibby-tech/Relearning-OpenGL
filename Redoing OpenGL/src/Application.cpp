// This is the main file that the application will be run from

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#define ASSERT(x) if (!(x)) __debugbreak();

#ifdef _DEBUG
    #define GlCall(x) GLClearError();\
        x;\
        ASSERT(GLLogCall(#x, __FILE__, __LINE__))
#else
    #define GlCall(x) x
#endif
    

static void GLClearError() {
    // While there are still errors in glGetError
    while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line) {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] (" << error << "): " << function << " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath) {
    std::ifstream stream(filepath);

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    ShaderType type = ShaderType::NONE;

    std::string line;
    std::stringstream ss[2];
    while (getline(stream, line)) {
        // The find function returns the position of the found string if it passes. If it doesn't pass, it returns npos.
        if (line.find("# shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos)
                // set mode to vertex
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                // set mode to fragment
                type = ShaderType::FRAGMENT;
        }
        else {
            ss[(int)type] << line << "\n";
        }
    }

    return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source) {
    GlCall(unsigned int id = glCreateShader(type));
    const char* src = source.c_str();
    GlCall(glShaderSource(id, 1, &src, nullptr));
    GlCall(glCompileShader(id));

    int result;
    GlCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));

    if (result == GL_FALSE) {
        int length;
        GlCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        char* message = (char*)alloca(length * sizeof(char));
        GlCall(glGetShaderInfoLog(id, length, &length, message));

        int shaderType;
        const char* shaderTypeString{};
        GlCall(glGetShaderiv(id, GL_SHADER_TYPE, &shaderType));
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
        GlCall(glDeleteShader(id));
        return 0;
    }

    return id;
}

// Because it is static, this function can only be seen from this file.
// Creates a shader with a unique id.
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    // Creates a shader program to link the vertex and fragment shaders together
    GlCall(unsigned int program = glCreateProgram());

    GlCall(unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader));
    GlCall(unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader));

    GlCall(glAttachShader(program, vs));
    GlCall(glAttachShader(program, fs));
    GlCall(glLinkProgram(program));
    GlCall(glValidateProgram(program));

    GlCall(glDeleteShader(vs));
    GlCall(glDeleteShader(fs));

    return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

    // This enables v-sync
    glfwSwapInterval(1);

    // glewInit() is here because it has to be called after a valid OpenGL rendering context is created
    if (glewInit() != GLEW_OK)
        std::cout << "Error!" << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl;

    float positions[] = {
        -0.5f, -0.5f, // 0
         0.5f, -0.5f, // 1
         0.5f,  0.5f, // 2
        -0.5f,  0.5f, // 3
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    unsigned int buffer;
    // A buffer is simply an array of bytes, or an array of data.
    // This makes a unique id for the buffer so we can tell OpenGL which buffer to draw when it's time
    GlCall(glGenBuffers(1, &buffer));
    // Binding in OpenGL is simply selecting a buffer, so this selects the specified buffer, and sets it up to be acted on in future lines of code
    // Since the buffer is simply an array of data, we use GL_ARRAY_BUFFER.
    GlCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
    // This copies the data from the positions array into the currently bound buffer in GL_ARRAY_BUFFER
    GlCall(glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), positions, GL_STATIC_DRAW));

    // glVertexAttribPointer tells OpenGL how to interpret the array buffer's vertex data when a draw call is run.
    // When we make a shader, we have to match the same data layout on this side (GPU) and the shader side (GPU).
    GlCall(glEnableVertexAttribArray(0));
    GlCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (const void*)0));

    unsigned int ibo;
    GlCall(glGenBuffers(1, &ibo));
    GlCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    GlCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * 2 * sizeof(unsigned int), indices, GL_STATIC_DRAW));

    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    // This binds the shader program so that it is used on oncoming draw calls.
    GlCall(glUseProgram(shader));

    GlCall(int location = glGetUniformLocation(shader, "u_Color"));
    ASSERT(location != -1);
    GlCall(glUniform4f(location, 0.8f, 0.3f, 0.8f, 1.0f));

    GlCall(glUseProgram(0));
    GlCall(glBindVertexArray(0));
    GlCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GlCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

    float r = 0.0f;
    float increment = 0.01f;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        GlCall(glUseProgram(shader));

        GlCall(glBindVertexArray(vao));

        GlCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));

        GlCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));
        // This issues a draw call on the currently bound array buffer
        GlCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

        if (r < 0.0f) {
            increment = 0.01f;
        }
        else if (r > 1.0f) {
            increment = -0.01f;
        }

        r += increment;

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    GlCall(glDeleteProgram(shader));

    glfwTerminate();
    return 0;
}