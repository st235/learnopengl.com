#ifndef __SHADER_H__
#define __SHADER_H__

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glad.h>

class Shader {
public:
    unsigned int ID;

    Shader(const char* vertex_shader_path,
           const char* fragment_shader_path) {
        std::string vertex_code;
        std::string fragment_code;

        std::ifstream v_shader_file;
        v_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        std::ifstream f_shader_file;
        f_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try {
            v_shader_file.open(vertex_shader_path);
            f_shader_file.open(fragment_shader_path);
            std::stringstream v_shader_stream;
            std::stringstream f_shader_stream;
            v_shader_stream << v_shader_file.rdbuf();
            f_shader_stream << f_shader_file.rdbuf();
            v_shader_file.close();
            f_shader_file.close();
            vertex_code = v_shader_stream.str();
            fragment_code = f_shader_stream.str();
        } catch (std::ifstream::failure e) {
            std::cout << "Failed to load a file." << std::endl;
            std::abort();
        }

        const char* vc_ptr = vertex_code.c_str();
        const char* fc_ptr = fragment_code.c_str();

        unsigned int vid;
        unsigned int fid;

        int status;
        char infoLog[512];

        vid = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vid, 1, &vc_ptr, nullptr);
        glCompileShader(vid);
        glGetShaderiv(vid, GL_COMPILE_STATUS, &status);
        if (!status) {
            glGetShaderInfoLog(vid, 512, nullptr, infoLog);
            std::cout << "Vertex shader compilation error: " << infoLog << std::endl;
            std::abort();
        }

        fid = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fid, 1, &fc_ptr, nullptr);
        glCompileShader(fid);
        glGetShaderiv(fid, GL_COMPILE_STATUS, &status);
        if (!status) {
            glGetShaderInfoLog(vid, 512, nullptr, infoLog);
            std::cout << "Fragment shader compilation error: " << infoLog << std::endl;
            std::abort();
        }

        ID = glCreateProgram();
        glAttachShader(ID, vid);
        glAttachShader(ID, fid);
        glLinkProgram(ID);
        glGetProgramiv(ID, GL_LINK_STATUS, &status);
        if (!status) {
            glGetProgramInfoLog(ID, 512, nullptr, infoLog);
            std::cout << "Shader program linkage error: " << infoLog << std::endl;
            std::abort();
        }

        glDeleteShader(vid);
        glDeleteShader(fid);
    }

    void use() {
        glUseProgram(ID);
    }

    void setBool(const std::string& name, bool value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), static_cast<int>(value));
    }

    void setInt(const std::string& name, int value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }

    void setFloat(const std::string& name, float value) const {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

    void setVec3(const std::string& name, const glm::vec3& vec) const {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), vec.x, vec.y, vec.z);
    }

    void setVec3(const std::string& name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }

    void setMat4(const std::string& name, const glm::mat4& mat) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }
};

#endif  // __SHADER_H__
