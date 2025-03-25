#ifndef SHADER_H
#define SHADER_H

#include "glm/gtc/type_ptr.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <ostream>
#include <string>

namespace MV
{

class Shader {
public:

    static Shader FACES_OUTLINES_SHADER;
    static Shader TET_CELLS_SHADER;
    static Shader FACES_SHADER;
    static Shader EDGES_SHADER;
    static Shader VERTICES_SHADER;
    static Shader PICKING_SHADER;

    // Call this one OpenGL is initialized
    static void loadGlobalShaders();

    unsigned int ID;

    Shader() : ID(0) {}

    // constructor generates the shader on the fly
    //Shader(const char* vertexPath, const char* geometryPath, const char* fragmentPath);

    //Shader(const char* vertexPath, const char* fragmentPath);

    // Compiles a shader program written in the file stored at filepath. Different types of shaders are
    // marked with #shader <type> where type = vertex | geometry | fragment
    Shader(const char* filepath);

    static void readFile(const char* filepath, std::string& vertexCode, std::string& geometryCode, std::string& fragmentCode);

    ~Shader()
    {
        glDeleteProgram(ID);
    }

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    // Transfer Ownership
    Shader& operator=(Shader&& s) noexcept
    {
        if (this != &s) {
            glDeleteProgram(ID);
            ID = s.ID;
            s.ID = 0;
        }
        return *this;
    }

    inline void use()
    {
        assert(ID);
        glUseProgram(ID);
    }

    inline void detach()
    {
        glUseProgram(0);
    }

    inline void setBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }

    inline void setInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }

    inline void setUInt(const std::string& name, unsigned int value) const
    {
        glUniform1ui(glGetUniformLocation(ID, name.c_str()), value);
    }

    inline void setFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

    inline void setVec2f(const std::string& name, float x, float y) const {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }

    template <typename Vec3fT>
    inline void setVec3f(const std::string& name, Vec3fT value) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), value[0], value[1], value[2]);
    }

    inline void setVec3f(const std::string& name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }

    template <typename Vec4fT>
    inline void setVec4f(const std::string& name, Vec4fT value) const
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), value[0], value[1], value[2], value[3]);
    }

    inline void setVec4f(const std::string& name, float x, float y, float z, float w) const {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }

    inline void setMat3x3f(const std::string& name, const glm::mat3x3& value) const {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }

    inline void setMat4x4f(const std::string& name, const glm::mat4x4& value) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }

private:
    // utility function for checking shader compilation/linking errors.
    void checkCompileErrors(unsigned int shader, std::string type);
};
}
#endif
