#include "VolumeMeshRenderer.hpp"
#include "AxoPlotl/Runner.h"
#include "AxoPlotl/rendering/Shader.h"

namespace AxoPlotl
{

void VolumeMeshRenderer::init(const StaticData& _data)
{
    n_vertices_ = _data.n_vertices_;
    n_edges_ = _data.n_edges_;
    n_faces_ = _data.n_faces_;
    n_cells_ = _data.n_cells_;

    //-------------
    // Upload Positions
    //-------------
    glGenBuffers(1, &positionsTBO);
    glBindBuffer(GL_TEXTURE_BUFFER, positionsTBO);
    glBufferData(GL_TEXTURE_BUFFER,
     _data.positions_.size() * sizeof(Vec4f),
     _data.positions_.data(),
     GL_STATIC_DRAW);

    glGenTextures(1, &positionsTex);
    glBindTexture(GL_TEXTURE_BUFFER, positionsTex);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, positionsTBO);

    //-------------
    // Vertex VBO
    //-------------
    idx_count_vertices_ = _data.vertex_draw_vertices_.size();

    glGenVertexArrays(1, &vertexVAO);
    glBindVertexArray(vertexVAO);

    glGenBuffers(1, &vertexDrawVBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertexDrawVBO);
    glBufferData(GL_ARRAY_BUFFER,
        _data.vertex_draw_vertices_.size() * sizeof(uint32_t),
        _data.vertex_draw_vertices_.data(),
        GL_STATIC_DRAW);

