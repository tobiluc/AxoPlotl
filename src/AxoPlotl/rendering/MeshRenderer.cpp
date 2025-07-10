#include "MeshRenderer.h"
#include "AxoPlotl/commons/Shader.h"
#include "AxoPlotl/utils/Utils.h"

namespace AxoPlotl::Rendering
{

MeshRenderer::MeshRenderer()
{}

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
}

void MeshRenderer::initFromMesh(const PolyhedralMesh &mesh)
{
    deleteBuffers();

    std::vector<VertexPointAttrib> pointAttribs;
    std::vector<VertexLineAttrib> lineAttribs;
    std::vector<VertexTriangleAttrib> triangleAttribs;
    std::vector<GLuint> pointIndices;
    std::vector<GLuint> lineIndices;
    std::vector<GLuint> triangleIndices;

    for (auto v_it = mesh.v_iter(); v_it.is_valid(); ++v_it) {

        // Compute smooth normal
        Vec3f n(0,0,0);
        for (auto vf_it = mesh.vf_iter(*v_it); vf_it.is_valid(); ++vf_it) {
            n += toVec3<Vec3f>(mesh.normal(vf_it->halfface_handle(0)));
        }
        n = glm::normalize(n);

        pointAttribs.push_back(VertexPointAttrib{
            .position = toVec3<Vec3f>(mesh.vertex(*v_it)),
            .color = Color::BLACK
        });

        lineAttribs.push_back(VertexLineAttrib{
           .position = toVec3<Vec3f>(mesh.vertex(*v_it)),
           .color = Color::LIGHTGRAY
        });

        triangleAttribs.push_back(VertexTriangleAttrib{
            .position = toVec3<Vec3f>(mesh.vertex(*v_it)),
            .color = Color::WHITE,
            .normal = n,
            .buffer = 0.0f
        });

    }

    for (auto v_it = mesh.v_iter(); v_it.is_valid(); ++v_it) {
        pointIndices.push_back(v_it->uidx());
    }

    for (auto e_it = mesh.e_iter(); e_it.is_valid(); ++e_it) {
        lineIndices.push_back(mesh.from_vertex_handle(e_it->halfedge_handle(0)).uidx());
        lineIndices.push_back(mesh.to_vertex_handle(e_it->halfedge_handle(0)).uidx());
    }

    for (auto f_it = mesh.f_iter(); f_it.is_valid(); ++f_it) {
        const auto& vhs = mesh.get_halfface_vertices(f_it->halfface_handle(0));
        for (uint j = 1; j < vhs.size()-1; ++j) {
            triangleIndices.push_back(vhs[0].uidx());
            triangleIndices.push_back(vhs[j].uidx());
            triangleIndices.push_back(vhs[j+1].uidx());
        }
    }

    n_points_ = pointIndices.size();
    n_lines_     = lineIndices.size() / 2;
    n_triangles_ = triangleIndices.size() / 3;

    //--------------------------
    // Triangles
    //--------------------------

    glGenVertexArrays(1, &vao_triangles_);
    glGenBuffers(1, &vbo_triangle_attrib_);
    glGenBuffers(1, &ibo_triangles_);

    // Upload data
    glBindVertexArray(vao_triangles_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_attrib_);
    glBufferData(GL_ARRAY_BUFFER, triangleAttribs.size() * sizeof(VertexTriangleAttrib), triangleAttribs.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_triangles_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangleIndices.size() * sizeof(GLuint), triangleIndices.data(), GL_STATIC_DRAW);

    // Enable only attribs used in shader

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
    // Lines
    //--------------------------

    glGenVertexArrays(1, &vao_lines_);
    glGenBuffers(1, &vbo_line_attrib_);
    glGenBuffers(1, &ibo_lines_);

    // Upload data
    glBindVertexArray(vao_lines_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_line_attrib_);
    glBufferData(GL_ARRAY_BUFFER, lineAttribs.size() * sizeof(VertexLineAttrib), lineAttribs.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_lines_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, lineIndices.size() * sizeof(GLuint), lineIndices.data(), GL_STATIC_DRAW);

    // Enable only attribs used in shader

    // -- Attrib 0: Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexLineAttrib), (void*)offsetof(VertexLineAttrib, position));
    glEnableVertexAttribArray(0);

    // -- Attrib 1: Color
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexLineAttrib), (void*)offsetof(VertexLineAttrib, color));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    //--------------------------
    // Points
    //--------------------------

    glGenVertexArrays(1, &vao_points_);
    glGenBuffers(1, &vbo_point_attrib_);
    glGenBuffers(1, &ibo_points_);

    // Upload data
    glBindVertexArray(vao_points_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_point_attrib_);
    glBufferData(GL_ARRAY_BUFFER, pointAttribs.size() * sizeof(VertexPointAttrib), pointAttribs.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_lines_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, pointIndices.size() * sizeof(GLuint), pointIndices.data(), GL_STATIC_DRAW);

    // Enable only attribs used in shader

    // -- Attrib 0: Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPointAttrib), (void*)offsetof(VertexPointAttrib, position));
    glEnableVertexAttribArray(0);

    // -- Attrib 1: Color
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexPointAttrib), (void*)offsetof(VertexPointAttrib, color));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void MeshRenderer::render(const Matrices &m)
{
    if (!settings_.visible) {return;}

    // Get Width and Height of Viewport
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    const float width = viewport[2];
    const float height = viewport[3];

    // float pointSize = 5.0f;
    // float lineWidth = 3.0f;
    // Light light{.ambient = Color::RED, .diffuse = Color::BLACK, .specular = Color::LIGHTGRAY};
    // float outlineWidth = 1.0f;
    // Color outlineColor = Color::BLACK;

    // Points
    if (settings_.renderPoints)
    {
        Shader::VERTICES_SHADER.use();
        Shader::VERTICES_SHADER.setFloat("point_size", settings_.pointSize);
        Shader::VERTICES_SHADER.setMat4x4f("model_view_projection_matrix", m.model_view_projection_matrix);

        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(-1.0f, -1.0f); // ensure the vertices are drawn slightly in front

        glBindVertexArray(vao_points_);
        glDrawElements(GL_POINTS, 1*n_points_, GL_UNSIGNED_INT, NULL);

        glDisable(GL_POLYGON_OFFSET_FILL);

        glBindVertexArray(0);
    }

    if (settings_.renderLines)
    {
        // Lines
        Shader::EDGES_SHADER.use();

        Shader::EDGES_SHADER.setMat4x4f("model_view_projection_matrix", m.model_view_projection_matrix);
        Shader::EDGES_SHADER.setVec2f("inverse_viewport_size", 1.f/width, 1.f/height);

        Shader::EDGES_SHADER.setFloat("line_width", settings_.lineWidth);

        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(-0.75f, -0.75f); // ensure the lines are drawn slightly in front

        glBindVertexArray(vao_lines_);
        glDrawElements(GL_LINES, 2*n_lines_, GL_UNSIGNED_INT, NULL);

        glDisable(GL_POLYGON_OFFSET_FILL);

        glBindVertexArray(0);
    }

    if (settings_.renderTriangles)
    {
        // Triangles
        Shader::FACES_SHADER.use();

        Shader::FACES_SHADER.setMat4x4f("view_matrix", m.view_matrix);
        Shader::FACES_SHADER.setMat4x4f("model_view_projection_matrix", m.model_view_projection_matrix);
        Shader::FACES_SHADER.setMat3x3f("normal_matrix", m.normal_matrix);

        Shader::FACES_SHADER.setVec3f("light.position", Vec3f(0,0,0));
        Shader::FACES_SHADER.setVec3f("light.ambient", settings_.light.ambient);
        Shader::FACES_SHADER.setVec3f("light.diffuse", settings_.light.diffuse);
        Shader::FACES_SHADER.setVec3f("light.specular", settings_.light.specular);

        Shader::FACES_OUTLINES_SHADER.use();

        Shader::FACES_OUTLINES_SHADER.setMat4x4f("model_view_projection_matrix", m.model_view_projection_matrix);
        Shader::FACES_OUTLINES_SHADER.setVec2f("inverse_viewport_size", 1.f/width, 1.f/height);
        Shader::FACES_OUTLINES_SHADER.setFloat("outline_width", settings_.outlineWidth);
        Shader::FACES_OUTLINES_SHADER.setVec3f("outline_color", settings_.outlineColor);

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

    // TODO
}

}
