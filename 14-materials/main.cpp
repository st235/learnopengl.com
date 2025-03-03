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

struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

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

    glm::vec3 cube_positions[] = {
        glm::vec3(-2.5f, 0.0f, 0.0f),
        glm::vec3(-1.25f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.25f, 0.0f, 0.0f),
        glm::vec3(-2.5f, 1.25f, 0.0f),
        glm::vec3(-1.25f, 1.25f, 0.0f),
        glm::vec3(0.0f, 1.25f, 0.0f),
        glm::vec3(1.25f, 1.25f, 0.0f),
    };

// /* emerald= */ { glm::vec3(0.0215f, 0.1745f, 0.0215f), glm::vec3(0.07568f, 0.61424f, 0.07568f), glm::vec3(0.633f, 0.727811f, 0.633f), 128.0f * 0.6f },
// /* jade= */ { glm::vec3(0.135f, 0.2225f, 0.1575f), glm::vec3(0.54f, 0.89f, 0.63f), glm::vec3(0.316228f, 0.316228f, 0.316228f), 128.0f * 0.1f },
// /* obsidian= */ { glm::vec3(0.05375f, 0.05f, 0.06625f), glm::vec3(0.18275f, 0.17f, 0.22525f), glm::vec3(0.332741f, 0.328634f, 0.346435f), 128.0f * 0.3f },
// /* pearl= */ { glm::vec3(0.25f, 0.20725f, 0.20725f), glm::vec3(1.0f, 0.829f, 0.829f), glm::vec3(0.296648f, 0.296648f, 0.296648f), 128.0f * 0.088f },
// /* ruby= */ { glm::vec3(0.1745f, 0.01175f, 0.01175f), glm::vec3(0.61424f, 0.04136f, 0.04136f), glm::vec3(0.727811f, 0.626959f, 0.626959f), 128.0f * 0.6f },
// /* turquoise= */ { glm::vec3(0.1f, 0.18725f, 0.1745f), glm::vec3(0.396f, 0.74151f, 0.69102f), glm::vec3(0.297254f, 0.30829f, 0.306678f), 128.0f * 0.1f },
// /* brass= */ { glm::vec3(0.329412f, 0.223529f, 0.027451f), glm::vec3(0.780392f, 0.568627f, 0.113725f), glm::vec3(0.992157f, 0.941176f, 0.807843f), 128.0f * 0.21794872f },
// /* bronze= */ { glm::vec3(0.2125f, 0.1275f, 0.054f), glm::vec3(0.714f, 0.4284f, 0.18144f), glm::vec3(0.393548f, 0.271906f, 0.166721f), 128.0f * 0.2f },
// /* chrome= */ { glm::vec3(0.25f, 0.25f, 0.25f), glm::vec3(0.4f, 0.4f, 0.4f), glm::vec3(0.774597f, 0.774597f, 0.774597f), 128.0f * 0.6f },
// /* copper= */ { glm::vec3(0.19125f, 0.0735f, 0.0225f), glm::vec3(0.7038f, 0.27048f, 0.0828f), glm::vec3(0.256777f, 0.137622f, 0.086014f), 128.0f * 0.1f },
// /* gold= */ { glm::vec3(0.24725f, 0.1995f, 0.0745f), glm::vec3(0.75164f, 0.60648f, 0.22648f), glm::vec3(0.628281f, 0.555802f, 0.366065f), 128.0f * 0.4f },
// /* silver= */ { glm::vec3(0.19225f, 0.19225f, 0.19225f), glm::vec3(0.50754f, 0.50754f, 0.50754f), glm::vec3(0.508273f, 0.508273f, 0.508273f), 128.0f * 0.4f },
// /* black plastic= */ { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.01f, 0.01f, 0.01f), glm::vec3(0.50f, 0.50f, 0.50f), 128.0f * 0.25f },
// /* cyan plastic= */ { glm::vec3(0.0f, 0.1f, 0.06f), glm::vec3(0.0f, 0.50980392f, 0.50980392f), glm::vec3(0.50196078f, 0.50196078f, 0.50196078f), 128.0f * 0.25f },
// /* green plastic= */ { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.1f, 0.35f, 0.1f), glm::vec3(0.45f, 0.55f, 0.45f), 128.0f * 0.25f },
// /* red plastic= */ { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.0f, 0.0f), glm::vec3(0.7f, 0.6f, 0.6f), 128.0f * 0.25f },
// /* white plastic= */ { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.55f, 0.55f, 0.55f), glm::vec3(0.70f, 0.70f, 0.70f), 128.0f * 0.25f },
// /* yellow plastic= */ { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(0.60f, 0.60f, 0.50f), 128.0f * 0.25f },
// /* black rubber= */ { glm::vec3(0.02f, 0.02f, 0.02f), glm::vec3(0.01f, 0.01f, 0.01f), glm::vec3(0.4f, 0.4f, 0.4f), 128.0f * 0.078125f },
// /* cyan rubber= */ { glm::vec3(0.0f, 0.05f, 0.05f), glm::vec3(0.4f, 0.5f, 0.5f), glm::vec3(0.04f, 0.7f, 0.7f), 128.0f * 0.078125f },
// /* green rubber= */ { glm::vec3(0.0f, 0.05f, 0.0f), glm::vec3(0.4f, 0.5f, 0.4f), glm::vec3(0.04f, 0.7f, 0.04f), 128.0f * 0.078125f },
// /* red rubber= */ { glm::vec3(0.05f, 0.0f, 0.0f), glm::vec3(0.5f, 0.4f, 0.4f), glm::vec3(0.7f, 0.04f, 0.04f), 128.0f * 0.078125f },
// /* white rubber= */ { glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.7f, 0.7f, 0.7f), 128.0f * 0.078125f },
// /* yellow rubber = */ { glm::vec3(0.05f, 0.05f, 0.0f), glm::vec3(0.5f, 0.5f, 0.4f), glm::vec3(0.7f, 0.7f, 0.04f), 128.0f * 0.078125f },

    Material cube_materials[] = {
        /* emerald= */ { glm::vec3(0.0215f, 0.1745f, 0.0215f), glm::vec3(0.07568f, 0.61424f, 0.07568f), glm::vec3(0.633f, 0.727811f, 0.633f), 128.0f * 0.6f },
        /* pearl= */ { glm::vec3(0.25f, 0.20725f, 0.20725f), glm::vec3(1.0f, 0.829f, 0.829f), glm::vec3(0.296648f, 0.296648f, 0.296648f), 128.0f * 0.088f },
        /* bronze= */ { glm::vec3(0.2125f, 0.1275f, 0.054f), glm::vec3(0.714f, 0.4284f, 0.18144f), glm::vec3(0.393548f, 0.271906f, 0.166721f), 128.0f * 0.2f },
        /* gold= */ { glm::vec3(0.24725f, 0.1995f, 0.0745f), glm::vec3(0.75164f, 0.60648f, 0.22648f), glm::vec3(0.628281f, 0.555802f, 0.366065f), 128.0f * 0.4f },
        /* cyan plastic= */ { glm::vec3(0.0f, 0.1f, 0.06f), glm::vec3(0.0f, 0.50980392f, 0.50980392f), glm::vec3(0.50196078f, 0.50196078f, 0.50196078f), 128.0f * 0.25f },
        /* red rubber= */ { glm::vec3(0.05f, 0.0f, 0.0f), glm::vec3(0.5f, 0.4f, 0.4f), glm::vec3(0.7f, 0.04f, 0.04f), 128.0f * 0.078125f },
        /* green rubber= */ { glm::vec3(0.0f, 0.05f, 0.0f), glm::vec3(0.4f, 0.5f, 0.4f), glm::vec3(0.04f, 0.7f, 0.04f), 128.0f * 0.078125f },
        /* yellow rubber = */ { glm::vec3(0.05f, 0.05f, 0.0f), glm::vec3(0.5f, 0.5f, 0.4f), glm::vec3(0.7f, 0.7f, 0.04f), 128.0f * 0.078125f },
    };

    Shader cube_shader("shader.vs", "shader.fs");
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

    glm::vec3 light_position(0.0f, 1.0f, 2.0f);

    float dt = 0.0f;
    float last_frame = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        float current_frame = static_cast<float>(glfwGetTime());
        dt = current_frame - last_frame;
        last_frame = current_frame;

        ProcessInput(dt, window, camera);
        camera.Reposition();

        // light_position.x = -4.0f * std::sinf(glfwGetTime());
        // light_position.z = -4.0f * std::cosf(glfwGetTime());

        glBindVertexArray(vertex_array);

        cube_shader.use();

        cube_shader.setVec3("light.position", light_position);
        // cube_shader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        // cube_shader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
        // cube_shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        cube_shader.setVec3("light.ambient", glm::vec3(1.0f));
        cube_shader.setVec3("light.diffuse", glm::vec3(1.0f));
        cube_shader.setVec3("light.specular", glm::vec3(1.0f));

        cube_shader.setVec3("viewPos", camera.position().x, camera.position().y, camera.position().z);

        glm::mat4 projection = glm::perspective(
            glm::radians(camera.zoom()), static_cast<float>(WINDOW_WIDTH) / WINDOW_HEIGHT, 0.1f, 100.0f);
        glUniformMatrix4fv(cubeProjectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUniformMatrix4fv(cubeViewLoc, 1, GL_FALSE, glm::value_ptr(camera.view()));

        for (size_t i = 0; i < sizeof(cube_positions) / sizeof(glm::vec3); i++) {
            const auto& cube_position = cube_positions[i];
            const auto& cube_material = cube_materials[i];


            cube_shader.setVec3("material.ambient", cube_material.ambient);
            cube_shader.setVec3("material.diffuse", cube_material.diffuse);
            cube_shader.setVec3("material.specular", cube_material.specular);
            cube_shader.setFloat("material.shininess", cube_material.shininess);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cube_position);

            glUniformMatrix4fv(cubeModelLoc, 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

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