    // Attrib 0 --- Vertex Index
    glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(uint32_t), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture Creation
    glGenBuffers(1, &vertexDataTBO);
    glBindBuffer(GL_TEXTURE_BUFFER, vertexDataTBO);
    glBufferData(GL_TEXTURE_BUFFER, _data.n_vertices_*sizeof(VertexData), nullptr, GL_DYNAMIC_DRAW);
    glGenTextures(1, &vertexDataTex);
    glBindTexture(GL_TEXTURE_BUFFER, vertexDataTex);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, vertexDataTBO);

    //-------------
    // Edge VBO
    //-------------
    idx_count_edges_ = _data.edge_draw_vertices_.size();

    glGenVertexArrays(1, &edgeVAO);
    glBindVertexArray(edgeVAO);

    glGenBuffers(1, &edgeDrawVBO);
    glBindBuffer(GL_ARRAY_BUFFER, edgeDrawVBO);
    glBufferData(GL_ARRAY_BUFFER,
        _data.edge_draw_vertices_.size() * sizeof(DrawEdgeVertex),
        _data.edge_draw_vertices_.data(),
        GL_STATIC_DRAW);

    // Attrib 0 --- Vertex Index
    glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT,
        sizeof(DrawEdgeVertex), (void*)0);
    glEnableVertexAttribArray(0);

    // Attrib 1 --- Edge Index
    glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT,
        sizeof(DrawEdgeVertex),
        (void*)offsetof(DrawEdgeVertex, edge_index));
    glEnableVertexAttribArray(1);

    // Texture Creation
    glGenBuffers(1, &edgeDataTBO);
    glBindBuffer(GL_TEXTURE_BUFFER, edgeDataTBO);
    glBufferData(GL_TEXTURE_BUFFER, _data.n_edges_*sizeof(EdgeData), nullptr, GL_DYNAMIC_DRAW);
    glGenTextures(1, &edgeDataTex);
    glBindTexture(GL_TEXTURE_BUFFER, edgeDataTex);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, edgeDataTBO);

    //--------------------
    // Face Triangle VBO
    //--------------------
    idx_count_face_triangles_ = _data.face_triangle_draw_vertices_.size();

    glGenVertexArrays(1, &faceVAO);
    glBindVertexArray(faceVAO);

    glGenBuffers(1, &faceDrawVBO);
    glBindBuffer(GL_ARRAY_BUFFER, faceDrawVBO);
    glBufferData(GL_ARRAY_BUFFER,
        _data.face_triangle_draw_vertices_.size() * sizeof(DrawFaceVertex),
        _data.face_triangle_draw_vertices_.data(),
        GL_STATIC_DRAW);

    // Attrib 0 --- Vertex Index
    glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT,
        sizeof(DrawFaceVertex), (void*)0);
    glEnableVertexAttribArray(0);

    // Attrib 1 --- Face Index
    glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT,
        sizeof(DrawFaceVertex),
        (void*)offsetof(DrawFaceVertex, face_index));
    glEnableVertexAttribArray(1);

    // Texture Creation
    glGenBuffers(1, &faceDataTBO);
    glBindBuffer(GL_TEXTURE_BUFFER, faceDataTBO);
    glBufferData(GL_TEXTURE_BUFFER, _data.n_faces_*sizeof(FaceData), nullptr, GL_DYNAMIC_DRAW);
    glGenTextures(1, &faceDataTex);
    glBindTexture(GL_TEXTURE_BUFFER, faceDataTex);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, faceDataTBO);

    //--------------------
    // Cell Triangle VBO
    //--------------------
    idx_count_cell_triangles_ = _data.cell_triangle_draw_vertices_.size();

    glGenVertexArrays(1, &cellTriangleVAO);
    glBindVertexArray(cellTriangleVAO);

    glGenBuffers(1, &cellTriangleDrawVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cellTriangleDrawVBO);
    glBufferData(GL_ARRAY_BUFFER,
         _data.cell_triangle_draw_vertices_.size() * sizeof(DrawCellVertex),
         _data.cell_triangle_draw_vertices_.data(),
         GL_STATIC_DRAW);

    // Attrib 0 --- Vertex Index
    glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT,
        sizeof(DrawCellVertex), (void*)0);
    glEnableVertexAttribArray(0);

    // Attrib 1 --- Cell Index
    glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT,
        sizeof(DrawCellVertex),
        (void*)offsetof(DrawCellVertex, cell_index));
    glEnableVertexAttribArray(1);

    // Attrib 2 --- Cell Incenter
    glVertexAttribPointer(2, 3,
        GL_FLOAT, GL_FALSE, sizeof(DrawCellVertex),
        (void*)offsetof(DrawCellVertex, incenter));
    glEnableVertexAttribArray(2);

    // Texture Creation
    glGenBuffers(1, &cellDataTBO);
    glBindBuffer(GL_TEXTURE_BUFFER, cellDataTBO);
    glBufferData(GL_TEXTURE_BUFFER, _data.n_cells_*sizeof(CellData), nullptr, GL_DYNAMIC_DRAW);
    glGenTextures(1, &cellDataTex);
    glBindTexture(GL_TEXTURE_BUFFER, cellDataTex);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, cellDataTBO);

    //--------------------
    // Cell Line VBO
    //--------------------
    idx_count_cell_lines_ = _data.cell_line_draw_vertices_.size();

    glGenVertexArrays(1, &cellLineVAO);
    glBindVertexArray(cellLineVAO);

    glGenBuffers(1, &cellLineDrawVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cellLineDrawVBO);
    glBufferData(GL_ARRAY_BUFFER,
        _data.cell_line_draw_vertices_.size() * sizeof(DrawCellVertex),
        _data.cell_line_draw_vertices_.data(),
        GL_STATIC_DRAW);

    // Attrib 0 --- Vertex Index
    glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT,
        sizeof(DrawCellVertex), (void*)0);
    glEnableVertexAttribArray(0);

    // Attrib 1 --- Cell Index
    glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT,
        sizeof(DrawCellVertex),
        (void*)offsetof(DrawCellVertex, cell_index));
    glEnableVertexAttribArray(1);

    // Attrib 2 --- Cell Incenter
    glVertexAttribPointer(2, 3,
        GL_FLOAT, GL_FALSE, sizeof(DrawCellVertex),
        (void*)offsetof(DrawCellVertex, incenter));
    glEnableVertexAttribArray(2);


    //--------------------------------
    // Create Visualization Settings
    //--------------------------------
    vertex_property_.color_map_.create();
    vertex_property_.color_map_.set_coolwarm(128);
    edge_property_.color_map_.create();
    edge_property_.color_map_.set_coolwarm(128);
    face_property_.color_map_.create();
    face_property_.color_map_.set_coolwarm(128);
    cell_property_.color_map_.create();
    cell_property_.color_map_.set_coolwarm(128);
}

void VolumeMeshRenderer::update_vertex_data(
    const std::vector<VertexData>& _data)
{
    glBindBuffer(GL_TEXTURE_BUFFER, vertexDataTBO);
    glBufferData(GL_TEXTURE_BUFFER,
    _data.size() * sizeof(VertexData),
    _data.data(), GL_DYNAMIC_DRAW);
}

void VolumeMeshRenderer::update_edge_data(
    const std::vector<EdgeData>& _data)
{
    glBindBuffer(GL_TEXTURE_BUFFER, edgeDataTBO);
    glBufferData(GL_TEXTURE_BUFFER,
                 _data.size() * sizeof(EdgeData),
                 _data.data(), GL_DYNAMIC_DRAW);
}

void VolumeMeshRenderer::update_face_data(
    const std::vector<FaceData>& _data)
{
    glBindBuffer(GL_TEXTURE_BUFFER, faceDataTBO);
    glBufferData(GL_TEXTURE_BUFFER,
                 _data.size() * sizeof(FaceData),
                 _data.data(), GL_DYNAMIC_DRAW);
}

