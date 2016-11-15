#version 330 core
in vec3 v_normal;
in vec3 FragPos;
out vec4 color;

uniform vec3 viewPos;
uniform samplerCube skybox;

void main()
{             
    vec3 I = normalize(FragPos - viewPos);
    vec3 R = reflect(I, normalize(v_normal));
    color = texture(skybox, R);
}