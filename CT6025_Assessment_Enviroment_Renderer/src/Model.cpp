#include "Model.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

Model::Model()
{
}

Model::~Model()
{
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ibo);
	glDeleteVertexArrays(1, &m_vao);
	glDeleteFramebuffers(1, &m_FBO);
}

void Model::Load(const char* a_modelFile)
{
	m_fbxModel = new FBXFile();
	m_fbxModel->load(a_modelFile, FBXFile::UNITS_DECIMETER);
	m_modelMatrix = glm::mat4();
}

void Model::Initialise(unsigned int a_windowWidth, unsigned int a_windowHeight, unsigned int a_shadowFramebufferSize)
{
	//Generate our OpenGL Vertex and Index Buffers for rendering our FBX Model Data
	// OPENGL: genorate the VBO, IBO and VAO
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ibo);
	glGenVertexArrays(1, &m_vao);

	// OPENGL: Bind  VAO, and then bind the VBO and IBO to the VAO
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

	//There is no need to populate the vbo & ibo buffers with any data at this stage
	//this can be done when rendering each mesh component of the FBX model

	// enable the attribute locations that will be used on our shaders
	glEnableVertexAttribArray(0); //Pos
	glEnableVertexAttribArray(1); //Col
	glEnableVertexAttribArray(2); //Norm
	glEnableVertexAttribArray(3); //Tex1


								  // tell our shaders where the information within our buffers lie
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char*)0) + FBXVertex::PositionOffset);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::ColourOffset);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::NormalOffset);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::TexCoord1Offset);

	// finally, where done describing our mesh to the shader
	// we can describe the next mesh
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//\===========================================================================================================
	//\Set up the frame buffer for rendering our shadows into
	m_FBO_texture = -1;
	SetupFrameBuffer(m_FBO, a_shadowFramebufferSize, a_shadowFramebufferSize, m_FBO_texture, m_FBO_depth_texture);
	//\===========================================================================================================
	//create a texture to hold the linear depth buffer samples
	//texture for linear depth buffer visualisation
	glGenTextures(1, &m_FBO_Linear_Depth);

	// bind the texture for editing
	glBindTexture(GL_TEXTURE_2D, m_FBO_Linear_Depth);

	// create the texture. Note, the width and height are the dimensions of the screen and the final
	// data pointer is 0 as we aren't including any initial data
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, a_windowWidth, a_windowHeight, 0, GL_RED, GL_FLOAT, 0);

	// set the filtering if we intend to sample within a shader
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//\===========================================================================================================
}

void Model::SetupFrameBuffer(unsigned int& a_fbo, unsigned int a_targetWidth, unsigned int a_targetHeight, unsigned int& a_renderTexture, unsigned int& a_depthTexture)
{
	//\======================================================================================
// Create our frame buffer object
//\=====================================================================================
// this would be within your onCreate() function
	glGenFramebuffers(1, &a_fbo);

	// bind the framebuffer for editing
	glBindFramebuffer(GL_FRAMEBUFFER, a_fbo);

	if (a_renderTexture != -1)
	{
		// create a texture to be attached to the framebuffer, stored in the derived app class as a member
		glGenTextures(1, &a_renderTexture);

		// bind the texture for editing
		glBindTexture(GL_TEXTURE_2D, a_renderTexture);

		// create the texture. Note, the width and height are the dimensions of the screen and the final
		// data pointer is 0 as we aren't including any initial data
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, a_targetWidth, a_targetHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

		// set the filtering if we intend to sample within a shader
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, a_renderTexture, 0);
	}

	// m_FBO_depth would be a member for the derived application class
	glGenTextures(1, &a_depthTexture);
	glBindTexture(GL_TEXTURE_2D, a_depthTexture);

	// note the use of GL_DEPTH_COMPONENT32F and GL_DEPTH_COMPONENT
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, a_targetWidth, a_targetHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// attach it as a GL_DEPTH_ATTACHMENT
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, a_depthTexture, 0);

	// Here we tell the framebuffer which color attachments we will be drawing to and how many
	if (a_renderTexture != -1)
	{
		GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, drawBuffers);
	}
	else
	{
		glDrawBuffer(GL_NONE);
	}

	// if Status doesn't equal GL_FRAMEBUFFER_COMPLETE there has been an error when creating it
	/*GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		Application_Log* log = Application_Log::Get();
		if (log != nullptr)
		{
			log->addLog(LOG_LEVEL::LOG_ERROR, "Framebuffer Creation Error!");
		}
	}*/

	// binding 0 to the framebuffer slot unbinds the framebuffer and means future render calls will be sent to 
	// the back buffer
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void Model::Update(float a_deltaTime, glm::mat4 a_depthViewMatrix, glm::mat4 a_depthProjectionMatrix)
{
	m_shadowProjectionMatrix = a_depthProjectionMatrix * a_depthViewMatrix;

	//Update our FBX model
	for (unsigned int i = 0; i < m_fbxModel->getMeshCount(); ++i)
	{
		// get the current mesh
		FBXMeshNode* mesh = m_fbxModel->getMeshByIndex(i);
		// if you move an object around within your scene
		// children nodes are not updated until this function is called.
		mesh->updateGlobalTransform();
	}
}

