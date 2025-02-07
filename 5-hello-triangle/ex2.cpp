// Now create the same 2 triangles using two different VAOs and VBOs for their data.

#include <iostream>

#include <glad.h>
#include <GLFW/glfw3.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

namespace {

const char* kVertexShader = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

void main() {
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
)";

const char* kFragmentShader = R"(
#version 330 core
out vec4 FragColor;

void main() {
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
)";

void framebuffer_size_callback(GLFWwindow* window,
                               int width,
                               int height) {
    glViewport(0, 0, width, height);
}

void ProcessInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

bool CheckShaderCompilation(unsigned int shader) {
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cout << "[Shader Compilation Failed] shader: "
            << shader << " " << infoLog << std::endl;
        return false;
    }
    return true;
}

bool CheckProgramCompilation(unsigned int program) {
    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cout << "[Program Linkage Failed] program: "
            << program << " " << infoLog << std::endl;
        return false;
    }
    return true;
}

}  // namespace

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cout << "Failed to initialised GLAD" << std::endl;
        return -1;
    }

    // MacOS specific workaround.
    int initial_framebuffer_width, initial_framebuffer_height;
    glfwGetFramebufferSize(window,
                           &initial_framebuffer_width,
                           &initial_framebuffer_height);
    glViewport(0, 0, initial_framebuffer_width, initial_framebuffer_height);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Wireframe mode.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    float triangle1[] = {
        // Triangle 1.
        -0.5f, -0.5f, 0.0f,
        0.0f, -0.5f, 0.0f,
        -0.25f, 0.0f, 0.0f,
    };

    float triangle2[] = {
        // Triangle 2.
        0.0f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
        0.25f, 0.0f, 0.0f,
    };

    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &kVertexShader, nullptr);
    glCompileShader(vertex_shader);

    if (!CheckShaderCompilation(vertex_shader)) {
        std::abort();
        return 1;
    }

    unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &kFragmentShader, nullptr);
    glCompileShader(fragment_shader);

    if (!CheckShaderCompilation(fragment_shader)) {
        std::abort();
        return 1;
    }

    unsigned int shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    if (!CheckProgramCompilation(shader_program)) {
        std::abort();
        return 1;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    unsigned int vao_triangle1;
    glGenVertexArrays(1, &vao_triangle1);
    glBindVertexArray(vao_triangle1);

    unsigned int vb_triangle1;
    glGenBuffers(1, &vb_triangle1);
    glBindBuffer(GL_ARRAY_BUFFER, vb_triangle1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle1), triangle1, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
        reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);

    unsigned int vao_triangle2;
    glGenVertexArrays(1, &vao_triangle2);
    glBindVertexArray(vao_triangle2);

    unsigned int vb_triangle2;
    glGenBuffers(1, &vb_triangle2);
    glBindBuffer(GL_ARRAY_BUFFER, vb_triangle2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle2), triangle2, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
        reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);

    glUseProgram(shader_program);
    while (!glfwWindowShouldClose(window)) {
        ProcessInput(window);

        glClearColor(0.2f, 0.3f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(vao_triangle1);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(vao_triangle2);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}
