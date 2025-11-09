#include "AxoPlotl/geometry/Octree.h"
#include "gtest/gtest.h"

namespace AxoPlotl
{

TEST(HelloWorldTest, OnePlusOneIsTwo)
{
    ASSERT_EQ(1+1, 2);
}

TEST(MortonCodeTest, Test1)
{
    uint32_t x = 4, y = 2, z = 7, depth = 5;
    Geometry::OctreeNodeCode code = Geometry::encode(x, y, z, depth);
    uint32_t dx, dy, dz, d;
    Geometry::decode(code, dx, dy, dz, d);

    ASSERT_EQ(x, dx);
    ASSERT_EQ(y, dy);
    ASSERT_EQ(z, dz);
    ASSERT_EQ(depth, d);
}

TEST(OctreeTest, Test1)
{
    // Octree octree(AABB{-1,2,-1,2,-1,2}, 2, 5);
    // ASSERT_EQ(8, octree.numNodes());
    // ASSERT_EQ(0, octree.findNodeIndexAtPoint(-0.2, -0.2, -0.2));
    // octree.refineNode(0);
    // ASSERT_EQ(16, octree.numNodes());
    // ASSERT_EQ(15, octree.findNodeIndexAtPoint(-0.2, -0.2, -0.2));
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
}
