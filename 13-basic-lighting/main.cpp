// Ex. 1
// Right now the light source is a boring static light source that doesn’t move. Try to move
// the light source around the scene over time using either sin or cos. Watching the lighting
// change over time gives you a good understanding of Phong’s lighting model.

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
        // positions // normals // texture coords
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    };

    glm::vec3 cube_position = glm::vec3(0.0f, 0.0f, 0.0f);

    Shader cube_shader("shader_gouraud.vs", "shader_gouraud.fs");
    Shader light_shader("lighting.vs", "lighting.fs");

    // Cube.
    unsigned int vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
        reinterpret_cast<void*>(0 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
        reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Light source.
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
        reinterpret_cast<void*>(0 * sizeof(float)));
    glEnableVertexAttribArray(0);

    unsigned int cubeModelLoc = glGetUniformLocation(cube_shader.ID, "model");
    unsigned int cubeViewLoc = glGetUniformLocation(cube_shader.ID, "view");
    unsigned int cubeProjectionLoc = glGetUniformLocation(cube_shader.ID, "projection");

    unsigned int lightModelLoc = glGetUniformLocation(light_shader.ID, "model");
    unsigned int lightViewLoc = glGetUniformLocation(light_shader.ID, "view");
    unsigned int lightProjectionLoc = glGetUniformLocation(light_shader.ID, "projection");

    glm::vec3 light_position(1.2f, 0.5f, -2.0f);

    float dt = 0.0f;
    float last_frame = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        float current_frame = static_cast<float>(glfwGetTime());
        dt = current_frame - last_frame;
        last_frame = current_frame;

        ProcessInput(dt, window, camera);
        camera.Reposition();

        // light_position.z += -2.0f * std::cosf(glfwGetTime());
        light_position.x = -2.0f * std::sinf(glfwGetTime());
        light_position.z = -2.0f * std::cosf(glfwGetTime());

        glBindVertexArray(vertex_array);

        cube_shader.use();
        cube_shader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        cube_shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        cube_shader.setVec3("lightPos", light_position.x, light_position.y, light_position.z);
        cube_shader.setVec3("viewPos", camera.position().x, camera.position().y, camera.position().z);

        glm::mat4 projection = glm::perspective(
            glm::radians(camera.zoom()), static_cast<float>(WINDOW_WIDTH) / WINDOW_HEIGHT, 0.1f, 100.0f);
        glUniformMatrix4fv(cubeProjectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUniformMatrix4fv(cubeViewLoc, 1, GL_FALSE, glm::value_ptr(camera.view()));

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cube_position);

        glUniformMatrix4fv(cubeModelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Light.
        light_shader.use();
        glBindVertexArray(lightVAO);

        glm::mat4 light_model = glm::mat4(1.0f);
        light_model = glm::translate(light_model, light_position);
        light_model = glm::scale(light_model, glm::vec3(0.2f));

        glUniformMatrix4fv(lightProjectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(lightViewLoc, 1, GL_FALSE, glm::value_ptr(camera.view()));
        glUniformMatrix4fv(lightModelLoc, 1, GL_FALSE, glm::value_ptr(light_model));

        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}
