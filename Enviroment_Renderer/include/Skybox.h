#ifndef __SKYBOX_H_
#define __SKYBOX_H_

#include <vector>
#include <string>

class Shader;
class Texture;

class SkyBox
{
public:
	SkyBox();
	~SkyBox();

	bool Draw(glm::mat4 a_projectionViewMatrix, glm::mat4 a_cameraMatrix);

	bool LoadSkyBox(std::vector<std::string> a_textureFaces, unsigned int a_cubeMapImageTags[6]);
	bool GenerateSkyBoxBuffers();
	
	void SetShader(unsigned int a_shaderID) { m_skyboxShader = a_shaderID; }
	unsigned int GetShaderProgram() { return m_skyboxShader; }

protected:
	Texture* m_texture;
	unsigned int m_cubeMapTextureID;
	unsigned int m_vao;
	unsigned int m_vbo;

	unsigned int m_skyboxShader;

};

#endif //__SKYBOX_H_