// #include "RenderBatchTetFaces.h"
// #include "../utils/Globals.h"

// namespace MV
// {

// void RenderBatchTetFaces::initFromMesh(TetrahedralMesh& mesh)
// {
//     uint nTriangles = mesh_n_boundary_faces(mesh);
//     const uint nVertices = 3*nTriangles;
//     uint nIndices = nVertices;

//     vao.generateNew();
//     vbo.generateNew(nVertices);
//     ibo.generateNew(nIndices);

//     // create the vertex data
//     auto prop = mesh.request_face_property<int>("AlgoHex::FeatureFaces");
//     int i = 0;
//     for (auto f_it = mesh.f_iter(); f_it.is_valid(); ++f_it)
//     {
//         auto fh = *f_it;
//         if (!mesh.is_boundary(fh)) continue;
//         auto hfh = fh.halfface_handle(0); if (!mesh.incident_cell(hfh).is_valid()) hfh = hfh.opposite_handle();
//         auto normal = mesh.normal(hfh);
//         auto vhs = mesh.get_halfface_vertices(fh.halfface_handle(0));

//         Color col = COLORS[prop[fh]%COLORS.size()];

//         auto d0 = toArray<float,10>(mesh.vertex(vhs[0]), col, normal, fh.idx()+1);
//         auto d1 = toArray<float,10>(mesh.vertex(vhs[1]), col, normal, fh.idx()+1);
//         auto d2 = toArray<float,10>(mesh.vertex(vhs[2]), col, normal, fh.idx()+1);

//         setFace(i++, std::vector({d0, d1, d2}));
//     }

//     // GENERATE OUTLINES
//     vao_outlines.generateNew();
//     vbo.defineAttributes({0});

//     nIndices = 6*nTriangles;
//     std::vector<GLuint> indices_outlines(nIndices);
//     for (int i = 0; i < nTriangles; ++i)
//     {
//         indices_outlines[6*i+0] = 3*i+0;
//         indices_outlines[6*i+1] = 3*i+1;
//         indices_outlines[6*i+2] = 3*i+1;
//         indices_outlines[6*i+3] = 3*i+2;
//         indices_outlines[6*i+4] = 3*i+2;
//         indices_outlines[6*i+5] = 3*i+0;
//     };
//     ibo_outlines.generateNew(indices_outlines);

//     // Generate picking buffers
//     vao_picking.generateNew();
//     vbo.defineAttributes({0,3});
// }

// void RenderBatchTetFaces::render()
// {
//     Shader::FACES_SHADER.use();

//     // only rebuffer part of data that has changed
//     vbo.bind();
//     while (!updatedFaces.empty())
//     {
//         int i = updatedFaces.extract(updatedFaces.begin()).value(); // pop
//         vbo.bufferSubData(3*i, 3);
//     }
//     vao.bind();
//     vbo.enableAttributes();

//     ibo.draw();

//     vbo.disableAttributes();
//     vao.unbind();

//     // Outlines

//     Shader::FACES_OUTLINES_SHADER.use();

//     glEnable(GL_POLYGON_OFFSET_FILL);
//     glPolygonOffset(-0.75f, -0.75f); // ensure the outline is drawn slightly in front

//     vbo.bind();
//     vao_outlines.bind();
//     vbo.enableAttributes({0});

//     ibo_outlines.draw();

//     vbo.disableAttributes();
//     vao_outlines.unbind();

//     glDisable(GL_POLYGON_OFFSET_FILL);

// }

// void RenderBatchTetFaces::renderPicking()
// {
//     Shader::PICKING_SHADER.use();

//     vbo.bind();
//     vao_picking.bind();
//     vbo.enableAttributes({0,3});

//     ibo.draw();

//     vbo.disableAttributes();
//     vao_picking.unbind();

//     return;
// }

// }
