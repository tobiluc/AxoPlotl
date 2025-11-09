#include "Octree.h"

namespace AxoPlotl::Geometry
{

void Octree::refineNode(u32 idx) {
    if (idx == UINT32_MAX) {return;}

    OctreeNodeCode code = nodes[idx].code;
    u32 x, y, z, depth;
    decode(code, x, y, z, depth);
    if (depth >= maxDepth) {return;}

    u32 childDepth = depth + 1;
    for (int i = 0; i < NUM_NODE_CHILDREN; ++i) {
        u32 cx = x * 2 + ((i >> 0) & 1);
        u32 cy = y * 2 + ((i >> 1) & 1);
        u32 cz = z * 2 + ((i >> 2) & 1);
        OctreeNodeCode childCode = encode(cx, cy, cz, childDepth);
        nodes.emplace_back(childCode);
        u32 childIdx = nodes.size()-1;
        nodes[idx].children[i] = childIdx;
        codeIndexMap[childCode] = childIdx;
    }
    nodes[idx].refined = true;
}

u32 Octree::findNodeIndex(double px, double py, double pz) const {

    // Normalize within tree bounds
    px = (px - bounds.x0) / (bounds.x1 - bounds.x0);
    py = (py - bounds.y0) / (bounds.y1 - bounds.y0);
    pz = (pz - bounds.z0) / (bounds.z1 - bounds.z0);

    // Check if outside tree bounds
    if (px < -EPS || px > 1.0+EPS
        || py < -EPS || py > 1.0+EPS
        || pz < -EPS || pz > 1.0+EPS) {
        return UINT32_MAX;
    }

    // Clamp
    px = std::clamp(px, 0.0, 1.0);
    py = std::clamp(py, 0.0, 1.0);
    pz = std::clamp(pz, 0.0, 1.0);

    // Find which root cell this falls into
    u32 x = (u32)(px * initialResolution);
    u32 y = (u32)(py * initialResolution);
    u32 z = (u32)(pz * initialResolution);

    OctreeNodeCode code = encode(x, y, z, 0);
    u32 nodeIdx = getNodeIndex(code);

    if (nodeIdx == UINT32_MAX) {return UINT32_MAX;}

    // Descend as far as possible
    while (true) {
        const OctreeNode& node = nodes[nodeIdx];

        u32 cx, cy, cz, depth;
        decode(node.code, cx, cy, cz, depth);

        if (depth >= maxDepth) {break;}

        // Compute the relative position of the point in this node
        double scale = getNodeScaleAtDepth(depth);
        double localX = (px - cx * scale) / scale;
        double localY = (py - cy * scale) / scale;
        double localZ = (pz - cz * scale) / scale;

        u32 childIndex =
            ((localZ >= 0.5) << 2) |
            ((localY >= 0.5) << 1) |
            ((localX >= 0.5) << 0);

        u32 childIdx = node.children[childIndex];
        if (childIdx == UINT32_MAX) {break;}

        nodeIdx = childIdx;
    }

    return nodeIdx;
}

AABB Octree::getNodeBounds(u32 idx) const {
    if (idx == UINT32_MAX) {return {0};}
    u32 cx, cy, cz, d;
    decode(nodes[idx].code, cx, cy, cz, d);
    auto size = getNodeSizeAtDepth(d);
    double x0 = bounds.x0 + cx * size[0];
    double y0 = bounds.y0 + cy * size[1];
    double z0 = bounds.z0 + cz * size[2];
    return AABB{x0,x0+size[0],y0,y0+size[1],z0,z0+size[2]};
}

}
