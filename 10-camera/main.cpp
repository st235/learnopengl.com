// Try to make every 3rd container (including the 1st) rotate over time, while leaving the other
// containers static using just the model matrix.

#include <iostream>

#include <glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#include "camera.h"
#include "shader.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

namespace {

static Camera camera(
    /* position= */ glm::vec3(0.0f, 0.0f, 3.0f),
    /* up= */ glm::vec3(0.0f, 1.0f, 0.0f)
);

void framebuffer_size_callback(GLFWwindow* window,
                               int width,
                               int height) {
    glViewport(0, 0, width, height);
}

void mouse_move_callback(GLFWwindow* window, double x, double y) {
    camera.PreProcessMouseMove(x, y);
    camera.Reposition();
}

void mouse_scroll_callback(GLFWwindow* window, double dx, double dy) {
    camera.PreProcessZoom(static_cast<float>(dy));
}

void ProcessInput( float dt, GLFWwindow* window, Camera& camera) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.PreProcessMovement(Camera::Movement::kForward, dt);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.PreProcessMovement(Camera::Movement::kLeft, dt);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.PreProcessMovement(Camera::Movement::kBackward, dt);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.PreProcessMovement(Camera::Movement::kRight, dt);
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_move_callback);
    glfwSetScrollCallback(window, mouse_scroll_callback);

    glEnable(GL_DEPTH_TEST);

    // Wireframe mode.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    float vertices[] = {
        // bl
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        // br
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        // tl
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        // tl
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        // tr
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        // br
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,

        // bl
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        // br
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        // tl
        -0.5f, -0.5f, 0.5f, 0.0f, 1.0f,
        // tl
        -0.5f, -0.5f, 0.5f, 0.0f, 1.0f,
        // tr
        0.5f, -0.5f, 0.5f, 1.0f, 1.0f,
        // br
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,

        // bl
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        // br
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        // tl
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        // tl
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        // tr
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        // br
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,

        // bl
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
        // br
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        // tl
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        // tl
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        // tr
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        // br
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        // bl
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        // br
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        // tl
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        // tl
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        // tr
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        // br
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f,

        // bl
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        // br
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        // tl
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        // tl
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        // tr
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        // br
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
    };

    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3( 2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f, 2.0f, -2.5f),
        glm::vec3( 1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f),
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
        reinterpret_cast<void*>(0 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
        reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

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

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view , glm::vec3(0.0f, 0.0f, -5.0f));

    unsigned int modelLoc = glGetUniformLocation(shader.ID, "model");
    unsigned int viewLoc = glGetUniformLocation(shader.ID, "view");
    unsigned int projectionLoc = glGetUniformLocation(shader.ID, "projection");

    float dt = 0.0f;
    float last_frame = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        float current_frame = static_cast<float>(glfwGetTime());
        dt = current_frame - last_frame;
        last_frame = current_frame;

        ProcessInput(dt, window, camera);
        camera.Reposition();

        glm::mat4 projection = glm::perspective(
            glm::radians(camera.zoom()), static_cast<float>(WINDOW_WIDTH) / WINDOW_HEIGHT, 0.1f, 100.0f);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glClearColor(0.2f, 0.3f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera.view()));

        for (size_t i = 0; i < sizeof(cubePositions) / sizeof(glm::vec3); i++) {
            const auto& cube = cubePositions[i];
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cube);
            model = glm::rotate(model, i * glm::radians(20.0f), glm::vec3(1.0f, 0.3f, 0.5f));

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}
