#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 Normal;
out vec3 FragPos;

uniform mat4 transform;
uniform mat4 transformi;
uniform mat4 view;
uniform mat4 project;

void main()
{
    gl_Position = project * view * transform * vec4(position, 1.0f);
    Normal = mat3(transpose(transformi)) * normal;
    FragPos = vec3(model * vec4(position, 1.0f));
}