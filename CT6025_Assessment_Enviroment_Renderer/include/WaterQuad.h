#ifndef __WATER_QUAD_H_
#define __WATER_QUAD_H_

#include <glm/glm.hpp>
#include <vector>
#include <string>

class SimpleVertex;

class WaterQuad
{
public:
	WaterQuad();
	~WaterQuad();

	void CreateQuad(unsigned int a_width, unsigned int a_height);
	void Update(float a_deltaTime);
	void Draw(glm::mat4 a_projectionViewMatrix, glm::mat4 a_cameraMatrix, glm::vec4 a_lightPos);
	bool LoadTexture(std::string a_diffuse, std::string a_normalMap, std::string a_specularMap, std::string a_displacementMap, std::string a_occlusionTex);

	void SetShader(unsigned int a_shaderID) { m_water_programID = a_shaderID; }
	unsigned int GetShaderProgram() { return m_water_programID; }

	//tesselation getters and setters
	int* GetInnerTessEdgeValue() { return &m_innerTessEdge; }
	int* GetOuterTessEdgeValue() { return &m_outerTessEdge; }
	float* GetDisplacementValue() { return &m_displacementScale; }


private:
	//shader program
	unsigned int m_water_programID;

	//Quad data
	unsigned int m_maxIndices;
	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_ibo;

	//Texture Handles for OpenGL
	unsigned int m_DiffuseTex;
	unsigned int m_NormalTex;
	unsigned int m_SpecularTex;
	unsigned int m_OcclusionTex;
	unsigned int m_DisplacementTex;

	SimpleVertex* m_vertices;

	//tesselation settings
	int m_innerTessEdge;
	int m_outerTessEdge;
	float m_displacementScale;

	//animation up down direction
	float m_displacementDirection;
};

#endif // !__WATER_QUAD_H_
