/*
    This is the header file for a class representing a simple shader
    used for rendering the depth map of a scene. This shader is simple
    enough; for instance, there is no fragment output, it just transforms
    the input vertex.
    The projection and view matrices input as uniforms must belong to the
    light (together making up the lightSpace matrix of the light).
    The model matrix is just the transform matrix of the shapes as usual.

*/

#ifndef SIMPLE_SHADER_H
#define SIMPLE_SHADER_H

#include "shader.h"

namespace pe {

    class SimpleShader : public Shader {

    public:

        SimpleShader() : Shader(
            "simpleVertexShader.glsl",
            "simpleFragmentShader.glsl"
        ) {}

    };
}

#endif