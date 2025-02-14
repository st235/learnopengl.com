// Try drawing a second container with another call to glDrawElements but place it at a
// different position using transformations only. Make sure this second container is placed at the
// top-left of the window and instead of rotating, scale it over time (using the sin function is
// useful here; note that using sin will cause the object to invert as soon as a negative scale is ap-
// plied).

#include <iostream>

#include <glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#include "shader.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

namespace {

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

unsigned int BindTexture(const std::string& path, 
    int storeProfile,
    int loadProfile) {
    int width, height, nrChannels;
    unsigned char* texture_data =
        stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (!texture_data) {
        std::cout << "Error while loading texture" << std::endl;
        std::abort();
        return -1;
    }

    unsigned int texture;
    glGenTextures(1, &texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, storeProfile, width, height, 0,
        loadProfile, GL_UNSIGNED_BYTE, texture_data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(texture_data);
    return texture;
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
        -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 1,
    };

    Shader shader("shader.vs", "shader.fs");

    // Rect.
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
        reinterpret_cast<void*>(0 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
        reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
        reinterpret_cast<void*>(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Texture.
    unsigned int texture1 = BindTexture("container.jpg", GL_RGB, GL_RGB);
    unsigned int texture2 = BindTexture("awesomeface.png", GL_RGBA, GL_RGBA);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    shader.use();
    shader.setInt("inTexture1", 0);
    shader.setInt("inTexture2", 1);
    glBindVertexArray(vertex_array);

    unsigned int inMatrixLoc = glGetUniformLocation(shader.ID, "inMat");

    while (!glfwWindowShouldClose(window)) {
        ProcessInput(window);

        glClearColor(0.2f, 0.3f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glm::mat4 transformation1 = glm::mat4(1.0f);
        transformation1 = glm::translate(transformation1, glm::vec3(0.5f, -0.5f, 0.0f));
        transformation1 = glm::rotate(transformation1, static_cast<float>(glfwGetTime()), glm::vec3(0.0, 0.0, 1.0));
        transformation1 = glm::scale(transformation1, glm::vec3(0.5, 0.5, 0.5));

        glUniformMatrix4fv(inMatrixLoc, 1, GL_FALSE, glm::value_ptr(transformation1));

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glm::mat4 transformation2 = glm::mat4(1.0f);
        transformation2 = glm::translate(transformation2, glm::vec3(-0.5f, 0.5f, 0.0f));

        float scale = static_cast<float>(std::abs(std::sin(glfwGetTime())));
        transformation2 = glm::scale(transformation2, glm::vec3(scale, scale, scale));
        glUniformMatrix4fv(inMatrixLoc, 1, GL_FALSE, glm::value_ptr(transformation2));

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}
