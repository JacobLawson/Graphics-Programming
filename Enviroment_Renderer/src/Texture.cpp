#include <iostream>
#include <glad/glad.h>

#include "Texture.h"
*
Texture::Texture(): m_filename(), m_width(0), m_height(0), m_textureID(0)
{}

Texture::~Texture()
{
	unload();
}

bool Texture::Load(std::string a_filename)
{
	int width = 0, height = 0, channels = 0;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* imageData = stbi_load(a_filename.c_str(), &width, &height, &channels, 4);
	if (imageData != nullptr)
	{
		m_filename = a_filename;
		m_width = width;
		m_height = height;
		glGenTextures(1, &m_textureID);
		glBindTexture(GL_TEXTURE_2D, m_textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(imageData);
		//Success
		return true;
	}
	//Fail
	return false;
}

void Texture::unload()
{
	glDeleteTextures(1, &m_textureID);
}

unsigned int Texture::LoadCubeMap(std::vector<std::string> a_textureFaces, unsigned int* cubeMapFace_ID)
{
	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);

	int width, height, nrChannels;
	for (int i = 0; i < 6; ++i)
	{
		stbi_set_flip_vertically_on_load(false);
		unsigned char* data;
		data = stbi_load(a_textureFaces[i].c_str(), &width, &height, &nrChannels, 4);
		if (data != nullptr)
		{
			m_width = width;
			m_height = height;
			glTexImage2D(cubeMapFace_ID[i], 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
			std::cout << "Successfully loaded image File: " << a_textureFaces[i] << std::endl;
		}
		else
		{
			std::cout << "Failed to open image File: " << a_textureFaces[i] << std::endl;
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	return m_textureID;
}
