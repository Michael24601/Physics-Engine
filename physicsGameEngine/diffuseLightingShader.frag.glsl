#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec4 color;
uniform sampler2D objectTexture;
uniform bool useTexture;

#define MAX_LIGHTS 10

uniform vec3 lightPos[MAX_LIGHTS];
uniform vec4 lightColors[MAX_LIGHTS];
uniform int numActiveLights;

out vec4 FragColor;

void main(){

    // When we disable face culling in order to draw both sides
    // of the faces of an object (such as when drawing cloth),
    // not only is the front face drawn, but also the back face
    // (which is the same face but with the vertices in reverse
    // order). The noraml of said face is the same as the front
    // face but inverted.
    vec3 normal = gl_FrontFacing ? Normal : -Normal;

    vec3 finalDiffuse = vec3(0.1);
    vec3 ambient = vec3(0.2);

    for (int i = 0; i < numActiveLights; ++i) {
        vec3 lightDir = normalize(lightPos[i] - FragPos);
        float diff = max(dot(normal, lightDir), 0.0);
        finalDiffuse += diff * lightColors[i].rgb;
    }

    vec4 objColor;
    if(useTexture)
        objColor = texture(objectTexture, TexCoord);
    else
        objColor = color;

    // This just ensures that when the texture is transparent
    // no color is rendered
    if(objColor.a < 0.2)
        discard;

    FragColor = vec4(objColor.rgb * (finalDiffuse + ambient), objColor.a);
}
