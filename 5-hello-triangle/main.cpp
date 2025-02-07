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

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 1,
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

    unsigned int vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int elements_buffer;
    glGenBuffers(1, &elements_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
        reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);

    glUseProgram(shader_program);
    glBindVertexArray(vertex_array);

    while (!glfwWindowShouldClose(window)) {
        ProcessInput(window);

        glClearColor(0.2f, 0.3f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}
