#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "AxoPlotl/commons/Color.h"
#include "AxoPlotl/rendering/Texture.h"
#include "AxoPlotl/typedefs/typedefs_OpenVolumeMesh.hpp"
#include "AxoPlotl/typedefs/typedefs_glm.hpp"

namespace AxoPlotl
{

class VolumeMeshRenderer
{
public:
    struct ClipBox {
        bool enabled_ = false;
        Vec2f x_;
        Vec2f y_;
        Vec2f z_;
    };
    ClipBox clip_box_;

    struct Property {
        enum class Visualization : int {
            SCALAR = 0,
            COLOR = 1,
            VEC3 = 2
        };
        Visualization vis_ = Visualization::COLOR;
        Vec2f range_ = {-9999, 9999};
        ColorMap color_map_;
    };

    bool render_anything_ = true;
    bool render_vertices_ = true;
    bool render_edges_ = true;
    bool render_faces_ = true;
    bool render_cells_ = true;

    float point_size_ = 12.0f;
    float line_width_ = 8.0f;
    float cell_scale_ = 0.9f;
    Vec4f cell_outline_color_ = Vec4f(0,0,0,1);

    Property vertex_property_;
    Property edge_property_;
    Property face_property_;
    Property cell_property_;

    struct alignas(16) VertexData {
        Vec4f property;
    };

    struct alignas(16) EdgeData {
        Vec4f property;
    };

    struct alignas(16) FaceData {
        Vec4f property;
    };

    struct alignas(16) CellData {
        Vec4f property;
    };

    struct DrawEdgeVertex {
        uint32_t vertex_index;
        uint32_t edge_index;
    };

    struct DrawFaceVertex {
        uint32_t vertex_index;
        uint32_t face_index;
    };

    struct DrawCellVertex {
        uint32_t vertex_index;
        uint32_t cell_index;
        Vec3f incenter;
    };

    struct StaticData
    {
        size_t n_vertices_ = 0;
        size_t n_edges_ = 0;
        size_t n_faces_ = 0;
        size_t n_cells_ = 0;
        std::vector<Vec4f> positions_;
        std::vector<uint32_t> vertex_draw_vertices_;
        std::vector<DrawEdgeVertex> edge_draw_vertices_;
        std::vector<DrawFaceVertex> face_triangle_draw_vertices_;
        std::vector<DrawCellVertex> cell_triangle_draw_vertices_;
        std::vector<DrawCellVertex> cell_line_draw_vertices_;
    };

    size_t n_vertices_ = 0;
    size_t n_edges_ = 0;
    size_t n_faces_ = 0;
    size_t n_cells_ = 0;

    // setup buffers for a given mesh (static)
    void init(const StaticData& _data);

    void update_vertex_data(const std::vector<VertexData>& _data);

    void update_edge_data(const std::vector<EdgeData>& _data);

    void update_face_data(const std::vector<FaceData>& _data);

    void update_cell_data(const std::vector<CellData>& _data);

    void render(const glm::mat4x4 &_mvp);

    void render_picking(const glm::mat4x4 &_mvp, int _id);

private:


    // Ideally we want
#if __APPLE__
    // TBO
#else
    // SSBO
#endif

    GLuint positionsTBO;
    GLuint vertexDataTBO;
    GLuint edgeDataTBO;
    GLuint faceDataTBO;
    GLuint cellDataTBO;

    GLuint positionsTex;
    GLuint vertexDataTex;
    GLuint edgeDataTex;
    GLuint faceDataTex;
    GLuint cellDataTex;

    GLuint vertexVAO;
    GLuint edgeVAO;
    GLuint faceVAO;
    GLuint cellTriangleVAO;
    GLuint cellLineVAO;

    GLuint vertexDrawVBO;
    GLuint edgeDrawVBO;
    GLuint faceDrawVBO;
    GLuint cellTriangleDrawVBO;
    GLuint cellLineDrawVBO;

    GLsizei idx_count_vertices_ = 0;
    GLsizei idx_count_edges_ = 0;
    GLsizei idx_count_face_triangles_ = 0;
    GLsizei idx_count_cell_triangles_ = 0;
    GLsizei idx_count_cell_lines_ = 0;
};

}
