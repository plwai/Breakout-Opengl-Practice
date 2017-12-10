#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <map>
#include <string>

#include "shader.h"
#include "texture.h"

class ResourceManager {
	public:
		static std::map<std::string, Shader> Shaders;
		static std::map<std::string, Texture2D> Textures;

		static Shader loadShader(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile, std::string name);
		static Shader getShader(std::string name);
		
		static Texture2D loadTexture(const GLchar *file, GLboolean alpha, std::string name);
		static Texture2D getTexture(std::string name);

		static void clear();

	private:
		ResourceManager() {}

		static Shader loadShaderFromFile(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile = nullptr);
		static Texture2D loadTextureFromFile(const GLchar *file, GLboolean alpha);
};

#endif // !RESOURCE_MANAGER_H
