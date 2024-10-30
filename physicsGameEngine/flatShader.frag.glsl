
#version 330 core

in vec3 FragPos;
in vec2 TexCoord;

uniform vec4 color;
uniform sampler2D objectTexture;
uniform bool useTexture;

out vec4 FragColor;

void main(){
    if(useTexture)
        FragColor = texture(objectTexture, TexCoord);
    else
        FragColor = color;
}