void Model::Draw(glm::mat4 a_projectionViewMatrix, glm::mat4 a_cameraMatrix, DirectionalLight* a_directionalLight, PointLight* a_pointLight, SpotLight* a_spotLight, unsigned int a_shaderProgramID)
{
	//bind our shader program
	glUseProgram(a_shaderProgramID);

	glm::mat4 viewMatrix = glm::inverse(a_cameraMatrix);

	//bind our vertex array object
	glBindVertexArray(m_vao);

	//get our shaders uniform location for our projectionViewMatrix and then use glUniformMatrix4fv to fill it with the correct data
	unsigned int projectionViewUniform = glGetUniformLocation(a_shaderProgramID, "ProjectionView");
	glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(a_projectionViewMatrix * viewMatrix));

	//pass throught the view matrix
	unsigned int viewMatrixUniform = glGetUniformLocation(a_shaderProgramID, "ViewMatrix");
	glUniformMatrix4fv(viewMatrixUniform, 1, false, glm::value_ptr(viewMatrix));
	//pass throught the view matrix
	unsigned int shadowProjectionViewxUniform = glGetUniformLocation(a_shaderProgramID, "shadowProjectionView");
	glUniformMatrix4fv(shadowProjectionViewxUniform, 1, false, glm::value_ptr(m_shadowProjectionMatrix));

	//pass our camera position to our fragment shader uniform
	unsigned int cameraPosUniform = glGetUniformLocation(a_shaderProgramID, "cameraPosition");
	glUniform4fv(cameraPosUniform, 1, glm::value_ptr(a_cameraMatrix[3]));


	//pass the directional light to our shader program
	a_directionalLight->GetLightDirUniform(a_shaderProgramID);
	a_directionalLight->GetLightColourUniform(a_shaderProgramID, "uLightDiffuse");
	a_directionalLight->GetLightAmbientUniform(a_shaderProgramID, "uLightAmbient");
	a_directionalLight->GetLightSpecularUniform(a_shaderProgramID, "uLightSpecular");

	//pass the point light to our shader program
	a_pointLight->GetLightPositionUniform(a_shaderProgramID, "uPointLightPosition");
	a_pointLight->GetLightAmbientUniform(a_shaderProgramID, "uPointLightAmbient");
	a_pointLight->GetLightColourUniform(a_shaderProgramID, "uPointLightDiffuse");
	a_pointLight->GetLightSpecularUniform(a_shaderProgramID, "uPointLightSpecular");
	a_pointLight->GetConstantUniform(a_shaderProgramID);
	a_pointLight->GetLinearUniform(a_shaderProgramID);
	a_pointLight->GetQuadraticUniform(a_shaderProgramID);

	//pass the Spotlight to our shader program
	a_spotLight->GetLightPositionUniform(a_shaderProgramID, "uSpotLightPosition");
	a_spotLight->GetLightDirUniform(a_shaderProgramID);
	a_spotLight->GetLightAmbientUniform(a_shaderProgramID, "uSpotLightAmbient");
	a_spotLight->GetLightColourUniform(a_shaderProgramID, "uSpotLightDiffuse");
	a_spotLight->GetLightSpecularUniform(a_shaderProgramID, "uSpotLightSpecular");
	a_spotLight->GetConstantUniform(a_shaderProgramID);
	a_spotLight->GetLinearUniform(a_shaderProgramID);
	a_spotLight->GetQuadraticUniform(a_shaderProgramID);

	//set shadow texture
	unsigned int shadowTexUniformID = glGetUniformLocation(a_shaderProgramID, "ShadowTexture");
	glUniform1i(shadowTexUniformID, 1);
	//set our active texture, and bind our loaded texture
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_FBO_depth_texture);

	// for each mesh in the model...
	for (unsigned int i = 0; i < m_fbxModel->getMeshCount(); ++i)
	{
		// get the current mesh
		FBXMeshNode* pMesh = m_fbxModel->getMeshByIndex(i);

		// send the Model
		glm::mat4 m4Model = pMesh->m_globalTransform;// *m_modelMatrix;
		unsigned int modelUniform = glGetUniformLocation(a_shaderProgramID, "Model");
		glUniformMatrix4fv(modelUniform, 1, false, glm::value_ptr(m4Model));
		//send the normal matrix
		//this is only beneficial if a model has a non-uniform scale or non-orthoganal model matrix
		glm::mat4 m4Normal = glm::transpose(glm::inverse(pMesh->m_globalTransform));// *m_modelMatrix;
		unsigned int normalMatrixUniform = glGetUniformLocation(a_shaderProgramID, "NormalMatrix");
		glUniformMatrix4fv(normalMatrixUniform, 1, false, glm::value_ptr(m4Normal));

		// Bind the texture to one of the ActiveTextures
		// if your shader supported multiple textures, you would bind each texture to a new Active Texture ID here
		//bind our textureLocation variable from the shaders and set it's value to 0 as the active texture is texture 0
		unsigned int texUniformID = glGetUniformLocation(a_shaderProgramID, "DiffuseTexture");
		glUniform1i(texUniformID, 0);
		//set our active texture, and bind our loaded texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, pMesh->m_material->textureIDs[FBXMaterial::DiffuseTexture]);

		// Send the vertex data to the VBO
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, pMesh->m_vertices.size() * sizeof(FBXVertex), pMesh->m_vertices.data(), GL_STATIC_DRAW);

		// send the index data to the IBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, pMesh->m_indices.size() * sizeof(unsigned int), pMesh->m_indices.data(), GL_STATIC_DRAW);

		glDrawElements(GL_TRIANGLES, pMesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}
	glBindVertexArray(0);
	glUseProgram(0);
}
