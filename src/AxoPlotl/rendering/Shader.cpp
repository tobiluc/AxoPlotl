#include "Shader.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <sstream>
#include "AxoPlotl/rendering/shader_source.ipp"

namespace AxoPlotl
{
Shader Shader::TBO_VERTICES_SHADER;
Shader Shader::TBO_EDGES_SHADER;
Shader Shader::TBO_FACES_SHADER;
Shader Shader::TBO_CELLS_SHADER;
Shader Shader::TBO_FACES_PICKING_SHADER;
Shader Shader::TBO_CELLS_OUTLINE_SHADER;
Shader Shader::TBO_CELLS_PICKING_SHADER;

void Shader::loadGlobalShaders()
{
    shader_from_source(tbo_vertices_shader_src, TBO_VERTICES_SHADER);
    shader_from_source(tbo_edges_shader_src, TBO_EDGES_SHADER);
    shader_from_source(tbo_faces_shader_src, TBO_FACES_SHADER);
    shader_from_source(tbo_cells_shader_src, TBO_CELLS_SHADER);
    shader_from_source(tbo_faces_picking_shader_src, TBO_FACES_PICKING_SHADER);
    shader_from_source(tbo_cells_outline_shader_src, TBO_CELLS_OUTLINE_SHADER);
    shader_from_source(tbo_cells_picking_shader_src, TBO_CELLS_PICKING_SHADER);
}

void Shader::shader_from_file(const std::filesystem::path& _path, Shader& _shader)
{
    std::ifstream file;

    try
    {
        file.open(_path);
        std::stringstream stream;
        stream << file.rdbuf();
        file.close();

        shader_from_source(stream.str().c_str(), _shader);
    }
    catch (std::ifstream::failure& e)
    {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
        exit(1);
    }
}

void Shader::shader_from_source(const char* _src, Shader& _shader)
{
    std::string vs = "", gs = "", fs = "";
    Shader::read_source(std::string(_src), vs, gs, fs);

    GLuint vertex = 0, geometry = 0, fragment = 0;

    GLuint ID = glCreateProgram();

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

    _shader.ID = ID;
}

void Shader::read_source(const std::string& src, std::string& vertexCode, std::string& geometryCode, std::string& fragmentCode)
{
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
