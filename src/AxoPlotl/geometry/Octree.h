#pragma once

#include <cassert>
#include <cstdint>
#include <unordered_map>
#include <vector>
#include <array>

namespace AxoPlotl
{

#define EPS 1e-12

#define NUM_NODE_CHILDREN 8
#define NUM_NODE_NEIGHBORS 6

#define NUM_DEPTH_BITS 6
#define NUM_MORTON_BITS 58

using u8 = uint8_t;
using u32 = uint32_t;
using u64 = uint64_t;

struct AABB {
    double x0, x1, y0, y1, z0, z1;

    inline std::array<double,3> center() const {
        return {
            0.5 * (x0 + x1),
            0.5 * (y0 + y1),
            0.5 * (z0 + z1)
        };
    }

    inline std::array<double,3> size() const {
        return {x1-x0,y1-y0,z1-z0};
    }

    template<typename Vec3T>
    inline void compute(const std::vector<Vec3T>& pts) {
        x0 = y0 = z0 = +std::numeric_limits<double>::infinity();
        x1 = y1 = z1 = -std::numeric_limits<double>::infinity();
        for (u32 i = 0; i < pts.size(); ++i) {
            x0 = min(x0, pts[i][0]);
            x1 = max(x1, pts[i][0]);
            y0 = min(y0, pts[i][1]);
            y1 = max(y1, pts[i][1]);
            z0 = min(z0, pts[i][2]);
            z1 = max(z1, pts[i][2]);
        }
    }

    template<typename Vec3T>
    inline std::array<Vec3T,8> corners() const {
        return {
            Vec3T(x0, y0, z0),
            Vec3T(x0, y0, z1),
            Vec3T(x1, y0, z1),
            Vec3T(x1, y0, z0),

            Vec3T(x0, y1, z0),
            Vec3T(x1, y1, z0),
            Vec3T(x1, y1, z1),
            Vec3T(x0, y1, z1)
        };
    }
};

// Encodes position and depth
using OctreeNodeCode = u64;

inline u64 expandBits(u32 v) {
    v = (v | (v << 16)) & 0x030000FF;
    v = (v | (v << 8)) & 0x0300F00F;
    v = (v | (v << 4)) & 0x030C30C3;
    v = (v | (v << 2)) & 0x09249249;
    return v;
}

inline OctreeNodeCode encode(u32 x, u32 y, u32 z, u32 depth) {
    assert(depth < (1u << NUM_DEPTH_BITS));

    u64 morton = (expandBits(x) << 0)
                 | (expandBits(y) << 1)
                 | (expandBits(z) << 2);
    return ((u64)(depth) << NUM_MORTON_BITS)
                    | (morton & ((1ULL << NUM_MORTON_BITS) - 1));
}

inline u32 compactBits(u64 v) {
    v &= 0x09249249;
    v = (v ^ (v >> 2)) & 0x030C30C3;
    v = (v ^ (v >> 4)) & 0x0300F00F;
    v = (v ^ (v >> 8)) & 0x030000FF;
    v = (v ^ (v >> 16)) & 0x000003FF;
    return (u32)v;
}

inline void decode(OctreeNodeCode code, u32& x, u32& y, u32& z, u32& depth) {
    u64 morton = code & ((1ULL << NUM_MORTON_BITS) - 1);
    x = compactBits(morton >> 0);
    y = compactBits(morton >> 1);
    z = compactBits(morton >> 2);
    depth = (u32)(code >> NUM_MORTON_BITS);
}

struct OctreeNode {
    OctreeNodeCode code;
    bool refined;
    std::array<u32, NUM_NODE_CHILDREN> children = {}; // UINT32_MAX means no child

    OctreeNode(OctreeNodeCode code) : code(code), refined(false) {
        children.fill(UINT32_MAX);
    }

};

class Octree
{
private:
    u32 maxDepth = 5;
    u32 initialResolution = 2;
    std::vector<OctreeNode> nodes;
    std::unordered_map<OctreeNodeCode, u32> codeIndexMap;
    AABB bounds;

public:
    Octree(AABB bounds, u32 initialResolution = 2, u32 maxDepth = 5)
        : bounds(bounds), maxDepth(maxDepth),
        initialResolution(initialResolution)
    {
        assert((initialResolution << maxDepth) <= 1024);

        // Initialize root nodes
        u32 capacity = pow(initialResolution,3);
        nodes.reserve(capacity);
        codeIndexMap.reserve(capacity);
        for (u32 z = 0; z < initialResolution; ++z) {
            for (u32 y = 0; y < initialResolution; ++y) {
                for (u32 x = 0; x < initialResolution; ++x) {
                    OctreeNodeCode code = encode(x, y, z, 0);
                    nodes.emplace_back(code);
                    codeIndexMap[code] = nodes.size()-1;
                }
            }
        }
    }

    inline bool containsNode(OctreeNodeCode code) const {
        return codeIndexMap.find(code) != codeIndexMap.end();
    }

    inline double getNodeScaleAtDepth(u32 depth) const {
        return 1.0 / (initialResolution << depth); // resolution * 2^depth
    }

    inline std::array<double,3> getNodeSizeAtDepth(u32 depth) const {
        double scale = getNodeScaleAtDepth(depth);
        auto treeSize = bounds.size();
        return {
            scale * treeSize[0],
            scale * treeSize[1],
            scale * treeSize[2]
        };
    }

    inline const u32 getNodeIndex(OctreeNodeCode code) const {
        auto it = codeIndexMap.find(code);
        return (it != codeIndexMap.end())? it->second : UINT32_MAX;
    }

    inline u32 numNodes() const {
        return nodes.size();
    }

    /// Splits a Node into 8 children
    void refineNode(u32 idx);

    u32 findNodeIndex(double px, double py, double pz) const;

    AABB getNodeBounds(u32 idx) const;
};

}

