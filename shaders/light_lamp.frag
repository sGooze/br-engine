#version 330 core
in vec2 TexCoords;

out vec4 color;

uniform vec3 lightColor;
uniform sampler2D mdlTexture;

void main()
{
	color = vec4(lightColor, 1.0f) * texture(mdlTexture, vec2(TexCoords.x, 1.0f - TexCoords.y));
}