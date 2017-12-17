#version 330 core

in VS_OUT {
	vec2 TexCoords;
} fs_in;

out vec4 color;

uniform sampler2D image;
uniform vec3 spriteColor;

void main() {
	color = vec4(spriteColor, 1.0) * texture(image, fs_in.TexCoords);
}