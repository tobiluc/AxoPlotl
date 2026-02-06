#include "MeshRenderer.h"
#include "AxoPlotl/Runner.h"
#include "AxoPlotl/rendering/Shader.h"
#include "AxoPlotl/utils/Utils.h"

namespace AxoPlotl::GL
{

MeshRenderer::MeshRenderer()
{
}

MeshRenderer::~MeshRenderer()
{
    deleteBuffers();
}

void MeshRenderer::deleteBuffers()
{
    if (vbo_point_attrib_) {glDeleteBuffers(1, &vbo_point_attrib_);}
    if (vbo_line_attrib_) {glDeleteBuffers(1, &vbo_line_attrib_);}
    if (vbo_triangle_attrib_) {glDeleteBuffers(1, &vbo_triangle_attrib_);}

    if (vao_points_) {glDeleteVertexArrays(1, &vao_points_);}
    if (vao_lines_) {glDeleteVertexArrays(1, &vao_lines_);}
    if (vao_triangles_) {glDeleteVertexArrays(1, &vao_triangles_);}
    if (vao_triangles_picking_) {glDeleteVertexArrays(1, &vao_triangles_picking_);}

    if (ibo_points_) {glDeleteBuffers(1, &ibo_points_);}
    if (ibo_lines_) {glDeleteBuffers(1, &ibo_lines_);}
    if (ibo_triangles_) {glDeleteBuffers(1, &ibo_triangles_);}

    vbo_point_attrib_ = vbo_line_attrib_ = vbo_triangle_attrib_
    = vao_points_ = vao_lines_ = vao_triangles_ = vao_triangles_picking_
        = ibo_points_ = ibo_lines_ = ibo_triangles_ = 0;
}

void MeshRenderer::createBuffers()
{
    glGenVertexArrays(1, &vao_triangles_);
    glGenBuffers(1, &vbo_triangle_attrib_);
    glGenBuffers(1, &ibo_triangles_);

    glGenVertexArrays(1, &vao_triangles_picking_);

    glGenVertexArrays(1, &vao_lines_);
    glGenBuffers(1, &vbo_line_attrib_);
    glGenBuffers(1, &ibo_lines_);

    glGenVertexArrays(1, &vao_points_);
    glGenBuffers(1, &vbo_point_attrib_);
    glGenBuffers(1, &ibo_points_);
}

void MeshRenderer::setupVertexAttributes()
{
    //---------------
    // Triangles
    //---------------
    glBindVertexArray(vao_triangles_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_attrib_);

    // -- Attrib 0: Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTriangleAttrib), (void*)offsetof(VertexTriangleAttrib, position));
    glEnableVertexAttribArray(0);

    // -- Attrib 1: Color
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexTriangleAttrib), (void*)offsetof(VertexTriangleAttrib, color));
    glEnableVertexAttribArray(1);

    // -- Attrib 2: Normal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTriangleAttrib), (void*)offsetof(VertexTriangleAttrib, normal));
    glEnableVertexAttribArray(2);

    // -- Attrib 3: Buffer
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(VertexTriangleAttrib), (void*)offsetof(VertexTriangleAttrib, buffer));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);

    //--------------------------
    // Triangles (Picking)
    //--------------------------
    glBindVertexArray(vao_triangles_picking_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_attrib_);

    // -- Attrib 0: Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTriangleAttrib), (void*)offsetof(VertexTriangleAttrib, position));
    glEnableVertexAttribArray(0);

    // -- Attrib 1: Buffer
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(VertexTriangleAttrib), (void*)offsetof(VertexTriangleAttrib, buffer));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    //---------------
    // Lines
    //---------------
    glBindVertexArray(vao_lines_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_line_attrib_);

    // -- Attrib 0: Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexLineAttrib), (void*)offsetof(VertexLineAttrib, position));
    glEnableVertexAttribArray(0);

    // -- Attrib 1: Color
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexLineAttrib), (void*)offsetof(VertexLineAttrib, color));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    //---------------
    // Points
    //---------------
    glBindVertexArray(vao_points_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_point_attrib_);

    // -- Attrib 0: Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPointAttrib), (void*)offsetof(VertexPointAttrib, position));
    glEnableVertexAttribArray(0);

    // -- Attrib 1: Color
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexPointAttrib), (void*)offsetof(VertexPointAttrib, color));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void MeshRenderer::updatePointsAttributes(const std::vector<VertexPointAttrib>& _p_attribs)
{
    glBindVertexArray(vao_points_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_point_attrib_);
    glBufferData(GL_ARRAY_BUFFER, _p_attribs.size() * sizeof(VertexPointAttrib), _p_attribs.data(), GL_DYNAMIC_DRAW);
    glBindVertexArray(0);
}

