
#version 330 core

in vec3 TexCoords;
out vec4 FragColor;

uniform samplerCube skybox;
uniform float darkness;

void main() {
    vec3 color = vec3(texture(skybox, TexCoords));
    color -= vec3(darkness);
    FragColor = vec4(color, 1.0);
}
