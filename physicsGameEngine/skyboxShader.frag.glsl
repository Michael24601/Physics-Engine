
#version 330 core

in vec3 TexCoords;
out vec4 FragColor;

uniform samplerCube cubemap;

void main() {
    vec3 color = vec3(texture(cubemap, TexCoords));    
    FragColor = vec4(color, 1.0);
}