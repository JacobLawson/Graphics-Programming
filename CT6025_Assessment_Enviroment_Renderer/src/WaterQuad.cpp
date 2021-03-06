#include "WaterQuad.h"

#include "SimpleVertex.h"
#include "Utilities.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

WaterQuad::WaterQuad()
{
	m_displacementScale = 0.f;
	m_innerTessEdge = 2;
	m_outerTessEdge = 2;

	m_displacementDirection = 1;
}

WaterQuad::~WaterQuad()
{
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ibo);
	glDeleteVertexArrays(1, &m_vao);
}

void WaterQuad::CreateQuad(unsigned int a_width, unsigned int a_height)
{
	//\================================================================================
	//\ Create a flat grid of vertex data that contains position, UV and normal data
	//\================================================================================
	//unsigned int width = 20; unsigned int height = 20;
	unsigned int maxVerts = a_width * a_height;
	m_vertices = new SimpleVertex[maxVerts];
	glm::vec4 currentPosition = glm::vec4(-(a_width * 0.5), -1.5f, -(a_height * 0.5f), 1.f);
	glm::vec2 currentUV = glm::vec2(0.f, 0.f);
	float uvXIncrement = 1.f / a_width;
	float uvYIncrement = 1.f / a_height;
	for (unsigned int i = 0; i < maxVerts; ++i)
	{
		m_vertices[i].position = currentPosition;
		m_vertices[i].normal = glm::vec4(0.f, 0.f, 0.f, 0.f);
		m_vertices[i].tangent = glm::vec4(0.f, 0.f, 0.f, 0.f);
		m_vertices[i].texCoord = currentUV;

		currentPosition.x += 1.f;
		currentUV.x += uvXIncrement;
		if (i > 0 && i % a_width == (a_width - 1))
		{
			currentPosition.x = -((float)(a_width) * 0.5f);
			currentPosition.z += 1.f;
			currentUV.x = 0.f;
			currentUV.y += uvYIncrement;
		}

	}
	//\===============================================================================
	//\Populate the index buffer with the indices to draw the mesh
	//\===============================================================================
	m_maxIndices = (a_width - 1) * (a_height - 1) * 6; //for every square we need 6 indicies to render two triangles
	unsigned int* indexBuffer = new unsigned int[m_maxIndices];
	for (unsigned int i = 0, j = 0; i < m_maxIndices; i += 6, j++)
	{
		if (j > 0 && j % a_width == (a_width - 1))
		{
			i -= 6;
			continue;
		}
		indexBuffer[i] = j;
		indexBuffer[i + 1] = j + a_width;
		indexBuffer[i + 2] = j + 1;

		indexBuffer[i + 3] = j + a_width + 1;
		indexBuffer[i + 4] = j + 1;
		indexBuffer[i + 5] = j + a_width;

	}

	//Calculate Normals and Tangents for this surface
	for (unsigned int i = 0; i < m_maxIndices; i += 3)
	{
		glm::vec3 v1 = m_vertices[indexBuffer[i + 2]].position.xyz - m_vertices[indexBuffer[i]].position.xyz;
		glm::vec3 v2 = m_vertices[indexBuffer[i + 1]].position.xyz - m_vertices[indexBuffer[i]].position.xyz;

		glm::vec4 normal = glm::vec4(glm::cross(v2, v1), 0.f);

		m_vertices[indexBuffer[i]].normal += normal;
		m_vertices[indexBuffer[i + 1]].normal += normal;
		m_vertices[indexBuffer[i + 2]].normal += normal;

		//This is to calculate the tangent to the normal to be used in lighting 
		//and use of the normal map.
		//The maths used here is taken from Mathematics for 3D Games Programming and Computer Graphics by Eric Lengyel
		glm::vec2 uv0 = m_vertices[indexBuffer[i]].texCoord;
		glm::vec2 uv1 = m_vertices[indexBuffer[i + 1]].texCoord;
		glm::vec2 uv2 = m_vertices[indexBuffer[i + 2]].texCoord;

		glm::vec2 uvVec1 = uv2 - uv0;
		glm::vec2 uvVec2 = uv1 - uv0;

		float coefficient = 1.0f / uvVec1.x * uvVec2.y - uvVec1.y * uvVec2.x;

		glm::vec4 tangent;
		tangent[0] = coefficient * (v2[0] * uvVec2[1] + v1[0] * -uvVec1[1]);
		tangent[1] = coefficient * (v2[1] * uvVec2[1] + v1[1] * -uvVec1[1]);
		tangent[2] = coefficient * (v2[2] * uvVec2[1] + v1[2] * -uvVec1[1]);
		tangent[3] = 0.f;

		m_vertices[indexBuffer[i]].tangent += tangent;
		m_vertices[indexBuffer[i + 1]].tangent += tangent;
		m_vertices[indexBuffer[i + 2]].tangent += tangent;

	}
	for (unsigned int i = 0; i < maxVerts; ++i)
	{
		m_vertices[i].normal = glm::normalize(m_vertices[i].normal);
		m_vertices[i].tangent = glm::normalize(m_vertices[i].tangent);
	}

	//Generate and bind our Vertex Array Object
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	//Generate our Vertex Buffer Object
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, maxVerts * sizeof(SimpleVertex), m_vertices, GL_DYNAMIC_DRAW);
	//Define our vertex attribute data for this buffer we have to do this for our vertex input data
	//Position, Normal & Colour this attribute order is taken from our szInputs string array order
	glEnableVertexAttribArray(0); //position
	glEnableVertexAttribArray(1); //normal
	glEnableVertexAttribArray(2); //tangent
	glEnableVertexAttribArray(3); //texCoords
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(SimpleVertex), ((char*)0) + SimpleVertex::PositionOffset);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(SimpleVertex), ((char*)0) + SimpleVertex::NormalOffset);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE, sizeof(SimpleVertex), ((char*)0) + SimpleVertex::TangentOffset);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_TRUE, sizeof(SimpleVertex), ((char*)0) + SimpleVertex::TexCoordOffset);

	//generate and bind an index buffer
	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_maxIndices * sizeof(unsigned int), indexBuffer, GL_STATIC_DRAW);


	glBindVertexArray(0);
	//unbind our current vertex Buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//unbind our current index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	glPatchParameteri(GL_PATCH_VERTICES, 3);
}

