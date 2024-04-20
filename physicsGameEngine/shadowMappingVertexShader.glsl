
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;
out vec4 FragPosLightSpace;
out vec3 ViewPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform mat4 lightSpace;

void main(){    

    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = transpose(inverse(mat3(model))) * aNormal;
    FragPosLightSpace = lightSpace * vec4(FragPos, 1.0);
    
    // We can extract the camera position from thew view matrix,
    // no need to send it as a uniform
    ViewPos = (inverse(view))[3].xyz;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}