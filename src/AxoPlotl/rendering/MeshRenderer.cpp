// #include "MeshRenderer.h"
// #include "AxoPlotl/Runner.h"
// #include "AxoPlotl/rendering/Shader.h"
// #include "AxoPlotl/utils/Utils.h"

// namespace AxoPlotl::GL
// {

// MeshRenderer::MeshRenderer()
// {
// }

// MeshRenderer::~MeshRenderer()
// {
//     deleteBuffers();
// }

// void MeshRenderer::deleteBuffers()
// {
//     if (vbo_point_attrib_) {glDeleteBuffers(1, &vbo_point_attrib_);}
//     if (vbo_line_attrib_) {glDeleteBuffers(1, &vbo_line_attrib_);}
//     if (vbo_triangle_attrib_) {glDeleteBuffers(1, &vbo_triangle_attrib_);}
//     if (vbo_cell_attrib_) {glDeleteBuffers(1, &vbo_cell_attrib_);}

//     if (vao_points_) {glDeleteVertexArrays(1, &vao_points_);}
//     if (vao_lines_) {glDeleteVertexArrays(1, &vao_lines_);}
//     if (vao_triangles_) {glDeleteVertexArrays(1, &vao_triangles_);}
//     if (vao_triangles_picking_) {glDeleteVertexArrays(1, &vao_triangles_picking_);}
//     if (vao_cells_) {glDeleteVertexArrays(1, &vao_cells_);}
//     if (vao_cells_outline_) {glDeleteVertexArrays(1, &vao_cells_outline_);}

//     if (ibo_points_) {glDeleteBuffers(1, &ibo_points_);}
//     if (ibo_lines_) {glDeleteBuffers(1, &ibo_lines_);}
//     if (ibo_triangles_) {glDeleteBuffers(1, &ibo_triangles_);}
//     if (ibo_cells_) {glDeleteBuffers(1, &ibo_cells_);}
//     if (ibo_cells_outline_) {glDeleteBuffers(1, &ibo_cells_outline_);}

//     vbo_point_attrib_ = vbo_line_attrib_
//         = vbo_triangle_attrib_ = vbo_cell_attrib_
//         = vao_points_ = vao_lines_ = vao_triangles_
//         = vao_triangles_picking_ = vao_cells_
//         = vao_cells_outline_
//         = ibo_points_ = ibo_lines_ = ibo_triangles_
//         = ibo_cells_ = ibo_cells_outline_ = 0;
// }

// void MeshRenderer::createBuffers()
// {
//     glGenVertexArrays(1, &vao_cells_);
//     glGenBuffers(1, &vbo_cell_attrib_);
//     glGenBuffers(1, &ibo_cells_);

//     glGenVertexArrays(1, &vao_cells_outline_);
//     glGenBuffers(1, &ibo_cells_outline_);

//     glGenVertexArrays(1, &vao_triangles_);
//     glGenBuffers(1, &vbo_triangle_attrib_);
//     glGenBuffers(1, &ibo_triangles_);

//     glGenVertexArrays(1, &vao_triangles_picking_);

//     glGenVertexArrays(1, &vao_lines_);
//     glGenBuffers(1, &vbo_line_attrib_);
//     glGenBuffers(1, &ibo_lines_);

//     glGenVertexArrays(1, &vao_points_);
//     glGenBuffers(1, &vbo_point_attrib_);
//     glGenBuffers(1, &ibo_points_);
// }

// void MeshRenderer::setupVertexAttributes()
// {
//     //---------------
//     // Cells
//     //---------------
//     glBindVertexArray(vao_cells_);
//     glBindBuffer(GL_ARRAY_BUFFER, vbo_cell_attrib_);

//     // -- Attrib 0: Position
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexCellAttrib), (void*)offsetof(VertexCellAttrib, position));
//     glEnableVertexAttribArray(0);

//     // -- Attrib 1: Data
//     glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexCellAttrib), (void*)offsetof(VertexCellAttrib, data));
//     glEnableVertexAttribArray(1);

//     // -- Attrib 2: Cell Incenter
//     glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexCellAttrib), (void*)offsetof(VertexCellAttrib, cell_incenter));
//     glEnableVertexAttribArray(2);