void MeshRenderer::updateLinesAttributes(const std::vector<VertexLineAttrib>& _l_attribs)
{
    glBindVertexArray(vao_lines_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_line_attrib_);
    glBufferData(GL_ARRAY_BUFFER, _l_attribs.size() * sizeof(VertexLineAttrib), _l_attribs.data(), GL_DYNAMIC_DRAW);
    glBindVertexArray(0);
}

void MeshRenderer::updateTrianglesAttributes(const std::vector<VertexTriangleAttrib>& _t_attribs)
{
    glBindVertexArray(vao_triangles_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_attrib_);
    glBufferData(GL_ARRAY_BUFFER, _t_attribs.size() * sizeof(VertexTriangleAttrib), _t_attribs.data(), GL_DYNAMIC_DRAW);

    glBindVertexArray(vao_triangles_picking_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_attrib_);
    glBufferData(GL_ARRAY_BUFFER, _t_attribs.size() * sizeof(VertexTriangleAttrib), _t_attribs.data(), GL_DYNAMIC_DRAW);

    glBindVertexArray(0);
}

void MeshRenderer::updateData(const Data& data)
{
    if (!vao_points_ || !vao_lines_ || !vao_triangles_ || !vao_triangles_picking_) {
        createBuffers();
        setupVertexAttributes();
    }

    n_points_ = data.pointIndices.size();
    n_lines_     = data.lineIndices.size() / 2;
    n_triangles_ = data.triangleIndices.size() / 3;

    //-------------
    // Triangles
    //-------------
    glBindVertexArray(vao_triangles_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_triangles_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.triangleIndices.size() * sizeof(GLuint), data.triangleIndices.data(), GL_DYNAMIC_DRAW);

    glBindVertexArray(vao_triangles_picking_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_triangles_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.triangleIndices.size() * sizeof(GLuint), data.triangleIndices.data(), GL_DYNAMIC_DRAW);

    updateTrianglesAttributes(data.triangleAttribs);

    //-----------
    // Lines
    //-----------
    glBindVertexArray(vao_lines_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_lines_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.lineIndices.size() * sizeof(GLuint), data.lineIndices.data(), GL_DYNAMIC_DRAW);

    updateLinesAttributes(data.lineAttribs);

    //-----------
    // Points
    //-----------
    glBindVertexArray(vao_points_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_points_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, n_points_ * sizeof(GLuint), data.pointIndices.data(), GL_DYNAMIC_DRAW);

    updatePointsAttributes(data.pointAttribs);

    glBindVertexArray(0);
}

void MeshRenderer::createData(const PolyhedralMesh& mesh, Data& data)
{
    data.pointAttribs.clear();
    data.lineAttribs.clear();
    data.triangleAttribs.clear();
    data.pointIndices.clear();
    data.lineIndices.clear();
    data.triangleIndices.clear();

    for (auto v_it = mesh.v_iter(); v_it.is_valid(); ++v_it) {

        // Compute smooth normal
        Vec3f n(0,0,0);
        for (auto vf_it = mesh.vf_iter(*v_it); vf_it.is_valid(); ++vf_it) {
            n += toVec3<Vec3f>(mesh.normal(vf_it->halfface_handle(0)));
        }
        n = glm::normalize(n);

        data.pointAttribs.push_back(VertexPointAttrib{
            .position = toVec3<Vec3f>(mesh.vertex(*v_it)),
            .color = Color::BLACK
        });

        data.lineAttribs.push_back(VertexLineAttrib{
            .position = toVec3<Vec3f>(mesh.vertex(*v_it)),
            .color = Color(v_it->idx()/(float)mesh.n_vertices(),0,0)
        });

        data.triangleAttribs.push_back(VertexTriangleAttrib{
            .position = toVec3<Vec3f>(mesh.vertex(*v_it)),
            .color = Color::WHITE,
            .normal = n,
            .buffer = 0.0f
        });

    }

    for (auto v_it = mesh.v_iter(); v_it.is_valid(); ++v_it) {
        data.pointIndices.push_back(v_it->uidx());
    }

    for (auto e_it = mesh.e_iter(); e_it.is_valid(); ++e_it) {
        uint v0 = mesh.from_vertex_handle(e_it->halfedge_handle(0)).uidx();
        uint v1 = mesh.from_vertex_handle(e_it->halfedge_handle(1)).uidx();
        data.lineIndices.push_back(v0);
        data.lineIndices.push_back(v1);
    }

    for (auto f_it = mesh.f_iter(); f_it.is_valid(); ++f_it) {
        const auto& vhs = mesh.get_halfface_vertices(f_it->halfface_handle(0));
        for (uint j = 1; j < vhs.size()-1; ++j) {
            data.triangleIndices.push_back(vhs[0].uidx());
            data.triangleIndices.push_back(vhs[j].uidx());
            data.triangleIndices.push_back(vhs[j+1].uidx());
        }
    }
}

