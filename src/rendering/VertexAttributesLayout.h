#ifndef VERTEXATTRIBUTESLAYOUT_H
#define VERTEXATTRIBUTESLAYOUT_H

#include <cstddef>
#include <vector>
#include "../utils/gl.h"

namespace MV
{

struct BaseVertexAttributesLayout
{
public:
    BaseVertexAttributesLayout() = default;
    inline virtual size_t attrOffset(size_t i) = 0;
    inline virtual size_t attrOffsetBytes(size_t i) = 0;
    inline virtual size_t attrSize(size_t i) = 0;
    inline virtual size_t attrSizeBytes(size_t i) = 0;
    inline virtual size_t numAttrs() = 0;
    inline virtual size_t totalSize() = 0;
    inline virtual size_t totalSizeBytes() = 0;
    inline virtual size_t typeSizeBytes() = 0;
    inline virtual GLenum type() = 0;
};

template <GLenum GLT, typename T, size_t... S>
struct VertexAttributesLayout : BaseVertexAttributesLayout
{
public:
    VertexAttributesLayout() : sizes{S...}
    {
        offsets.resize(sizes.size());
        for (int i = 1; i < offsets.size(); ++i) offsets[i] = offsets[i-1] + sizes[i-1];
        size = offsets[offsets.size()-1] + sizes[offsets.size()-1];
    }

    ~VertexAttributesLayout() {}
    inline size_t attrOffset(size_t i) override {return offsets[i];}
    inline size_t attrOffsetBytes(size_t i) override {return sizeof(T)*offsets[i];}
    inline size_t attrSize(size_t i) override {return sizes[i];}
    inline size_t attrSizeBytes(size_t i) override {return sizeof(T)*sizes[i];}
    inline size_t numAttrs() override {return sizes.size();}
    inline size_t totalSize() override {return size;}
    inline size_t totalSizeBytes() override {return sizeof(T) * size;}
    inline size_t typeSizeBytes() override {return sizeof(T);}
    inline GLenum type() override {return GLT;}
private:
    std::vector<size_t> sizes;
    std::vector<size_t> offsets;
    size_t size;
};
using VAL343f = VertexAttributesLayout<GL_FLOAT, float, 3, 4, 3>; // e.g. position, color, normal
using VAL3f = VertexAttributesLayout<GL_FLOAT, float, 3, 4>; // e.g. position, color
}

#endif // VERTEXATTRIBUTESLAYOUT_H