void VolumeMeshRenderer::update_cell_data(
    const std::vector<CellData>& _data)
{
    glBindBuffer(GL_TEXTURE_BUFFER, cellDataTBO);
    glBufferData(GL_TEXTURE_BUFFER,
                 _data.size() * sizeof(CellData),
                 _data.data(), GL_DYNAMIC_DRAW);
}

void VolumeMeshRenderer::render(const glm::mat4x4 &_mvp)
{
    if (!render_anything_) {return;}

    Vec3f clip_min = {clip_box_.x_[0], clip_box_.y_[0], clip_box_.z_[0]};
    Vec3f clip_max = {clip_box_.x_[1], clip_box_.y_[1], clip_box_.z_[1]};

    //---------------
    // Vertices
    //---------------
    if (render_vertices_)
    {
        glBindVertexArray(vertexVAO);

        Shader::TBO_VERTICES_SHADER.use();
        Shader::TBO_VERTICES_SHADER.setMat4x4f("model_view_projection_matrix", _mvp);
        Shader::TBO_VERTICES_SHADER.setFloat("point_size", point_size_);
        Shader::TBO_VERTICES_SHADER.setBool("clip_box.enabled", clip_box_.enabled_);
        Shader::TBO_VERTICES_SHADER.setVec3f("clip_box.min", clip_min);
        Shader::TBO_VERTICES_SHADER.setVec3f("clip_box.max", clip_max);
        Shader::TBO_VERTICES_SHADER.setInt("property.visualization", static_cast<int>(vertex_property_.vis_));
        Shader::TBO_VERTICES_SHADER.setVec2f("property.range", vertex_property_.range_);
        vertex_property_.color_map_.bind(2); // unit 2 since 0,1 are used for our data
        Shader::TBO_VERTICES_SHADER.setInt("property.colormap", 2);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_BUFFER, positionsTex);
        Shader::TBO_VERTICES_SHADER.setInt("positions", 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_BUFFER, vertexDataTex);
        Shader::TBO_VERTICES_SHADER.setInt("vertex_data", 1);

        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(-0.75f, -0.75f);
        glDrawArrays(GL_POINTS, 0, idx_count_vertices_);
        glDisable(GL_POLYGON_OFFSET_FILL);

        Shader::TBO_VERTICES_SHADER.detach();
    }

    //---------------
    // Edges
    //---------------
    if (render_edges_)
    {
        // Get Width and Height of Viewport
        GLint width, height;
        getViewportSize(width, height);

        glBindVertexArray(edgeVAO);

        Shader::TBO_EDGES_SHADER.use();
        Shader::TBO_EDGES_SHADER.setMat4x4f("model_view_projection_matrix", _mvp);
        Shader::TBO_EDGES_SHADER.setFloat("line_width", line_width_);
        Shader::TBO_EDGES_SHADER.setVec2f("inverse_viewport_size", 1.f/width, 1.f/height);
        Shader::TBO_EDGES_SHADER.setBool("clip_box.enabled", clip_box_.enabled_);
        Shader::TBO_EDGES_SHADER.setVec3f("clip_box.min", clip_min);
        Shader::TBO_EDGES_SHADER.setVec3f("clip_box.max", clip_max);
        Shader::TBO_EDGES_SHADER.setInt("property.visualization", static_cast<int>(edge_property_.vis_));
        Shader::TBO_EDGES_SHADER.setVec2f("property.range", edge_property_.range_);
        edge_property_.color_map_.bind(2);
        Shader::TBO_EDGES_SHADER.setInt("property.colormap", 2);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_BUFFER, positionsTex);
        Shader::TBO_EDGES_SHADER.setInt("positions", 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_BUFFER, edgeDataTex);
        Shader::TBO_EDGES_SHADER.setInt("edge_data", 1);

        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(-0.75f, -0.75f);
        glDrawArrays(GL_LINES, 0, idx_count_edges_);
        glDisable(GL_POLYGON_OFFSET_FILL);

        Shader::TBO_EDGES_SHADER.detach();
    }

    //---------------
    // Faces
    //---------------
    if (render_faces_)
    {
        glBindVertexArray(faceVAO);

        Shader::TBO_FACES_SHADER.use();
        Shader::TBO_FACES_SHADER.setMat4x4f("model_view_projection_matrix", _mvp);
        Shader::TBO_FACES_SHADER.setBool("clip_box.enabled", clip_box_.enabled_);
        Shader::TBO_FACES_SHADER.setVec3f("clip_box.min", clip_min);
        Shader::TBO_FACES_SHADER.setVec3f("clip_box.max", clip_max);
        Shader::TBO_FACES_SHADER.setInt("property.visualization", static_cast<int>(face_property_.vis_));
        Shader::TBO_FACES_SHADER.setVec2f("property.range", face_property_.range_);
        face_property_.color_map_.bind(2);
        Shader::TBO_FACES_SHADER.setInt("property.colormap", 2);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_BUFFER, positionsTex);
        Shader::TBO_FACES_SHADER.setInt("positions", 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_BUFFER, faceDataTex);
        Shader::TBO_FACES_SHADER.setInt("face_data", 1);

        glDrawArrays(GL_TRIANGLES, 0, idx_count_face_triangles_);
        Shader::TBO_FACES_SHADER.detach();
    }

    //---------------
    // Cells
    //---------------
    if (render_cells_)
    {
        glBindVertexArray(cellTriangleVAO);

        Shader::TBO_CELLS_SHADER.use();
        Shader::TBO_CELLS_SHADER.setMat4x4f("model_view_projection_matrix", _mvp);
        Shader::TBO_CELLS_SHADER.setFloat("cell_scale", cell_scale_);
        Shader::TBO_CELLS_SHADER.setBool("clip_box.enabled", clip_box_.enabled_);
        Shader::TBO_CELLS_SHADER.setVec3f("clip_box.min", clip_min);
        Shader::TBO_CELLS_SHADER.setVec3f("clip_box.max", clip_max);
        Shader::TBO_CELLS_SHADER.setInt("property.visualization", static_cast<int>(cell_property_.vis_));
        Shader::TBO_CELLS_SHADER.setVec2f("property.range", cell_property_.range_);
        cell_property_.color_map_.bind(2);
        Shader::TBO_CELLS_SHADER.setInt("property.colormap", 2);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_BUFFER, positionsTex);
        Shader::TBO_CELLS_SHADER.setInt("positions", 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_BUFFER, cellDataTex);
        Shader::TBO_CELLS_SHADER.setInt("cell_data", 1);

        glDrawArrays(GL_TRIANGLES, 0, idx_count_cell_triangles_);
        Shader::TBO_CELLS_SHADER.detach();

        // Cells Outline
        glBindVertexArray(cellLineVAO);
        Shader::TBO_CELLS_OUTLINE_SHADER.use();
        Shader::TBO_CELLS_OUTLINE_SHADER.setMat4x4f("model_view_projection_matrix", _mvp);
        Shader::TBO_CELLS_OUTLINE_SHADER.setFloat("cell_scale", cell_scale_);
        Shader::TBO_CELLS_OUTLINE_SHADER.setVec4f("outline_color", cell_outline_color_);
        Shader::TBO_CELLS_OUTLINE_SHADER.setBool("clip_box.enabled", clip_box_.enabled_);
        Shader::TBO_CELLS_OUTLINE_SHADER.setVec3f("clip_box.min", clip_min);
        Shader::TBO_CELLS_OUTLINE_SHADER.setVec3f("clip_box.max", clip_max);
        Shader::TBO_CELLS_OUTLINE_SHADER.setInt("property.visualization", static_cast<int>(cell_property_.vis_));
        Shader::TBO_CELLS_OUTLINE_SHADER.setVec2f("property.range", cell_property_.range_);
        cell_property_.color_map_.bind(2);
        Shader::TBO_CELLS_OUTLINE_SHADER.setInt("property.colormap", 2);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_BUFFER, positionsTex);
        Shader::TBO_CELLS_OUTLINE_SHADER.setInt("positions", 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_BUFFER, cellDataTex);
        Shader::TBO_CELLS_OUTLINE_SHADER.setInt("cell_data", 1);

        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(-0.75f, -0.75f);
        glDrawArrays(GL_LINES, 0, idx_count_cell_lines_);
        glDisable(GL_POLYGON_OFFSET_FILL);

        Shader::TBO_CELLS_OUTLINE_SHADER.detach();

    }
}

void VolumeMeshRenderer::render_picking(const glm::mat4x4 &_mvp, int _id)
{
    if (render_faces_)
    {
        glBindVertexArray(faceVAO);

        Shader::TBO_PICK_FACES_SHADER.use();
        Shader::TBO_PICK_FACES_SHADER.setMat4x4f("model_view_projection_matrix", _mvp);
        Shader::TBO_PICK_FACES_SHADER.setUInt("object_index", _id);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_BUFFER, positionsTex);
        Shader::TBO_PICK_FACES_SHADER.setInt("positions", 0);

        glDrawArrays(GL_TRIANGLES, 0, idx_count_face_triangles_);
        Shader::TBO_PICK_FACES_SHADER.detach();
    }
}

}