//     // -- Attrib 3: Cell Index
//     glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(VertexCellAttrib), (void*)offsetof(VertexCellAttrib, cell_index));
//     glEnableVertexAttribArray(3);

//     glBindVertexArray(0);

//     //------------------
//     // Cells (Outline)
//     //------------------
//     glBindVertexArray(vao_cells_outline_);
//     glBindBuffer(GL_ARRAY_BUFFER, vbo_cell_attrib_);

//     // -- Attrib 0: Position
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexCellAttrib), (void*)offsetof(VertexCellAttrib, position));
//     glEnableVertexAttribArray(0);

//     // -- Attrib 1: Data
//     glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexCellAttrib), (void*)offsetof(VertexCellAttrib, data));
//     glEnableVertexAttribArray(1);

//     // -- Attrib 2: Cell Incenter
//     glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexCellAttrib), (void*)offsetof(VertexCellAttrib, cell_incenter));
//     glEnableVertexAttribArray(2);

//     glBindVertexArray(0);

//     //---------------
//     // Triangles
//     //---------------
//     glBindVertexArray(vao_triangles_);
//     glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_attrib_);

//     // -- Attrib 0: Position
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTriangleAttrib), (void*)offsetof(VertexTriangleAttrib, position));
//     glEnableVertexAttribArray(0);

//     // -- Attrib 1: Color
//     glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexTriangleAttrib), (void*)offsetof(VertexTriangleAttrib, color));
//     glEnableVertexAttribArray(1);/*

//     // -- Attrib 2: Normal
//     glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTriangleAttrib), (void*)offsetof(VertexTriangleAttrib, normal));
//     glEnableVertexAttribArray(2);

//     // -- Attrib 3: Buffer
//     glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(VertexTriangleAttrib), (void*)offsetof(VertexTriangleAttrib, face_index));
//     glEnableVertexAttribArray(3);*/

//     glBindVertexArray(0);

//     //--------------------------
//     // Triangles (Picking)
//     //--------------------------
//     glBindVertexArray(vao_triangles_picking_);
//     glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_attrib_);

//     // -- Attrib 0: Position
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTriangleAttrib), (void*)offsetof(VertexTriangleAttrib, position));
//     glEnableVertexAttribArray(0);

//     // -- Attrib 1: Index
//     glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(VertexTriangleAttrib), (void*)offsetof(VertexTriangleAttrib, face_index));
//     glEnableVertexAttribArray(1);

//     glBindVertexArray(0);

//     //---------------
//     // Lines
//     //---------------
//     glBindVertexArray(vao_lines_);
//     glBindBuffer(GL_ARRAY_BUFFER, vbo_line_attrib_);

//     // -- Attrib 0: Position
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexLineAttrib), (void*)offsetof(VertexLineAttrib, position));
//     glEnableVertexAttribArray(0);

//     // -- Attrib 1: Color
//     glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexLineAttrib), (void*)offsetof(VertexLineAttrib, color));
//     glEnableVertexAttribArray(1);

//     glBindVertexArray(0);

//     //---------------
//     // Points
//     //---------------
//     glBindVertexArray(vao_points_);
//     glBindBuffer(GL_ARRAY_BUFFER, vbo_point_attrib_);

//     // -- Attrib 0: Position
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPointAttrib), (void*)offsetof(VertexPointAttrib, position));
//     glEnableVertexAttribArray(0);

//     // -- Attrib 1: Color
//     glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexPointAttrib), (void*)offsetof(VertexPointAttrib, color));
//     glEnableVertexAttribArray(1);

//     glBindVertexArray(0);
// }

// void MeshRenderer::updateVertexPoints(
//     const std::vector<VertexPointAttrib>& _p_attribs,
//     const std::vector<GLuint>& _v_point_indices)
// {
//     init();

//     n_points_ = _v_point_indices.size();

//     glBindVertexArray(vao_points_);
//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_points_);
//     glBufferData(GL_ELEMENT_ARRAY_BUFFER, _v_point_indices.size() * sizeof(GLuint), _v_point_indices.data(), GL_DYNAMIC_DRAW);

