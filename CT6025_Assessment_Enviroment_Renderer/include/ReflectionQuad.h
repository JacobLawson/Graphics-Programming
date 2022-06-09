#ifndef __REFLECTION_QUAD_H_
#define __REFLECTION_QUAD_H_

#include <glm/glm.hpp>
#include "FBXFile.h"

class Model;

struct ReflectionVertex
{
	enum Offsets
	{
		PositionOffset = 0,
		TexCoord1Offset = PositionOffset + sizeof(glm::vec4)
	};

	glm::vec4 pos;
	glm::vec2 uv;

	ReflectionVertex() {}
	ReflectionVertex(glm::vec4 a_pos, glm::vec2 a_uv) : pos(a_pos), uv(a_uv) {}
};
class ReflectionQuad
{
public:
	ReflectionQuad();
	~ReflectionQuad();

	void CreateModelBuffer();
	void CreateQuad(unsigned int a_width, unsigned int a_height);
	void CreateOutputFrameBuffer(unsigned int a_windowWidth, unsigned int a_windowHeight);

	void DrawReflection(glm::mat4 a_projectionViewMatrix, glm::mat4 a_cameraMatrix, glm::vec4 a_lightPos);
	void DrawFullscreenQuad(glm::mat4 a_projectionViewMatrix, glm::mat4 a_cameraMatrix);

	void SetModel(Model* a_model) { m_reflectedModel = a_model; }
	void SetShader(unsigned int a_shaderFBO, unsigned int a_shaderFS) { m_reflectionQuad_fbo_programID = a_shaderFBO;  m_reflectionQuad_fs_programID = a_shaderFS; }
	unsigned int GetShaderFBOProgram() { return m_reflectionQuad_fbo_programID; }

private:
	//window
	unsigned int m_windowWidth;
	unsigned int m_windowHeight;

	//shader programs
	unsigned int m_reflectionQuad_fbo_programID;
	unsigned int m_reflectionQuad_fs_programID;

	//model related variables
	FBXFile* m_reflectionQuad_fbxModel;
	Model* m_reflectedModel;

	unsigned int m_reflectionQuad_vao[2];
	unsigned int m_reflectionQuad_vbo[2];
	unsigned int m_reflectionQuad_ibo[2];

	//FBO related variables
	unsigned int m_reflectionQuad_fbo;
	unsigned int m_reflectionQuad_fbo_texture;
	unsigned int m_reflectionQuad_fbo_depth_texture;
	unsigned int m_reflectionQuad_fbo_linear_depth;
};

#endif //__REFLECTION_QUAD_H_