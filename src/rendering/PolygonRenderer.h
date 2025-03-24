// #ifndef POLYGONRENDERER_H
// #define POLYGONRENDERER_H

// #include "PointsRenderBatch.h"
// namespace MV
// {

// template<size_t N>
// struct Polygon
// {
//     using VBO = VBO<GL_FLOAT, float, 3, 3, 3, 1>; // position, color, normal, index

//     Point vertices[N];
//     float normal[3];

//     inline void buffer(VBO& vbo, uint idx) const
//     {
//         for (unsigned char i = 0; i < N; ++i)
//         {
//             std::array<float, 10> data = {
//                 vertices[i].position[0],vertices[i].position[1],vertices[i].position[2],
//                 vertices[i].color[0],vertices[i].color[1],vertices[i].color[2],
//                 normal[0], normal[1], normal[2],
//                 (float)(idx+1)
//             };
//             vbo.set(N*idx+i, data);
//         }
//     }
// };
// //using Triangle = Polygon<3>;
// using Quad = Polygon<4>;
// using Pentagon = Polygon<5>;

// template<size_t N>
// class PolygonRenderer
// {
// private:
//     std::unordered_set<uint> updated;
//     std::unordered_set<uint> free;

//     Polygon<N>::VBO vbo;
//     IBO<GL_TRIANGLES> ibo;
//     VAO vao;
//     IBO<GL_LINES> ibo_outlines;
//     VAO vao_outlines;
//     VAO vao_picking;

// public:
//     PolygonRenderer(size_t max_num_elements);

//     void render();

//     void renderPicking(const glm::mat4x4& mvp, const uint batch_idx);

//     int add(const Polygon<N>& p);

//     void remove(uint idx);
// };

// //using TriangleRenderer = PolygonRenderer<3>;
// using QuadRenderer = PolygonRenderer<4>;

// }

// #endif // POLYGONRENDERER_H
