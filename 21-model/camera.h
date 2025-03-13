#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <iostream>

#include "glad.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

namespace {
constexpr float kCameraMovementSpeed = 10;
constexpr float kDefaultYaw = -90.0f;
constexpr float kDefaultPitch = 0.0f;
constexpr float kZoomMin = 1.0f;
constexpr float kZoomMax = 45.0f;
}  // namespace

class Camera {
public:
    enum class Movement {
        kForward,
        kBackward,
        kLeft,
        kRight,
    };

    Camera(const glm::vec3& position,
           const glm::vec3& world_up) :
        _is_mouse_initialised(false),
        _mouse_x(0.0f),
        _mouse_y(0.0f),
        _yaw(kDefaultYaw),
        _pitch(kDefaultPitch),
        _position(position),
        _front(),
        _world_up(world_up),
        _zoom(kZoomMax),
        _should_fly(true) {
        _front = glm::normalize(
            glm::vec3(std::cosf(glm::radians(_yaw)) * std::cosf(glm::radians(_pitch)),
            std::sinf(glm::radians(_pitch)), std::sinf(glm::radians(_yaw)) * std::cosf(glm::radians(_pitch))));
        Reposition();
    }

    inline const glm::mat4& view() const {
        return _view;
    }

    void PreProcessMovement(const Movement& movement, float dt) {
        glm::vec3 target = _front;
        if (!_should_fly) {
            target = glm::normalize(_front - glm::length(_front) * glm::dot(_front, glm::vec3(0, 1, 0)));
        }

        glm::vec3 right = glm::normalize(glm::cross(_front, _world_up));
        switch (movement) {
            case Movement::kForward: {
                _position += kCameraMovementSpeed * target * dt;
                break;
            }
            case Movement::kBackward: {
                _position -= kCameraMovementSpeed * target * dt;
                break;
            }
            case Movement::kLeft: {
                _position -= kCameraMovementSpeed * right * dt;
                break;
            }
            case Movement::kRight: {
                _position += kCameraMovementSpeed * right * dt;
                break;
            }
        }
    }

    void PreProcessMouseMove(float nx, float ny) {
        if (!_is_mouse_initialised) {
            _mouse_x = nx;
            _mouse_y = ny;
            _is_mouse_initialised = true;
        } else {
            float dx = nx - _mouse_x;
            float dy = _mouse_y - ny;

            _mouse_x = nx;
            _mouse_y = ny;

            float sensitivity = 0.1f;
            dx *= sensitivity;
            dy *= sensitivity;

            _yaw += dx;
            _pitch += dy;

            if (_pitch > 89.0f) {
                _pitch = 89.0f;
            } else if (_pitch < -89.0f) {
                _pitch = -89.0f;
            }

        _front = glm::normalize(
            glm::vec3(
                std::cosf(glm::radians(_yaw)) * std::cosf(glm::radians(_pitch)),
                std::sinf(glm::radians(_pitch)),
                std::sinf(glm::radians(_yaw)) * std::cosf(glm::radians(_pitch))
            )
        );
        }
    }

    void PreProcessZoom(float dy) {
        _zoom += dy;
        if (_zoom < kZoomMin) {
            _zoom = kZoomMin;
        } else if (_zoom > kZoomMax) {
            _zoom = kZoomMax;
        }
    }

    void Reposition() {
        glm::vec3 front = -_front;
        glm::vec3 right = glm::normalize(glm::cross(_world_up, front));
        glm::vec3 camera_up = glm::normalize(glm::cross(front, right));

        glm::mat4 view(1.0f);
        view[0][0] = right.x;
        view[1][0] = right.y;
        view[2][0] = right.z;

        view[0][1] = camera_up.x;
        view[1][1] = camera_up.y;
        view[2][1] = camera_up.z;

        view[0][2] = front.x;
        view[1][2] = front.y;
        view[2][2] = front.z;

        view[3][0] = -glm::dot(_position, right);
        view[3][1] = -glm::dot(_position, camera_up);
        view[3][2] = -glm::dot(_position, front);

        _view = view;
        // _view = glm::lookAt(_position, _position + _front, _world_up);
    }

    void set_fly(bool should_fly) {
        _should_fly = should_fly;
    }

    inline const glm::vec3& position() const {
        return _position;
    }

    inline const glm::vec3& front() const {
        return _front;
    }

    inline float zoom() const {
        return _zoom;
    }

private:
    bool _is_mouse_initialised;
    float _mouse_x;
    float _mouse_y;
    float _yaw;
    float _pitch;
    float _zoom;
    bool _should_fly;

    glm::vec3 _position;
    glm::vec3 _front;
    glm::vec3 _world_up;

    glm::mat4 _view;
};

#endif  // __CAMERA_H__
