#ifndef POST_PROCESSOR_H
#define POST_PROCESSOR_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "shader.h"

class PostProcessor {
	public:
		Shader postProcessorShader;
		Texture2D texture;
		GLuint width, height;
		GLboolean confuse, chaos, shake;

		PostProcessor(Shader shader, GLuint width, GLuint height);
		void beginRender();
		void endRender();
		void render(GLfloat time);
		void clear();

	private:
		GLuint MSFBO, FBO;
		GLuint RBO;
		GLuint VAO;
		
		void initRenderData();
};

#endif // !POST_PROCESSOR_H