//     glBindBuffer(GL_ARRAY_BUFFER, vbo_point_attrib_);
//     glBufferData(GL_ARRAY_BUFFER, _p_attribs.size() * sizeof(VertexPointAttrib), _p_attribs.data(), GL_DYNAMIC_DRAW);
//     glBindVertexArray(0);
// }

// void MeshRenderer::updateEdgeLines(
//     const std::vector<VertexLineAttrib>& _l_attribs,
//     const std::vector<GLuint>& _e_line_indices)
// {
//     init();

//     n_lines_     = _e_line_indices.size() / 2;

//     glBindVertexArray(vao_lines_);
//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_lines_);
//     glBufferData(GL_ELEMENT_ARRAY_BUFFER, _e_line_indices.size() * sizeof(GLuint), _e_line_indices.data(), GL_DYNAMIC_DRAW);

//     glBindBuffer(GL_ARRAY_BUFFER, vbo_line_attrib_);
//     glBufferData(GL_ARRAY_BUFFER, _l_attribs.size() * sizeof(VertexLineAttrib), _l_attribs.data(), GL_DYNAMIC_DRAW);
//     glBindVertexArray(0);
// }

// void MeshRenderer::updateFaceTriangles(
//     const std::vector<VertexTriangleAttrib>& _t_attribs,
//     const std::vector<GLuint>& _f_triangle_indices)
// {
//     init();

//     n_face_triangles_ = _f_triangle_indices.size() / 3;

//     glBindVertexArray(vao_triangles_);
//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_triangles_);
//     glBufferData(GL_ELEMENT_ARRAY_BUFFER, _f_triangle_indices.size() * sizeof(GLuint), _f_triangle_indices.data(), GL_DYNAMIC_DRAW);

//     glBindVertexArray(vao_triangles_picking_);
//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_triangles_);
//     glBufferData(GL_ELEMENT_ARRAY_BUFFER, _f_triangle_indices.size() * sizeof(GLuint), _f_triangle_indices.data(), GL_DYNAMIC_DRAW);

//     glBindVertexArray(vao_triangles_);
//     glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_attrib_);
//     glBufferData(GL_ARRAY_BUFFER, _t_attribs.size() * sizeof(VertexTriangleAttrib), _t_attribs.data(), GL_DYNAMIC_DRAW);

//     glBindVertexArray(vao_triangles_picking_);

//     glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_attrib_);
//     glBufferData(GL_ARRAY_BUFFER, _t_attribs.size() * sizeof(VertexTriangleAttrib), _t_attribs.data(), GL_DYNAMIC_DRAW);

//     glBindVertexArray(0);
// }

// void MeshRenderer::updateCellTriangles(
//     const std::vector<VertexCellAttrib>& _c_attribs,
//     const std::vector<GLuint>& _c_triangle_indices,
//     const std::vector<GLuint> &_c_line_indices)
// {

//     init();

//     n_cell_triangles_ = _c_triangle_indices.size() / 3;
//     n_cell_lines_ = _c_line_indices.size() / 2;

//     // Cells
//     glBindVertexArray(vao_cells_);

//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cells_);
//     glBufferData(GL_ELEMENT_ARRAY_BUFFER, _c_triangle_indices.size() * sizeof(GLuint), _c_triangle_indices.data(), GL_DYNAMIC_DRAW);

//     glBindBuffer(GL_ARRAY_BUFFER, vbo_cell_attrib_);
//     glBufferData(GL_ARRAY_BUFFER, _c_attribs.size() * sizeof(VertexCellAttrib), _c_attribs.data(), GL_DYNAMIC_DRAW);

//     // Cells (Outline)
//     glBindVertexArray(vao_cells_outline_);

//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cells_outline_);
//     glBufferData(GL_ELEMENT_ARRAY_BUFFER, _c_line_indices.size() * sizeof(GLuint), _c_line_indices.data(), GL_DYNAMIC_DRAW);

//     glBindVertexArray(0);
// }

// void MeshRenderer::updateData(const Data& data)
// {
//     updateCellTriangles(data.cell_attribs_, data.cell_triangle_indices_, data.cell_line_indices_);
//     updateFaceTriangles(data.triangleAttribs, data.face_triangle_indices_);
//     updateEdgeLines(data.lineAttribs, data.lineIndices);
//     updateVertexPoints(data.pointAttribs, data.pointIndices);

