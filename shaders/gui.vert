#version 330 core
in vec2 position;
in vec2 UV;
in vec4 color;

out vec2 Frag_UV;
out vec4 Frag_Color;

uniform mat4 ProjMtx;

void main()
{
	Frag_UV = UV;
	Frag_Color = color;
	gl_Position = ProjMtx * vec4(position.xy,0,1);
}