#include "Week_04_Normal_Mapping.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <iostream>
#include <imgui.h>

#include "Application_Log.h"
#include <stb_image.h>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif // !STB_IMAGE_IMPLEMENTATION

Week_04_Normal_Mapping::Week_04_Normal_Mapping()
{

}

Week_04_Normal_Mapping::~Week_04_Normal_Mapping()
{

}

bool Week_04_Normal_Mapping::onCreate()
{
	// initialise the Gizmos helper class
	Gizmos::create();

	//Load fbx model
	m_fbxModel = new FBXFile();
	m_fbxModel->load("resources/models/ruinedtank/tank.fbx", FBXFile::UNITS_CENTIMETER);

	//Load normal Textures. These textures are not correctly associated with the model
	LoadImageFromFile("resources/models/ruinedtank/left_engine_norm.png", m_textureID[0]);
	LoadImageFromFile("resources/models/ruinedtank/turret_norm.png", m_textureID[1]);

	//load in and link shader program
	m_vertexShader = Utility::loadShader("shaders/vertex.glsl", GL_VERTEX_SHADER);
	m_fragmentShader = Utility::loadShader("shaders/fragment.glsl", GL_FRAGMENT_SHADER);
	const char* szInputs[] = { "Position", "Colour", "Normal", "Tangent", "BiTangent", "Tex1" };
	const char* szOutputs[] = { "fragColour" };
	//Link Shader program
	m_programID = Utility::createProgram(m_vertexShader, 0, 0, 0, m_fragmentShader, 6, szInputs, 1, szOutputs);

	//Generate vertex array objects
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(2, m_modelBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_modelBuffer[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_modelBuffer[1]);

	glEnableVertexAttribArray(0);	//Pos
	glEnableVertexAttribArray(1);	//Col
	glEnableVertexAttribArray(2);	//Norm
	glEnableVertexAttribArray(3);	//Tangent
	glEnableVertexAttribArray(4);	//BiTangent
	glEnableVertexAttribArray(5);	//tex1

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char*)0 + FBXVertex::PositionOffset);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), (char*)0 + FBXVertex::ColourOffset);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), (char*)0 + FBXVertex::NormalOffset);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), (char*)0 + FBXVertex::TangentOffset);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), (char*)0 + FBXVertex::BiNormalOffset);
	glVertexAttribPointer(5, 2, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), (char*)0 + FBXVertex::TexCoord1Offset);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// create a world-space matrix for a camera
	m_cameraMatrix = glm::inverse(glm::lookAt(glm::vec3(10, 10, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));

	// create a perspective projection matrix with a 90 degree field-of-view and widescreen aspect ratio
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, m_windowWidth / (float)m_windowHeight, 0.1f, 1000.0f);

	//model
	m_modelMatrix = glm::mat4();
	m_lightPosition = glm::vec4(20.f, 15.f, 0.f, 1.f);

	// set the clear colour and enable depth testing and backface culling
	glClearColor(0.25f, 0.25f, 0.25f, 1.f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glDeleteShader(m_fragmentShader);
	glDeleteShader(m_vertexShader);

	return true;
}

void Week_04_Normal_Mapping::Update(float a_deltaTime)
{
#pragma region Utilities
	// update our camera matrix using the keyboard/mouse
	Utility::freeMovement(m_cameraMatrix, a_deltaTime, 10);

	// clear all gizmos from last frame
	Gizmos::clear();

	// add an identity matrix gizmo
	Gizmos::addTransform(glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1));
	// add a 20x20 grid on the XZ-plane
	for (int i = 0; i < 21; ++i)
	{
		Gizmos::addLine(glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10),
			i == 10 ? glm::vec4(1, 1, 1, 1) : glm::vec4(0, 0, 0, 1));

		Gizmos::addLine(glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i),
			i == 10 ? glm::vec4(1, 1, 1, 1) : glm::vec4(0, 0, 0, 1));
	}


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
	if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();
#pragma endregion

	//move light around
	float s = sinf(a_deltaTime * 0.2f);
	float c = cosf(a_deltaTime * 0.2f);

	glm::vec4 temp = m_lightPosition;
	m_lightPosition.x = temp.x * c - temp.z * s;
	m_lightPosition.z = temp.x * s + temp.z * c;

	Gizmos::addBox(m_lightPosition.xyz, glm::vec3(0.2, 0.2, 0.2), true, glm::vec4(1.f, 0.85f, 0.05f, 1.f));
	// for each mesh in model file update child nodes
	for (int i = 0; i < m_fbxModel->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = m_fbxModel->getMeshByIndex(i);
		mesh->updateGlobalTransform();
	}
}

