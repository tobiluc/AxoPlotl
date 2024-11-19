#ifndef SHADER_H
#define SHADER_H

#include "../utils/gl.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
    unsigned int ID;

    // constructor generates the shader on the fly
    Shader(const char* vertexPath, const char* fragmentPath);

    // activate the shader
    void use();

    // upload bool uniform
    void setBool(const std::string& name, bool value) const;

    // upload int uniform
    void setInt(const std::string& name, int value) const;

    // upload float uniform
    void setFloat(const std::string& name, float value) const;

    // upload vec3 uniform
    void setVec3(const std::string& name, glm::vec3 value) const;

    // upload vec3 uniform
    void setVec3(const std::string& name, float x, float y, float z) const;

private:
    // utility function for checking shader compilation/linking errors.
    void checkCompileErrors(unsigned int shader, std::string type);
};
#endif