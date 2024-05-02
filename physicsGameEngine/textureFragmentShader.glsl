
#version 330 core

in vec3 FragPos;
in vec2 TexCoord;

uniform sampler2D objectTexture;

out vec4 FragColor;

void main(){
    vec4 texColor = texture(objectTexture, TexCoord);
    FragColor = texColor;
}