//     glBindVertexArray(0);
// }

// void MeshRenderer::render(const Matrices &m)
// {
//     if (!visible) {return;}

//     // Get Width and Height of Viewport
//     GLint width, height;
//     getViewportSize(width, height);

//     Vec3f clip_box_min = {clip_box_x_[0], clip_box_y_[0], clip_box_z_[0]};
//     Vec3f clip_box_max = {clip_box_x_[1], clip_box_y_[1], clip_box_z_[1]};

//     // Points
//     if (vao_points_ && render_vertices_)
//     {
//         Shader::VERTICES_SHADER.use();

//         color_map_.bind(0);
//         Shader::VERTICES_SHADER.setInt("colormap", 0);

//         Shader::VERTICES_SHADER.setFloat("point_size", point_size_);
//         Shader::VERTICES_SHADER.setVec2f("visible_data_range", vertex_scalar_prop_range_.min_value,vertex_scalar_prop_range_.max_value);
//         Shader::VERTICES_SHADER.setMat4x4f("model_view_projection_matrix", m.model_view_projection_matrix);

//         Shader::VERTICES_SHADER.setInt("data_type", static_cast<int>(vertex_prop_type_));

//         Shader::VERTICES_SHADER.setBool("clip_box_enabled", clip_box_enabled_);
//         Shader::VERTICES_SHADER.setVec3f("clip_box_min", clip_box_min);
//         Shader::VERTICES_SHADER.setVec3f("clip_box_max", clip_box_max);

//         glEnable(GL_POLYGON_OFFSET_FILL);
//         glPolygonOffset(-1.0f, -1.0f); // ensure the vertices are drawn slightly in front

//         glBindVertexArray(vao_points_);
//         glDrawElements(GL_POINTS, 1*n_points_, GL_UNSIGNED_INT, NULL);

//         glDisable(GL_POLYGON_OFFSET_FILL);

//         glBindVertexArray(0);
//     }

//     if (vao_lines_ && render_edges_)
//     {
//         // Lines
//         Shader::EDGES_SHADER.use();

//         Shader::EDGES_SHADER.setMat4x4f("model_view_projection_matrix", m.model_view_projection_matrix);
//         Shader::EDGES_SHADER.setVec2f("inverse_viewport_size", 1.f/width, 1.f/height);

//         Shader::EDGES_SHADER.setFloat("line_width", line_width_);

//         color_map_.bind(0);
//         Shader::EDGES_SHADER.setInt("colormap", 0);
//         Shader::EDGES_SHADER.setVec2f("visible_data_range", edge_scalar_prop_range_.min_value,edge_scalar_prop_range_.max_value);
//         Shader::EDGES_SHADER.setInt("data_type", static_cast<int>(edge_prop_type_));

//         Shader::EDGES_SHADER.setBool("clip_box_enabled", clip_box_enabled_);
//         Shader::EDGES_SHADER.setVec3f("clip_box_min", clip_box_min);
//         Shader::EDGES_SHADER.setVec3f("clip_box_max", clip_box_max);

//         glEnable(GL_POLYGON_OFFSET_FILL);
//         glPolygonOffset(-0.75f, -0.75f); // ensure the lines are drawn slightly in front

//         glBindVertexArray(vao_lines_);
//         glDrawElements(GL_LINES, 2*n_lines_, GL_UNSIGNED_INT, NULL);

//         glDisable(GL_POLYGON_OFFSET_FILL);

//         glBindVertexArray(0);
//     }

//     if (vao_triangles_ && render_faces_)
//     {
//         // Triangles
//         Shader::FACES_SHADER.use();

//         Shader::FACES_SHADER.setMat4x4f("model_view_projection_matrix", m.model_view_projection_matrix);

//         color_map_.bind(0);
//         Shader::FACES_SHADER.setInt("colormap", 0);
//         Shader::FACES_SHADER.setVec2f("visible_data_range", face_scalar_prop_range_.min_value, face_scalar_prop_range_.max_value);
//         Shader::FACES_SHADER.setInt("data_type", static_cast<int>(face_prop_type_));

