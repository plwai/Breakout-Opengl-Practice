#version 330 core

out vec4 color;

in vec2 TexCoords;

uniform sampler2D scene;

uniform bool chaos;
uniform bool confuse;
uniform bool shake;

uniform vec2 offsets[9];
uniform float edge_kernel[9];
uniform float blur_kernel[9];
void main() {


	color = vec4(0.0, 0.0, 0.0, 1.0);
	vec3 sampleTex[9];

	if(chaos || shake) {
		for(int i = 0; i < 9; i++) {
			sampleTex[i] = vec3(texture(scene, TexCoords.st + offsets[i]));
		}
	}

	if(chaos) {
		for(int i = 0; i < 9; i++) {
			color += vec4(sampleTex[i] * edge_kernel[i], 0.0f);
		}

		color.a = 1.0f;
	}
	else if(confuse) {
		color = vec4(1.0 - texture(scene, TexCoords).rgb , 1.0);
	}
	else if(shake) {
		for(int i = 0; i < 9; i++) {
			color += vec4(sampleTex[i] * blur_kernel[i], 0.0f);
		}

		color.a = 1.0f;
	}
	else {
		color = texture(scene, TexCoords);
	}
}