#include "postProcessor.h"

#include <iostream>

PostProcessor::PostProcessor(Shader shader, GLuint width, GLuint height)
	: postProcessorShader(shader), texture(), width(width), height(height), confuse(GL_FALSE), chaos(GL_FALSE), shake(GL_FALSE) {

	glGenFramebuffers(1, &this->MSFBO);
	glGenFramebuffers(1, &this->FBO);
	glGenRenderbuffers(1, &this->RBO);

	glBindFramebuffer(GL_FRAMEBUFFER, this->MSFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, this->RBO);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, GL_RGB, this->width, this->height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, this->RBO);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR::POSTPROCESSOR: Failed to initialize MSFBO" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
	this->texture.generate(width, height, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->texture.ID, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::POSTPROCESSOR: Failed to initialize FBO" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	this->initRenderData();
	this->postProcessorShader.setInteger("scene", 0, GL_TRUE);

	GLfloat offset = 1.0f / 300.0f;
	GLfloat offsets[9][2] = {
			{ -offset, offset },  // top-left
			{ 0.0f, offset },  // top-center
			{ offset, offset },  // top-right
			{ -offset, 0.0f },  // center-left
			{ 0.0f, 0.0f },  // center-center
			{ offset, 0.0f },  // center - right
			{ -offset, -offset },  // bottom-left
			{ 0.0f, -offset },  // bottom-center
			{ offset, -offset }   // bottom-right    
	};
	glUniform2fv(glGetUniformLocation(this->postProcessorShader.ID, "offsets"), 9, (GLfloat*)offsets);

	GLfloat edge_kernel[9] = {
		-1, -1, -1,
		-1, 8, -1,
		-1, -1, -1,
	};
	glUniform1fv(glGetUniformLocation(this->postProcessorShader.ID, "edge_kernel"), 9, edge_kernel);

	GLfloat blur_kernel[9] = {
		1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0,
	};
	glUniform1fv(glGetUniformLocation(this->postProcessorShader.ID, "blur_kernel"), 9, blur_kernel);
}

void PostProcessor::beginRender() {
	glBindFramebuffer(GL_FRAMEBUFFER, this->MSFBO);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void PostProcessor::endRender() {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, this->MSFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->FBO);
	glBlitFramebuffer(0, 0, this->width, this->height, 0, 0, this->width, this->height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void PostProcessor::render(GLfloat time) {
	this->postProcessorShader.use();
	this->postProcessorShader.setFloat("time", time);
	this->postProcessorShader.setInteger("confuse", this->confuse);
	this->postProcessorShader.setInteger("chaos", this->chaos);
	this->postProcessorShader.setInteger("shake", this->shake);

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(this->VAO);
	this->texture.bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void PostProcessor::initRenderData() {
	GLuint VBO;
	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 0.0f, 1.0f,

		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	};
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(this->VAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (GLvoid*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}