//         Shader::FACES_SHADER.setBool("clip_box_enabled", clip_box_enabled_);
//         Shader::FACES_SHADER.setVec3f("clip_box_min", clip_box_min);
//         Shader::FACES_SHADER.setVec3f("clip_box_max", clip_box_max);

//         Shader::FACES_SHADER.use();

//         glBindVertexArray(vao_triangles_);

//         glPolygonMode( GL_FRONT_AND_BACK, faces_wireframe_? GL_LINE : GL_FILL );
//         glDrawElements(GL_TRIANGLES, 3*n_face_triangles_, GL_UNSIGNED_INT, NULL);
//         if (faces_wireframe_) {glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );}

//         glBindVertexArray(0);
//     }

//     if (vao_cells_ && render_cells_)
//     {
//         Shader::CELLS_SHADER.use();

//         // General
//         Shader::CELLS_SHADER.setMat4x4f("model_view_projection_matrix", m.model_view_projection_matrix);
//         Shader::CELLS_SHADER.setFloat("cell_scale", cell_scale_);

//         // Prop Vis
//         color_map_.bind(0);
//         Shader::CELLS_SHADER.setInt("colormap", 0);
//         Shader::CELLS_SHADER.setVec2f("visible_data_range", cell_scalar_prop_range_.min_value, cell_scalar_prop_range_.max_value);
//         Shader::CELLS_SHADER.setInt("data_type", static_cast<int>(cell_prop_type_));

//         // Clip Box
//         Shader::CELLS_SHADER.setBool("clip_box_enabled", clip_box_enabled_);
//         Shader::CELLS_SHADER.setVec3f("clip_box_min", clip_box_min);
//         Shader::CELLS_SHADER.setVec3f("clip_box_max", clip_box_max);

//         // Draw
//         glBindVertexArray(vao_cells_);

//         glPolygonMode( GL_FRONT_AND_BACK, cells_wireframe_? GL_LINE : GL_FILL );
//         glDrawElements(GL_TRIANGLES, 3*n_cell_triangles_, GL_UNSIGNED_INT, NULL);
//         if (cells_wireframe_) {glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );}

//         glBindVertexArray(0);

//         // Cells Outline
//         Shader::CELLS_OUTLINE_SHADER.use();

//         // General
//         Shader::CELLS_OUTLINE_SHADER.setMat4x4f("model_view_projection_matrix", m.model_view_projection_matrix);
//         Shader::CELLS_OUTLINE_SHADER.setFloat("cell_scale", cell_scale_);
//         Shader::CELLS_OUTLINE_SHADER.setVec4f("color", cells_outline_color_);

//         // Prop Vis
//         Shader::CELLS_OUTLINE_SHADER.setVec2f("visible_data_range", cell_scalar_prop_range_.min_value, cell_scalar_prop_range_.max_value);
//         Shader::CELLS_OUTLINE_SHADER.setInt("data_type", static_cast<int>(cell_prop_type_));

//         // Clip Box
//         Shader::CELLS_OUTLINE_SHADER.setBool("clip_box_enabled", clip_box_enabled_);
//         Shader::CELLS_OUTLINE_SHADER.setVec3f("clip_box_min", clip_box_min);
//         Shader::CELLS_OUTLINE_SHADER.setVec3f("clip_box_max", clip_box_max);

//         // Draw
//         glEnable(GL_POLYGON_OFFSET_FILL);
//         glPolygonOffset(-0.75f, -0.75f);

//         glBindVertexArray(vao_cells_outline_);
//         glDrawElements(GL_LINES, 2*n_cell_lines_, GL_UNSIGNED_INT, NULL);

//         glDisable(GL_POLYGON_OFFSET_FILL);

//         glBindVertexArray(0);
//     }

// }

// void MeshRenderer::renderPicking(const Matrices& m, int id)
// {
//     if (!visible) {return;}

//     Shader::PICKING_SHADER.use();

//     Shader::PICKING_SHADER.setMat4x4f("model_view_projection_matrix", m.model_view_projection_matrix);
//     Shader::PICKING_SHADER.setUInt("object_index", id);

//     glBindVertexArray(vao_triangles_picking_);

//     glDrawElements(GL_TRIANGLES, 3*n_face_triangles_, GL_UNSIGNED_INT, NULL);

//     glBindVertexArray(0);
// }

// }