void WaterQuad::Update(float a_deltaTime)
{
	//control waves
	if (m_displacementScale >= 2 || m_displacementScale <= 0)
	{
		m_displacementDirection *= -1;
	}
	m_displacementScale += (1 * a_deltaTime) * m_displacementDirection;
}

void WaterQuad::Draw(glm::mat4 a_projectionViewMatrix, glm::mat4 a_cameraMatrix, glm::vec4 a_lightPos)
{
	//bind our shader program
	glUseProgram(m_water_programID);
	//bind our vertex array object
	glBindVertexArray(m_vao);

	glm::mat4 viewMatrix = glm::inverse(a_cameraMatrix);
	//get our shaders uniform location for our projectionViewMatrix and then use glUniformMatrix4fv to fill it with the correct data
	unsigned int projectionViewUniform = glGetUniformLocation(m_water_programID, "ProjectionView");
	glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(a_projectionViewMatrix * viewMatrix));

	unsigned int ViewMatrixUniform = glGetUniformLocation(m_water_programID, "ViewMatrix");
	glUniformMatrix4fv(ViewMatrixUniform, 1, false, glm::value_ptr(viewMatrix));

	//get our shaders uniform location for our projectionViewMatrix and then use glUniformMatrix4fv to fill it with the correct data
	glm::mat4 m4Model = glm::mat4(1.0);
	unsigned int ModelMatrixUniform = glGetUniformLocation(m_water_programID, "Model");
	glUniformMatrix4fv(ModelMatrixUniform, 1, false, glm::value_ptr(m4Model));

	unsigned int cameraPosUniform = glGetUniformLocation(m_water_programID, "cameraPosition");
	glUniform4fv(cameraPosUniform, 1, glm::value_ptr(a_cameraMatrix[3]));

	//pass the directional light direction to our fragment shader
	glm::vec4 lightDir = -a_lightPos;
	lightDir.w = 0.f;
	lightDir = glm::normalize(lightDir);
	unsigned int lightDirUniform = glGetUniformLocation(m_water_programID, "lightDirection");
	glUniform4fv(lightDirUniform, 1, glm::value_ptr(lightDir));

	//Tessellation location edge uniforms
	unsigned int tesUniformID = glGetUniformLocation(m_water_programID, "innerEdge");
	glUniform1i(tesUniformID, m_innerTessEdge);

	tesUniformID = glGetUniformLocation(m_water_programID, "outerEdge");
	glUniform1i(tesUniformID, m_outerTessEdge);

	tesUniformID = glGetUniformLocation(m_water_programID, "displacementScale");
	glUniform1f(tesUniformID, m_displacementScale);
	//bind our textureLocation variables from the shaders and set it's value to 0 as the active texture is texture 0
	unsigned int texUniformID = glGetUniformLocation(m_water_programID, "diffuseTexture");
	glUniform1i(texUniformID, 0);
	//set our active texture, and bind our loaded texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_DiffuseTex);

	texUniformID = glGetUniformLocation(m_water_programID, "normalTexture");
	glUniform1i(texUniformID, 1);
	//set our active texture, and bind our loaded texture
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_NormalTex);

	texUniformID = glGetUniformLocation(m_water_programID, "specularTexture");
	glUniform1i(texUniformID, 2);
	//set our active texture, and bind our loaded texture
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_SpecularTex);

	texUniformID = glGetUniformLocation(m_water_programID, "displacementTexture");
	glUniform1i(texUniformID, 3);
	//set our active texture, and bind our loaded texture
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_DisplacementTex);

	texUniformID = glGetUniformLocation(m_water_programID, "occlusionTexture");
	glUniform1i(texUniformID, 4);
	//set our active texture, and bind our loaded texture
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, m_OcclusionTex);



	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//depending on the state call draw with glDrawElements to draw our buffer
	//glDrawElements uses the index array in our index buffer to draw the vertices in our vertex buffer
	glDrawElements(GL_PATCHES, m_maxIndices, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glUseProgram(0);
}

bool WaterQuad::LoadTexture(std::string a_diffuse, std::string a_normalMap, std::string a_specularMap, std::string a_displacementMap, std::string a_occlusionTex)
{
	//load in our Texture
	Utility::LoadImageFromFile(a_diffuse, m_DiffuseTex);
	Utility::LoadImageFromFile(a_normalMap, m_NormalTex);
	Utility::LoadImageFromFile(a_specularMap, m_SpecularTex);
	Utility::LoadImageFromFile(a_displacementMap, m_DisplacementTex);
	Utility::LoadImageFromFile(a_occlusionTex, m_OcclusionTex);
	return true;
}
