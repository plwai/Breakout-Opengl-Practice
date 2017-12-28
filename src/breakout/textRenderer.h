#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <map>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <BreakoutConfig.h>

#include "texture.h"
#include "shader.h"

struct Character {
	GLuint textureID;
	glm::ivec2 size;
	glm::ivec2 bearing;
	GLuint advance;
};

class TextRenderer {
	public:
		std::map<GLuint, Character> characters;
		Shader textShader;

		TextRenderer(GLuint width, GLuint height);
		~TextRenderer();
		void load(std::string font, GLuint fontSize);
		void renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color = glm::vec3(1.0f));

	private:
		GLuint VAO, VBO;
};


#endif // !TEXT_RENDERER_H


