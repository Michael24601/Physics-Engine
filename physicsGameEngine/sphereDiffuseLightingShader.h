/*
    In order to simulate the collision of a sphere, we tessalate
    the sphere (turn it to a polyhedra with several faces).
    We can use the shader we use for solid shapes on these faces
    but we face an issue:
        - The sphere won't look smooth, even if we really increase
        the number of faces.
    As such, while we can use the regular diffuse lighting shader
    on the faces of the sphere, It is better to use this shader,
    which will create a smoother surface (because it calculates normals
    programatically, for each point, taking advantage of the fact that this
    is a sphere).
    Another thing to consider is that we on't necessarily be taking
    the faces used in the sphere object (the ones used for collision).
    We may want to save on performance, and instead have less faces used
    in the sphere object for collision, and more used in the graphics.
*/

#ifndef SPHERE_DIFFUSE_LIGHTING_SHADER_H
#define SPHERE_DIFFUSE_LIGHTING_SHADER_H

#include "shaderProgram.h"
#include "vector3D.h"
#include "drawingUtil.h"


/*
    The vertex positions are received, but not their normals, because in
    a sphere, normals are always the vector form the center to the vertex,
    so it can be derived from the position.
*/
const std::string sphereDiffuseLightingVertexShader = R"(
    #version 330 core

    layout(location = 0) in vec3 aPos;  // Input attribute for sphere vertices

    out vec3 FragPos;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    uniform vec3 sphereCenter;
    uniform float sphereRadius;

    void main() {
        vec3 spherePos = aPos;

        gl_Position = projection * view * model * vec4(spherePos, 1.0);
        FragPos = spherePos;
    }
)";


const std::string sphereDiffuseLightingFragmentShader = R"(
    #version 330 core

    in vec3 FragPos;

    uniform vec4 objectColor;

    // Maximum number of lights
    #define MAX_LIGHTS 10

    uniform vec3 lightPos[MAX_LIGHTS];
    uniform vec4 lightColors[MAX_LIGHTS];

    // Number of active lights
    uniform int numActiveLights;

    // Center and radius of the sphere
    uniform vec3 sphereCenter;
    uniform float sphereRadius;

    out vec4 FragColor;

    void main(){
        // Calculate the sphere's normal based on its center and radius
        vec3 normal = normalize(FragPos - sphereCenter);

        vec3 finalDiffuse = vec3(0.0);

        for (int i = 0; i < numActiveLights; ++i) {
            vec3 lightDir = normalize(lightPos[i] - FragPos);
            float diff = max(dot(normal, lightDir), 0.0);
            vec3 diffuse = objectColor.rgb * diff * lightColors[i].rgb;
            finalDiffuse += diffuse;
        }

        FragColor = vec4(finalDiffuse, objectColor.a);
    }
)";

namespace pe {

    class SphereDiffuseLightingShader {

    private:

        // Coded into the shader, as it must be known at compile time
        static constexpr int MAXIMUM_NUMBER_OF_LIGHT_SOURCES = 10;

        ShaderProgram shaderProgramObject;

    public:

        SphereDiffuseLightingShader() : shaderProgramObject(
            sphereDiffuseLightingVertexShader,
            sphereDiffuseLightingFragmentShader) {}

        void drawFace(
            const std::vector<std::vector<Vector3D>>& faces,
            const glm::vec3& centerPosition,
            real radius,
            const glm::mat4& model,
            const glm::mat4& view,
            const glm::mat4& projection,
            const glm::vec4& objectColor,
            int activeLightSources,
            glm::vec3* lightSourcesPosition,
            glm::vec4* lightSourcesColor
        );
    };
}

#endif