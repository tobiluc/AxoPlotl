#include "Shader.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <sstream>

namespace MV
{
Shader Shader::FACES_OUTLINES_SHADER;
Shader Shader::TET_CELLS_SHADER;
Shader Shader::FACES_SHADER;
Shader Shader::EDGES_SHADER;
Shader Shader::VERTICES_SHADER;

Shader::Shader(const char* filepath)
{
    std::string vs = "", gs = "", fs = "";
    Shader::readFile(filepath, vs, gs, fs);

    GLuint vertex = 0, geometry = 0, fragment = 0;

    ID = glCreateProgram();

    // vertex shader
    if (!vs.empty())
    {
        const char* vc = vs.c_str();
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vc, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        glAttachShader(ID, vertex);
    }

    // geometry shader
    if (!gs.empty())
    {
        const char* gc = gs.c_str();
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gc, NULL);
        glCompileShader(geometry);
        checkCompileErrors(geometry, "GEOMETRY");
        glAttachShader(ID, geometry);
    }

    // fragment Shader
    if (!fs.empty())
    {
        const char* fc = fs.c_str();
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fc, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        glAttachShader(ID, fragment);
    }

    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(geometry);
    glDeleteShader(fragment);
}

void Shader::readFile(const char* filepath, std::string& vertexCode, std::string& geometryCode, std::string& fragmentCode)
{
    std::ifstream file;

    try
    {
        file.open(filepath);
        std::stringstream stream;
        stream << file.rdbuf();
        file.close();

        std::string src = stream.str();

        // Regular expression to match sections: #shader <type>\n<shader code>
        std::regex rgx(R"(#shader\s+(vertex|fragment|geometry)\n((.|\n)*?)(?=#shader|$))");
        std::smatch matches;
        std::string::const_iterator from(src.cbegin());
        while (std::regex_search(from, src.cend(), matches, rgx)) {
            const std::string& type = matches[1].str();
            if (type == "vertex") {vertexCode = matches[2].str();}
            else if (type == "geometry") {geometryCode = matches[2].str();}
            else if (type == "fragment") {fragmentCode = matches[2].str();}

            from = matches.suffix().first; // Move the search position forward
        }
    }
    catch (std::ifstream::failure& e)
    {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
        exit(1);
    }
}

void Shader::checkCompileErrors(unsigned int shader, std::string type) {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            exit(1);
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            exit(1);
        }
    }
}
}
