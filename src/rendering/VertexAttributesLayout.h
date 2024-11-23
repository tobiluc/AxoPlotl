#ifndef VERTEXATTRIBUTESLAYOUT_H
#define VERTEXATTRIBUTESLAYOUT_H

#include <cstddef>
#include <vector>
#include "../utils/gl.h"

namespace MV
{

template <GLenum GLT, typename T, size_t... S>
struct VertexAttributesLayout
{
public:
    VertexAttributesLayout() : sizes{S...}
    {
        offsets.resize(sizes.size());
        for (int i = 1; i < offsets.size(); ++i) offsets[i] = offsets[i-1] + sizes[i-1];
        size = offsets[offsets.size()-1] + sizes[offsets.size()-1];
    }

    ~VertexAttributesLayout() {}
    inline size_t attrOffset(size_t i) {return offsets[i];}
    inline size_t attrOffsetBytes(size_t i) {return sizeof(T)*offsets[i];}
    inline size_t attrSize(size_t i) {return sizes[i];}
    inline size_t attrSizeBytes(size_t i) {return sizeof(T)*sizes[i];}
    inline size_t numAttrs() {return sizes.size();}
    inline size_t totalSize() {return size;}
    inline size_t totalSizeBytes() {return sizeof(T) * size;}
    inline GLenum type() {return GLT;}
private:
    std::vector<size_t> sizes;
    std::vector<size_t> offsets;
    size_t size;
};
using VAL343f = VertexAttributesLayout<GL_FLOAT, float, 3, 4, 3>; // e.g. position, color, normal
using VAL3f = VertexAttributesLayout<GL_FLOAT, float, 3, 4>; // e.g. position, color
}

#endif // VERTEXATTRIBUTESLAYOUT_H