void MeshRenderer::render(const Matrices &m)
{
    if (!settings_.visible) {return;}

    // Get Width and Height of Viewport
    GLint width, height;
    getViewportSize(width, height);

    // Points
    if (vao_points_ && settings_.renderPoints)
    {
        Shader::VERTICES_SHADER.use();
        Shader::VERTICES_SHADER.setFloat("point_size", settings_.pointSize);
        Shader::VERTICES_SHADER.setVec4f("min_color", settings_.scalar_property_range.min_color);
        Shader::VERTICES_SHADER.setVec4f("max_color", settings_.scalar_property_range.max_color);
        Shader::VERTICES_SHADER.setVec2f("visible_data_range", settings_.scalar_property_range.min_value,settings_.scalar_property_range.max_value);
        Shader::VERTICES_SHADER.setMat4x4f("model_view_projection_matrix", m.model_view_projection_matrix);

        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(-1.0f, -1.0f); // ensure the vertices are drawn slightly in front

        glBindVertexArray(vao_points_);
        glDrawElements(GL_POINTS, 1*n_points_, GL_UNSIGNED_INT, NULL);

        glDisable(GL_POLYGON_OFFSET_FILL);

        glBindVertexArray(0);
    }

    if (vao_lines_ && settings_.renderLines)
    {
        // Lines
        Shader::EDGES_SHADER.use();

        Shader::EDGES_SHADER.setMat4x4f("model_view_projection_matrix", m.model_view_projection_matrix);
        Shader::EDGES_SHADER.setVec2f("inverse_viewport_size", 1.f/width, 1.f/height);

        Shader::EDGES_SHADER.setFloat("line_width", settings_.lineWidth);

        Shader::EDGES_SHADER.setBool("use_global_color", settings_.useGlobalLineColor);
        Shader::EDGES_SHADER.setVec4f("global_color", settings_.globalLineColor);

        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(-0.75f, -0.75f); // ensure the lines are drawn slightly in front

        glBindVertexArray(vao_lines_);
        glDrawElements(GL_LINES, 2*n_lines_, GL_UNSIGNED_INT, NULL);

        glDisable(GL_POLYGON_OFFSET_FILL);

        glBindVertexArray(0);
    }

    if (vao_triangles_ && settings_.renderTriangles)
    {
        // Triangles
        Shader::FACES_SHADER.use();

        // Shader::FACES_SHADER.setMat4x4f("view_matrix", m.view_matrix);
        Shader::FACES_SHADER.setMat4x4f("model_view_projection_matrix", m.model_view_projection_matrix);
        // Shader::FACES_SHADER.setMat3x3f("normal_matrix", m.normal_matrix);

        // Shader::FACES_SHADER.setVec3f("light.position", Vec3f(0,0,0));
        // Shader::FACES_SHADER.setVec3f("light.ambient", settings_.light.ambient);
        // Shader::FACES_SHADER.setVec3f("light.diffuse", settings_.light.diffuse);
        // Shader::FACES_SHADER.setVec3f("light.specular", settings_.light.specular);

        Shader::FACES_SHADER.setBool("use_global_color", settings_.useGlobalTriangleColor);
        Shader::FACES_SHADER.setVec4f("global_color", settings_.gobalTriangleColor);

        // Shader::FACES_OUTLINES_SHADER.use();

        // Shader::FACES_OUTLINES_SHADER.setMat4x4f("model_view_projection_matrix", m.model_view_projection_matrix);
        // Shader::FACES_OUTLINES_SHADER.setVec2f("inverse_viewport_size", 1.f/width, 1.f/height);
        // Shader::FACES_OUTLINES_SHADER.setFloat("outline_width", settings_.outlineWidth);
        // Shader::FACES_OUTLINES_SHADER.setVec3f("outline_color", settings_.outlineColor);

        Shader::FACES_SHADER.use();

        glBindVertexArray(vao_triangles_);

        glPolygonMode( GL_FRONT_AND_BACK, settings_.wireframe? GL_LINE : GL_FILL );
        glDrawElements(GL_TRIANGLES, 3*n_triangles_, GL_UNSIGNED_INT, NULL);
        if (settings_.wireframe) {glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );}

        glBindVertexArray(0);
    }

}

void MeshRenderer::renderPicking(const Matrices& m, int id)
{
    if (!settings_.visible) {return;}

    Shader::PICKING_SHADER.use();

    Shader::PICKING_SHADER.setMat4x4f("model_view_projection_matrix", m.model_view_projection_matrix);
    Shader::PICKING_SHADER.setUInt("object_index", id);

    glBindVertexArray(vao_triangles_picking_);

    glDrawElements(GL_TRIANGLES, 3*n_triangles_, GL_UNSIGNED_INT, NULL);

    glBindVertexArray(0);
}

}
