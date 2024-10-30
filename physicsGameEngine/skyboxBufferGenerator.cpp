
#include "skyboxBufferGenerator.h"

using namespace pe;

VertexBuffer pe::createSkyboxVertexBuffer() {
    return VertexBuffer(
        108,
        std::vector<unsigned int>{3},
        3,
        GL_STATIC_DRAW
    );
}