// #include "PolygonRenderer.h"
// #include "../commons/Shader.h"

// namespace MV
// {

// template<size_t N>
// PolygonRenderer<N>::PolygonRenderer(size_t max_num_elements)
// {
//     assert(N >= 3);
//     if (N < 3) return;

//     size_t max_num_vertices = N * max_num_elements;
//     size_t max_num_triangles = (N - 2) * max_num_elements;

//     vao.generateNew();
//     vbo.generateNew(max_num_vertices);

//     // Take care with the indices. N Vertices = 1 Polygon = (N - 2) Triangles
//     uint num_indices = 3*max_num_triangles;
//     std::vector<uint> indices; indices.reserve(num_indices);
//     for (uint i = 0; i < max_num_elements; ++i)
//     {
//         for (uint j = 1; j < N - 1; ++j)
//         {
//             indices.push_back(N*i+0);
//             indices.push_back(N*i+j);
//             indices.push_back(N*i+j+1);
//         }
//     }
//     ibo.generateNew(indices);

//     // Mark every slot as free initially
//     for (uint i = 0; i < max_num_elements; ++i) {free.insert(i);}

//     vao.unbind();

//     // Generate Outlines - The Polygon Edges
//     vao_outlines.generateNew();
//     vbo.defineAttributes({0});
//     num_indices = 2*N*max_num_elements;
//     indices.clear();
//     indices.resize(num_indices);
//     for (uint i = 0; i < max_num_elements; ++i)
//     {
//         for (uint j = 0; j < N; ++j)
//         {
//             indices[2*N*i + 2*j+0] = N*i+j;
//             indices[2*N*i + 2*j+1] = N*i+(j+1)%N;
//         }
//     };
//     ibo_outlines.generateNew(indices);

//     // Generate picking buffers
//     vao_picking.generateNew();
//     vbo.defineAttributes({0,3});
// }

// template<size_t N>
// void PolygonRenderer<N>::render()
// {
//     Shader::FACES_SHADER.use();

//     // only rebuffer part of data that has changed
//     vbo.bind();
//     while (!updated.empty())
//     {
//         uint i = updated.extract(updated.begin()).value(); // pop
//         vbo.bufferSubData(N*i, N);
//     }
//     vao.bind();
//     vbo.enableAttributes();

//     ibo.drawAll();

//     vbo.disableAttributes();
//     vao.unbind();

//     // Outlines

//     Shader::FACES_OUTLINES_SHADER.use();

//     glEnable(GL_POLYGON_OFFSET_FILL);
//     glPolygonOffset(-0.75f, -0.75f); // ensure the outline is drawn slightly in front

//     vbo.bind();
//     vao_outlines.bind();
//     vbo.enableAttributes({0});

//     ibo_outlines.drawAll();

//     vbo.disableAttributes();
//     vao_outlines.unbind();

//     glDisable(GL_POLYGON_OFFSET_FILL);
// }

// template<size_t N>
// void PolygonRenderer<N>::renderPicking(const glm::mat4x4& mvp, const uint batch_idx)
// {
//     Shader::PICKING_SHADER.use();

//     Shader::PICKING_SHADER.setMat4x4f("model_view_projection_matrix", mvp);
//     Shader::PICKING_SHADER.setUInt("mesh_index", batch_idx+1);

//     vbo.bind();
//     vao_picking.bind();
//     vbo.enableAttributes({0,3}); // position and index

//     ibo.drawAll();

//     vbo.disableAttributes();
//     vao_picking.unbind();
// }

// template<size_t N>
// int PolygonRenderer<N>::add(const Polygon<N>& p)
// {
//     // If the batch is full, return -1
//     if (free.empty()) return -1;

//     // Retrieve the first free index
//     uint idx = *free.begin();
//     free.erase(free.begin());

//     // Add the index to the updated list
//     updated.insert(idx);

//     // Store the data in the vbo
//     p.buffer(vbo, idx);

//     // Return the index
//     return idx;
// }

// template<size_t N>
// void PolygonRenderer<N>::remove(uint idx)
// {
//     // TODO
// }

// template class PolygonRenderer<3>;
// template class PolygonRenderer<4>;
// template class PolygonRenderer<5>;
// template class PolygonRenderer<6>;

// }