void Week_04_Normal_Mapping::Draw()
{
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse(m_cameraMatrix);

	// draw the gizmos from this frame
	Gizmos::draw(viewMatrix, m_projectionMatrix);

	//Get light direction
	glm::vec4 lightDir = glm::normalize(glm::vec4(0.f, 0.f, 0.f, 1.f) - m_lightPosition);

	//draw our model with our shader
	glUseProgram(m_programID);
	glBindVertexArray(m_vao);

	int projectionViewUniformLocation = glGetUniformLocation(m_programID, "u_projectionView");
	glUniformMatrix4fv(projectionViewUniformLocation, 1, false, glm::value_ptr(m_projectionMatrix * viewMatrix));

	int viewMatrixUniformLocation = glGetUniformLocation(m_programID, "u_viewMatrix");
	glUniformMatrix4fv(viewMatrixUniformLocation, 1, false, glm::value_ptr(viewMatrix));

	int cameraPositionUniformLoc = glGetUniformLocation(m_programID, "u_cameraPosition");
	glUniform4fv(cameraPositionUniformLoc, 1, glm::value_ptr(m_cameraMatrix[3]));

	int lightDirUniformLoc = glGetUniformLocation(m_programID, "u_lightDirection");
	glUniform4fv(lightDirUniformLoc, 1, glm::value_ptr(lightDir));

	int lightColourUniformLoc = glGetUniformLocation(m_programID, "u_lightColour");
	glUniform4fv(lightColourUniformLoc, 1, glm::value_ptr(glm::vec4(1.f, 0.85f, 0.05f, 1.f)));

	for (int i = 0; i < m_fbxModel->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = m_fbxModel->getMeshByIndex(i);
		glm::mat4 modelMatrix = mesh->m_globalTransform;
		//Set each model Matrix fot mesh
		int modelMatrixUniformLoc = glGetUniformLocation(m_programID, "u_modelMatrix");
		glUniformMatrix4fv(modelMatrixUniformLoc, 1, false, glm::value_ptr(modelMatrix));
		//pass through the normal matrix
		int normalMatrixUniformLoc = glGetUniformLocation(m_programID, "u_normalMatrix");
		glUniformMatrix4fv(normalMatrixUniformLoc, 1, false, glm::value_ptr(glm::transpose(glm::inverse( modelMatrix))));

		//Materials
		int materialAmbientUniformLoc = glGetUniformLocation(m_programID, "u_matAmbient");
		glUniform4fv(materialAmbientUniformLoc, 1, glm::value_ptr(mesh->m_material->ambient));

		int materialDiffuseUniformLoc = glGetUniformLocation(m_programID, "u_matDiffuse");
		glUniform4fv(materialDiffuseUniformLoc, 1, glm::value_ptr(mesh->m_material->diffuse));

		int materialSpecularUniformLoc = glGetUniformLocation(m_programID, "u_matSpecular");
		glUniform4fv(materialSpecularUniformLoc, 1, glm::value_ptr(mesh->m_material->specular));

		int rafUniform = glGetUniformLocation(m_programID, "u_RAF");
		glUniform3fv(rafUniform, 1, glm::value_ptr(modelMatrix));

		//textures on the model send to shader
		int texUniformLoc = glGetUniformLocation(m_programID, "u_diffuseTexture");
		glUniform1i(texUniformLoc, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->m_material->textureIDs[FBXMaterial::DiffuseTexture]);

		//As the model for the tank has normal data bound incorrectly we need to send through our loaded normal textures as
		texUniformLoc = glGetUniformLocation(m_programID, "u_NormalTexture");
		glUniform1i(texUniformLoc, 1);
		//Set our active texture and bind our loaded texture
		int useNormalTextureUniform = glGetUniformLocation(m_programID, "u_useNormalMap");
		if (strcmp(mesh->m_material->textureFilenames[FBXMaterial::DiffuseTexture], "left_engine_diff.png") == 0)
		{
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_textureID[0]);

			glUniform1i(useNormalTextureUniform, 1);
		}
		else if (strcmp(mesh->m_material->textureFilenames[FBXMaterial::DiffuseTexture], "turret_diff.png") == 0)
		{
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_textureID[1]);

			glUniform1i(useNormalTextureUniform, 1);
		}
		else
		{
			glUniform1i(useNormalTextureUniform, 0);
		}

		glBindBuffer(GL_ARRAY_BUFFER, m_modelBuffer[0]);
		glBufferData(GL_ARRAY_BUFFER, mesh->m_vertices.size() * sizeof(FBXVertex), mesh->m_vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_modelBuffer[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->m_indices.size() * sizeof(GL_UNSIGNED_INT), mesh->m_indices.data(), GL_STATIC_DRAW);

		glDrawElements(GL_TRIANGLES, mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}
	glBindVertexArray(0);
	glUseProgram(0);
}

void Week_04_Normal_Mapping::Destroy()
{
	delete m_fbxModel;
	Gizmos::destroy();

	glDeleteBuffers(1, &m_modelBuffer[0]);
	glDeleteBuffers(1, &m_modelBuffer[1]);
	glDeleteVertexArrays(1, &m_vao);
	glDeleteProgram(m_programID);

}

bool Week_04_Normal_Mapping::LoadImageFromFile(std::string a_filePath, unsigned int& a_textureID)
{
	int width = 0, height = 0, channels = 0;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* imageData = stbi_load(a_filePath.c_str(), &width, &height, &channels, 0);
	if (imageData != nullptr)
	{
		a_textureID = 0;
		glGenTextures(1, &a_textureID);

		glBindTexture(GL_TEXTURE_2D, a_textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		if (channels == 1)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_R, width, height, 0, GL_R, GL_UNSIGNED_BYTE, imageData);
		}
		if (channels == 3)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(imageData);

		Application_Log* log = Application_Log::Get();
		if (log != nullptr)
		{
			log->addLog(LOG_LEVEL::LOG_INFO, "Successfully loaded Texture : %s", a_filePath.c_str());
		}
		return true;
	}
	else
	{
		Application_Log* log = Application_Log::Get();
		if (log != nullptr)
		{
			log->addLog(LOG_LEVEL::LOG_ERROR, "Failed to Load Texture : %s", a_filePath.c_str());
		}
	}
	return false;
}
