#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "AxoPlotl/commons/Color.h"
#include "AxoPlotl/typedefs/typedefs_OpenVolumeMesh.hpp"
#include "AxoPlotl/typedefs/typedefs_glm.hpp"

namespace AxoPlotl::GL
{

class MeshRenderer
{
public:

    struct Matrices
    {
        glm::mat4x4 model_matrix;
        glm::mat4x4 view_matrix;
        glm::mat4x4 projection_matrix;
        glm::mat4x4 model_view_matrix;
        glm::mat4x4 model_view_projection_matrix;
        glm::mat3x3 normal_matrix;

        Matrices(const glm::mat4x4& m, const glm::mat4x4& v, const glm::mat4x4& p) :
            model_matrix(m),
            view_matrix(v),
            projection_matrix(p),
            model_view_matrix(v * m),
            model_view_projection_matrix(p * model_view_matrix),
            normal_matrix(glm::transpose(glm::inverse(model_view_matrix)))
        {}
    };

    struct ScalarRangeConfig
    {
        float min_value = 0;
        float max_value = 1;
        Vec4f min_color = {0,0,1,1};
        Vec4f max_color = {1,0,0,1};
    };

    struct Settings
    {
        bool visible = true;
        bool renderPoints = true;
        bool renderLines = true;
        bool renderTriangles = true;
        bool render_cells_ = true;
        float pointSize = 5.0f;
        float lineWidth = 3.0f;
        float cell_scale_ = 0.9f;
        bool useDataForPointColor = true;
        bool useDataForLineColor = true;
        bool useDataForTriangleColor = true;
        bool use_data_as_cell_color_ = true;


        ScalarRangeConfig scalar_property_range;

        // Color globalLineColor = Color::BLUE;
        // bool useGlobalLineColor = true;
        // Color gobalTriangleColor = Color::GREEN;
        // bool useGlobalTriangleColor = true;

        //Light light{.ambient = Color::LIGHTGRAY, .diffuse = Color::LIGHTGRAY, .specular = Color::DARKGRAY};
        // float outlineWidth = 1.0f;
        // Color outlineColor = Color::BLACK;
        bool wireframe = false;
    };

    typedef struct {
        Vec3f position;
        Vec4f color;
    } VertexPointAttrib;

    typedef struct {
        Vec3f position;
        Vec4f color;
    } VertexLineAttrib;

    typedef struct {
        Vec3f position;
        Vec4f color;
        Vec3f normal;
        float face_index;
    } VertexTriangleAttrib;

    typedef struct {
        Vec3f position;
        Vec4f data;
        Vec3f cell_incenter;
        float cell_index;
    } VertexCellAttrib;

    typedef struct {
        std::vector<VertexPointAttrib> pointAttribs;
        std::vector<VertexLineAttrib> lineAttribs;
        std::vector<VertexTriangleAttrib> triangleAttribs;
        std::vector<VertexCellAttrib> cell_attribs_;
        std::vector<GLuint> pointIndices;
        std::vector<GLuint> lineIndices;
        std::vector<GLuint> face_triangle_indices_;
        std::vector<GLuint> cell_triangle_indices_;
    } Data;

private:
    size_t n_points_;
    size_t n_lines_;
    size_t n_face_triangles_;
    size_t n_cell_triangles_;

    GLuint vbo_point_attrib_ = 0;
    GLuint vbo_line_attrib_ = 0;
    GLuint vbo_triangle_attrib_ = 0;
    GLuint vbo_cell_attrib_ = 0;
    GLuint vao_points_ = 0;
    GLuint vao_lines_ = 0;
    GLuint vao_triangles_ = 0;
    GLuint vao_cells_ = 0;
    GLuint vao_triangles_picking_ = 0;
    GLuint ibo_points_ = 0;
    GLuint ibo_lines_ = 0;
    GLuint ibo_triangles_ = 0;
    GLuint ibo_cells_ = 0;

    Settings settings_;

    void deleteBuffers();

    void createBuffers();

    void setupVertexAttributes();

    inline void init() {
        if (!vao_points_ || !vao_lines_ || !vao_triangles_ || !vao_triangles_picking_ || !vao_cells_) {
            createBuffers();
            setupVertexAttributes();
        }
    }

public:
    MeshRenderer();

    ~MeshRenderer();

    void updateVertexPoints(
        const std::vector<VertexPointAttrib>& _p_attribs,
        const std::vector<GLuint>& _v_point_indices);

    void updateEdgeLines(
        const std::vector<VertexLineAttrib>& _l_attribs,
        const std::vector<GLuint>& _e_line_indices);

    void updateFaceTriangles(
        const std::vector<VertexTriangleAttrib>& _t_attribs,
        const std::vector<GLuint>& _f_triangle_indices);

    void updateCellTriangles(
        const std::vector<VertexCellAttrib>& _c_attribs,
        const std::vector<GLuint>& _c_triangle_indices);

    void updateData(const Data& data);

    void render(const Matrices& m);

    void renderPicking(const Matrices& m, int id);

    inline Settings& settings() {return settings_;}

    inline uint n_points() const {return n_points_;}

    inline uint n_lines() const {return n_lines_;}

    inline uint n_triangles() const {return n_face_triangles_;}

    inline size_t n_cell_triangles() const {return n_cell_triangles_;}
};

}
