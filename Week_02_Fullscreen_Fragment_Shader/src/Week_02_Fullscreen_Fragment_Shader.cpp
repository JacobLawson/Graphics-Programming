#include "Week_02_Fullscreen_Fragment_Shader.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <iostream>
#include <imgui.h>

#include "Application_Log.h"

Week_02_Fullscreen_Fragment_Shader::Week_02_Fullscreen_Fragment_Shader()
{

}

Week_02_Fullscreen_Fragment_Shader::~Week_02_Fullscreen_Fragment_Shader()
{

}

bool Week_02_Fullscreen_Fragment_Shader::onCreate()
{
	//Load shaders
	m_vertexShader = Utility::loadShader("shaders/vertex.glsl", GL_VERTEX_SHADER);
	m_fragmentShader = Utility::loadShader("shaders/fragment.glsl", GL_FRAGMENT_SHADER);
	//define input outputs for out shaders
	const char* szInputs[] = { "position", "colour" };
	const char* szOutputs[] = { "fragColour" };
	//Link shaders together and create program
	m_programID = Utility::createProgram(m_vertexShader, 0, 0, 0, m_fragmentShader, 2, szInputs, 1, szOutputs);
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
	delete[] verts;
	//Generate and fill index buffer data
	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	// set the clear colour and enable depth testing and backface culling
	glClearColor(0.25f,0.25f,0.25f,1.f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	return true;
}

void Week_02_Fullscreen_Fragment_Shader::Update(float a_deltaTime)
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

void Week_02_Fullscreen_Fragment_Shader::Draw()
{
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_programID);
	glBindVertexArray(m_vao);

	//pass time to shader program
	//get uniform location
	int timeUniformLocation = glGetUniformLocation(m_programID, "u_time");
	glUniform1f(timeUniformLocation, Utility::getTotalTime());

	int resolutionUniformLocation = glGetUniformLocation(m_programID, "u_resolution");
	glUniform2fv(resolutionUniformLocation, 1, glm::value_ptr(glm::vec2(m_windowWidth, m_windowHeight)));

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (char*)0);

	glBindVertexArray(0);
	glUseProgram(0);
}

void Week_02_Fullscreen_Fragment_Shader::Destroy()
{
	glDeleteProgram(m_programID);
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ibo);
	glDeleteVertexArrays(1, &m_vao);
}

