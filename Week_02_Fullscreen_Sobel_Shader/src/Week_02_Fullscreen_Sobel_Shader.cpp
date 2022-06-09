#include "Week_02_Fullscreen_Sobel_Shader.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <iostream>
#include <imgui.h>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#endif // !STB_IMAGE_IMPLEMENTATION

#include "Application_Log.h"

Week_02_Fullscreen_Sobel_Shader::Week_02_Fullscreen_Sobel_Shader()
{

}

Week_02_Fullscreen_Sobel_Shader::~Week_02_Fullscreen_Sobel_Shader()
{

}

bool Week_02_Fullscreen_Sobel_Shader::onCreate()
{
	//Load Texture
	LoadImageFromFile("images/Icon2.png", m_texture);

	//Load shaders
	m_vertexShader = Utility::loadShader("shaders/vertex.glsl", GL_VERTEX_SHADER);
	m_fragmentShader = Utility::loadShader("shaders/fragment.glsl", GL_FRAGMENT_SHADER);
	//define input outputs for out shaders
	const char* szInputs[] = { "position", "colour", "TexCoord" };
	const char* szOutputs[] = { "fragColour" };
	//Link shaders together and create program
	m_programID = Utility::createProgram(m_vertexShader, 0, 0, 0, m_fragmentShader, 3, szInputs, 1, szOutputs);
	//clean up shaders
	glDeleteShader(m_vertexShader);
	glDeleteShader(m_fragmentShader);

	Vertex* verts = new Vertex[4];
	verts[0].position = glm::vec4(-1.f, -1.f, 0.f, 1.f);
	verts[1].position = glm::vec4(-1.f, 1.f, 0.f, 1.f);
	verts[2].position = glm::vec4(1.f, -1.f, 0.f, 1.f);
	verts[3].position = glm::vec4(1.f, 1.f, 0.f, 1.f);
	verts[0].colour = glm::vec4(1.f, 0.f, 0.f, 1.f);
	verts[1].colour = glm::vec4(0.f, 1.f, 0.f, 1.f);
	verts[2].colour = glm::vec4(0.f, 0.f, 1.f, 1.f);
	verts[3].colour = glm::vec4(1.f, 1.f, 1.f, 1.f);
	verts[0].uvs = glm::vec2(0.f, 0.f);
	verts[1].uvs = glm::vec2(0.f, 1.f);
	verts[2].uvs = glm::vec2(1.f, 0.f);
	verts[3].uvs = glm::vec2(1.f, 1.f);
	unsigned int indices[] = { 0, 2, 1, 3, 1, 2 };

	//generate and bind vertex array object
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	//genearate and bind vertex buffer object
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), verts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)0 + 16);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)0 + 32);
	delete[] verts;
	//Generate and fill index buffer data
	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// set the clear colour and enable depth testing and backface culling
	glClearColor(0.25f, 0.25f, 0.25f, 1.f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	return true;
}

void Week_02_Fullscreen_Sobel_Shader::Update(float a_deltaTime)
{	
	static bool show_demo_window = true;
	//ImGui::ShowDemoWindow(&show_demo_window);
	Application_Log* log = Application_Log::Get();
	if (log != nullptr && show_demo_window)
	{
		log->showLog(&show_demo_window);
	}
	//show application log window
	if (glfwGetKey(m_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(m_window, GLFW_KEY_L) == GLFW_PRESS) {
		show_demo_window = !show_demo_window;
	}
	// quit our application when escape is pressed
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();
}

void Week_02_Fullscreen_Sobel_Shader::Draw()
{
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_programID);

	//pass time to shader program
	//get uniform location
	int textureUniformLocation = glGetUniformLocation(m_programID, "u_texture");
	glUniform1i(textureUniformLocation, 0);

	int imageWidthUniformLocation = glGetUniformLocation(m_programID, "u_width");
	glUniform1f(imageWidthUniformLocation, m_texture.width);

	int imageHeightUniformLocation = glGetUniformLocation(m_programID, "u_height");
	glUniform1f(imageHeightUniformLocation, m_texture.height);

	glBindVertexArray(m_vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture.textureID);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (char*)0);

	glBindVertexArray(0);
	glUseProgram(0);
}

void Week_02_Fullscreen_Sobel_Shader::Destroy()
{
	glDeleteProgram(m_programID);
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ibo);
	glDeleteVertexArrays(1, &m_vao);
}

bool Week_02_Fullscreen_Sobel_Shader::LoadImageFromFile(std::string a_filePath, Texture& a_texture)
{
	int width = 0, height = 0, channels = 0;
	stbi_set_flip_vertically_on_load(true);

	//load image
	unsigned char* imageData = stbi_load(a_filePath.c_str(), &width, &height, &channels, 4);
	if (imageData != nullptr)	//Texture has successfully been opened
	{
		a_texture.textureID = 0;
		a_texture.width = width;
		a_texture.height = height;

		glGenTextures(1, &a_texture.textureID);
		glBindTexture(GL_TEXTURE_2D, a_texture.textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(imageData);
		return true;
	}
	Application_Log* log = Application_Log::Get();
	if (log != nullptr)
	{
		log->addLog(LOG_LEVEL::LOG_ERROR, "Failed to open Image File: %s", a_filePath.c_str());
	}
	return false;
}

