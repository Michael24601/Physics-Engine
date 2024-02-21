

#ifndef COOK_TORRANCE_TEXTURE_SHADER_H
#define COOK_TORRANCE_TEXTURE_SHADER_H

#include "shaderProgram.h"
#include "vector3D.h"
#include "drawingUtil.h"
#include "shaderInterface.h"


const std::string cookTorranceTextureVertexShader = R"(
    #version 330 core

    layout(location = 0) in vec3 aPos;
    layout(location = 1) in vec3 aNormal;
    layout(location = 2) in vec2 aTexCoord; // Texture coordinates

    out vec3 FragPos;
    out vec3 Normal;
    out vec2 TexCoord; // Interpolated texture coordinates to pass to fragment shader

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main(){
        FragPos = vec3(model * vec4(aPos, 1.0));
        Normal = mat3(transpose(inverse(model))) * aNormal;
        TexCoord = aTexCoord; // Pass texture coordinates to fragment shader
        gl_Position = projection * view * vec4(FragPos, 1.0);
    }
)";


const std::string cookTorranceTextureFragmentShader = R"(
    #version 330 core

    in vec3 FragPos;
    in vec3 Normal;
    in vec2 TexCoord; // Interpolated texture coordinates from vertex shader

    uniform sampler2D textureSampler; // Texture sampler

    #define MAX_LIGHTS 10

    uniform vec3 lightPos[MAX_LIGHTS];
    uniform vec4 lightColors[MAX_LIGHTS];

    uniform int numActiveLights;

    uniform vec3 viewPos;  
    uniform float roughness;  
    uniform float fresnel;    

    out vec4 FragColor;

    void main(){
        vec3 finalDiffuse = vec3(0.0);
        vec3 finalSpecular = vec3(0.0);

        for (int i = 0; i < numActiveLights; ++i) {
            vec3 lightDir = normalize(lightPos[i] - FragPos);
            float diff = max(dot(Normal, lightDir), 0.0);
            vec3 diffuse = texture(textureSampler, TexCoord).rgb * diff; // Sample texture for diffuse color
            finalDiffuse += diffuse;

            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 halfwayDir = normalize(lightDir + viewDir);
            float NdotH = max(dot(Normal, halfwayDir), 0.0);
            float D = (roughness * roughness) / (3.14159265359 * pow((NdotH * NdotH * (roughness * roughness - 1.0) + 1.0), 2.0));
            float G = min(1.0, min(2.0 * NdotH * dot(Normal, viewDir) / dot(viewDir, halfwayDir), 2.0 * NdotH * dot(Normal, lightDir) / dot(lightDir, halfwayDir)));
            float F = fresnel + (1.0 - fresnel) * pow(1.0 - NdotH, 5.0);

            vec3 specular = lightColors[i].rgb * (D * G * F) / (4.0 * dot(Normal, viewDir) * dot(Normal, lightDir));
            finalSpecular += specular;
        }

        vec3 ambientColor = 0.1 * texture(textureSampler, TexCoord).rgb; // Sample texture for ambient color
        vec3 resultColor = finalDiffuse + finalSpecular + ambientColor;

        FragColor = vec4(resultColor, 1.0); // Set alpha to 1.0 as we're not using alpha from texture
    }
)";


namespace pe {

    class CookTorranceTextureShader {

    private:

        // Coded into the shader, as it must be known at compile time
        static constexpr int MAXIMUM_NUMBER_OF_LIGHT_SOURCES = 10;

        ShaderProgram shaderProgramObject;

    public:

        CookTorranceTextureShader() : shaderProgramObject(
            cookTorranceTextureVertexShader,
            cookTorranceTextureFragmentShader
        ) {}

        /*
            Here, the view position is just the camera position.
            Roughness controls the surface roughness of the material,
            where 0 is very smooth and 1 is very rough.
            Fresnel controls the reflectivity at glancing angles, with
            0 being less reflective, and 1 being more reflective.
        */
        void drawFaces(
            const std::vector<glm::vec3>& faces,
            const std::vector<glm::vec3>& normals,
            const std::vector<glm::vec2>& texCoords,
            const glm::mat4& model,
            const glm::mat4& view,
            const glm::mat4& projection,
            GLuint textureId,
            int activeLightSources,
            glm::vec3* lightSourcesPosition,
            glm::vec4* lightSourcesColor,
            const glm::vec3& viewPosition,
            real roughness,
            real fresnel
        );
    };
}

#endif
