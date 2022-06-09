#ifndef __Week_02_Fullscreen_Sobel_Shader_H_
#define __Week_02_Fullscreen_Sobel_Shader_H_

#include "Application.h"
#include <glm/glm.hpp>
#include <string>

// Derived application class that wraps up all globals neatly
class Week_02_Fullscreen_Sobel_Shader : public Application
{
public:

	Week_02_Fullscreen_Sobel_Shader();
	virtual ~Week_02_Fullscreen_Sobel_Shader();

protected:

	typedef struct Vertex
	{
		glm::vec4 position;
		glm::vec4 colour;
		glm::vec2 uvs;
	}Vertex;

	typedef struct Texture
	{
		unsigned int textureID;
		int width;
		int height;
	};

	bool LoadImageFromFile(std::string a_filePath, Texture& a_texture);
	virtual bool onCreate();
	virtual void Update(float a_deltaTime);
	virtual void Draw();
	virtual void Destroy();

	Texture m_texture;
	Vertex* m_verticies;

	unsigned int m_programID;
	unsigned int m_vertexShader;
	unsigned int m_fragmentShader;

	unsigned int m_maxVerts;
	unsigned int m_vao;				//Vertex array object
	unsigned int m_vbo;				//Vertex buffer object
	unsigned int m_ibo;				//index buffer object
};

#endif // __Week_02_Fullscreen_Sobel_Shader_H_