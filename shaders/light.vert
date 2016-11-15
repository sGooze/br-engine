#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

out vec3 v_normal;
out vec3 FragPos; 
out vec2 TexCoords; 

uniform mat4 transform;
uniform mat4 transformi;
uniform mat4 view;
uniform mat4 project;

void main()
{
    gl_Position = project * view * transform * vec4(position, 1.0f);
	FragPos = vec3(transform * vec4(position, 1.0f));
	v_normal = mat3(transpose(transformi)) * normal;
	TexCoords = texCoord;
}