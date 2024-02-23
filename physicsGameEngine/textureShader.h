
#ifndef TEXTURE_SHADER_H
#define TEXTURE_SHADER_H

#include "shaderProgram.h"
#include "vector3D.h"
#include "drawingUtil.h"
#include "shaderInterface.h"

const std::string textureVertexShader = R"(
    #version 330 core

    layout(location = 0) in vec3 aPos;
    layout(location = 1) in vec3 aNormal;
    layout(location = 2) in vec2 aTexCoord; // Texture coordinates

    // Passes the fragment position to the fragment shader
    out vec3 FragPos;
    // Passes the normal to the fragment shader
    out vec3 Normal;
    // Passes texture coordinates to the fragment shader
    out vec2 TexCoord;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main(){
        FragPos = vec3(model * vec4(aPos, 1.0));
        Normal = mat3(transpose(inverse(model))) * aNormal;
        TexCoord = aTexCoord;
        gl_Position = projection * view * vec4(FragPos, 1.0);
    }
)";

const std::string textureFragmentShader = R"(
    #version 330 core

    // Received from the vertex shader
    in vec3 FragPos;
    // Received from the vertex shader
    in vec3 Normal;
    // Received texture coordinates from the vertex shader
    in vec2 TexCoord;

    uniform sampler2D textureSampler; // Texture sampler

    // Maximum number of lights
    #define MAX_LIGHTS 10

    uniform vec3 lightPos[MAX_LIGHTS];
    uniform vec4 lightColors[MAX_LIGHTS];

    // Number of active lights
    uniform int numActiveLights;

    out vec4 FragColor;

    void main(){
        vec3 finalDiffuse = vec3(0.0);

        for (int i = 0; i < numActiveLights; ++i) {
            vec3 lightDir = normalize(lightPos[i] - FragPos);
            float diff =  max(dot(Normal, lightDir), 0.0);
            finalDiffuse += diff;
        }

        // Sample texture color using texture coordinates
        vec4 texColor = texture(textureSampler, TexCoord);
        FragColor = texColor * vec4(finalDiffuse, 1.0);
    }
)";

namespace pe {

    class TextureShader {

    private:

        // Coded into the shader, as it must be known at compile time
        static constexpr int MAXIMUM_NUMBER_OF_LIGHT_SOURCES = 10;

        ShaderProgram shaderProgramObject;

    public:

        TextureShader() : shaderProgramObject(
            textureVertexShader,
            textureFragmentShader
        ) {}

        void drawFaces(
            const std::vector<glm::vec3>& faces,
            const std::vector<glm::vec3>& normals,
            const std::vector<glm::vec2>& texCoords,
            const glm::mat4& model,
            const glm::mat4& view,
            const glm::mat4& projection,
            GLuint textureID, // Texture ID
            int activeLightSources,
            glm::vec3* lightSourcesPosition,
            glm::vec4* lightSourcesColor
        );
    };
}

#endif
