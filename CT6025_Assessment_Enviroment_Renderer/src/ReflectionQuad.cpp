#include "ReflectionQuad.h"
#include "Model.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

ReflectionQuad::ReflectionQuad()
{
	//Generate our OpenGL Vertex and Index Buffers for rendering our FBX Model Data
	// OPENGL: genorate the VBO, IBO and VAO
	glGenBuffers(2, m_reflectionQuad_vbo);
	glGenBuffers(2, m_reflectionQuad_ibo);
	glGenVertexArrays(2, m_reflectionQuad_vao);
}

ReflectionQuad::~ReflectionQuad()
{
	glDeleteBuffers(2, m_reflectionQuad_vbo);
	glDeleteBuffers(2, m_reflectionQuad_ibo);
	glDeleteVertexArrays(2, m_reflectionQuad_vao);
	glDeleteFramebuffers(2, &m_reflectionQuad_fbo);
}

void ReflectionQuad::CreateModelBuffer()
{
	// OPENGL: Bind  VAO, and then bind the VBO and IBO to the VAO
	glBindVertexArray(m_reflectionQuad_vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, m_reflectionQuad_vbo[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_reflectionQuad_ibo[0]);

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
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ReflectionQuad::CreateQuad(unsigned int a_width, unsigned int a_height)
{
	//we can describe the next mesh
	ReflectionVertex vertexData[4];
	vertexData[0] = ReflectionVertex(glm::vec4(-10.0f, -5.6f, -10.0f, 1.0f), glm::vec2(0.0f, 1.0f));
	vertexData[1] = ReflectionVertex(glm::vec4(-10.0f, -5.6f, 10.0f, 1.0f), glm::vec2(0.0f, 0.0f));
	vertexData[2] = ReflectionVertex(glm::vec4(10.0f, -5.6f, -10.0f, 1.0f), glm::vec2(1.0f, 1.0f));
	vertexData[3] = ReflectionVertex(glm::vec4(10.0f, -5.6f, 10.0f, 1.0f), glm::vec2(1.0f, 0.0f));

	unsigned int elements[6] = { 0, 1, 2, 1, 3, 2 };
	//Generate our OpenGL Vertex and Index Buffers for rendering our FBX Model Data
	// OPENGL: Bind  VAO, and then bind the VBO and IBO to the VAO
	glBindVertexArray(m_reflectionQuad_vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, m_reflectionQuad_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(ReflectionVertex), vertexData, GL_STATIC_DRAW);

	//send the index data to the IBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_reflectionQuad_ibo[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), elements, GL_STATIC_DRAW);

	//enable the attribute locations that will be used on our shader
	glEnableVertexAttribArray(0); //Pos
	glEnableVertexAttribArray(1); //Tex1

									  // tell our shaders where the information within our buffers lie
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(ReflectionVertex), ((char*)0) + ReflectionVertex::PositionOffset);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, sizeof(ReflectionVertex), ((char*)0) + ReflectionVertex::TexCoord1Offset);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ReflectionQuad::CreateOutputFrameBuffer(unsigned int a_windowWidth, unsigned int a_windowHeight)
{
	m_windowWidth = a_windowWidth;
	m_windowHeight = a_windowHeight;

	//Create out frame buffer object
	glGenFramebuffers(1, &m_reflectionQuad_fbo);

	//bind the framebuffer for edditing
	glBindFramebuffer(GL_FRAMEBUFFER, m_reflectionQuad_fbo);

	//create a texture to be attached to the framebuffer,
	//stored in the derived app class as a number
	glGenTextures(1, &m_reflectionQuad_fbo_texture);

	//bind the texture for editing
	glBindTexture(GL_TEXTURE_2D, m_reflectionQuad_fbo_texture);

	//create the texture. Note, the width and height are the
	//dimensions of the screen and the final
	//data pointer is 0 as we aren't including any initial data
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_windowWidth, m_windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	//set the filtering if we intend to sample within a shader
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_reflectionQuad_fbo_texture, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//m_fbo_depth would be a member for the derived application class
	glGenTextures(1, &m_reflectionQuad_fbo_depth_texture);
	glBindTexture(GL_TEXTURE_2D, m_reflectionQuad_fbo_depth_texture);

	//notE the use of GL_DEPTH_COMPONENT32f and GL_DEPTH_COMPONENT
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, m_windowWidth, m_windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	//set the filtering if we intend to sample within a shader
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//attach the texture to the 0th colour attachment of the framebuffer
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_reflectionQuad_fbo_depth_texture, 0);

	glBindTexture(GL_TEXTURE_2D, 0);

	//here we tell the framebuffer which color attachment we will be drawing to and how many
	GLenum drawbuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawbuffers);

	//if status doesn't equal GL_FRAMEBUFFER_COMPLETE there has been an error when creating it
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Framebuffer Error!\n");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//Create the linear depth texture
	//generate the texture here to avoid any gl warnings
	glGenTextures(1, &m_reflectionQuad_fbo_linear_depth);
	glBindTexture(GL_TEXTURE_2D, m_reflectionQuad_fbo_linear_depth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_windowWidth,
		m_windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

}

void ReflectionQuad::DrawReflection(glm::mat4 a_projectionViewMatrix, glm::mat4 a_cameraMatrix, glm::vec4 a_lightPos)
{
	m_reflectionQuad_fbxModel = m_reflectedModel->GetModelData();

	//pass the directional light direction to our fragment shader
	glm::vec4 lightDir = -a_lightPos;
	lightDir.w = 0.f;
	lightDir = glm::normalize(lightDir);

	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse(a_cameraMatrix);

	//bind the frameBuffer for rendering 
	glBindFramebuffer(GL_FRAMEBUFFER, m_reflectionQuad_fbo);

	// clear the backbuffer
	glClearColor(0.f, 0.f, 0.f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	

#pragma region First Pass Render
	//draw the model reflected around the origin to the framebuffer to sample in a later render pass
	glFrontFace(GL_CW);
	//bing our shader program
	glUseProgram(m_reflectionQuad_fbo_programID);
	//bind our vertex array object
	glBindVertexArray(m_reflectionQuad_vao[0]);

	//get our shaders uniform location for our projectionViewMatrix and then use glUniformMatrix4fv to fill it with the correct data
	unsigned int projectionViewUniform = glGetUniformLocation(m_reflectionQuad_fbo_programID, "ProjectionView");
	glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(a_projectionViewMatrix * viewMatrix));

	//pass throught the view matrix
	unsigned int viewMatrixUniform = glGetUniformLocation(m_reflectionQuad_fbo_programID, "ViewMatrix");
	glUniformMatrix4fv(viewMatrixUniform, 1, false, glm::value_ptr(viewMatrix));

	//pass our camera position to our fragment shader uniform
	unsigned int cameraPosUniform = glGetUniformLocation(m_reflectionQuad_fbo_programID, "cameraPosition");
	glUniform4fv(cameraPosUniform, 1, glm::value_ptr(a_cameraMatrix[3]));

	
	unsigned int lightDirUniform = glGetUniformLocation(m_reflectionQuad_fbo_programID, "lightDirection");
	glUniform4fv(lightDirUniform, 1, glm::value_ptr(lightDir));

	//Draw our FBX Model

	// for each mesh in the model...
	for (unsigned int i = 0; i < m_reflectionQuad_fbxModel->getMeshCount(); ++i)
	{
		// get the current mesh
		FBXMeshNode* pMesh = m_reflectionQuad_fbxModel->getMeshByIndex(i);

		glm::mat4 reflectionMatrix = glm::mat4(1.0);
		reflectionMatrix = glm::reflect3D(reflectionMatrix, glm::vec3(0, 1, 0));

		// send the Model
		glm::mat4 m4Model = pMesh->m_globalTransform;// *m_modelMatrix;
		unsigned int modelUniform = glGetUniformLocation(m_reflectionQuad_fbo_programID, "Model");
		glUniformMatrix4fv(modelUniform, 1, false, glm::value_ptr(reflectionMatrix * m4Model));
		//send the normal matrix
		//this is only beneficial if a model has a non-uniform scale or non-orthoganal model matrix
		glm::mat4 m4Normal = glm::transpose(glm::inverse(pMesh->m_globalTransform));// *m_modelMatrix;
		unsigned int normalMatrixUniform = glGetUniformLocation(m_reflectionQuad_fbo_programID, "NormalMatrix");
		glUniformMatrix4fv(normalMatrixUniform, 1, false, glm::value_ptr(m4Normal));

		// Send the vertex data to the VBO
		glBindBuffer(GL_ARRAY_BUFFER, m_reflectionQuad_vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, pMesh->m_vertices.size() * sizeof(FBXVertex), pMesh->m_vertices.data(), GL_STATIC_DRAW);

		// send the index data to the IBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_reflectionQuad_ibo[0]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, pMesh->m_indices.size() * sizeof(unsigned int), pMesh->m_indices.data(), GL_STATIC_DRAW);

		glDrawElements(GL_TRIANGLES, pMesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}

	glBindVertexArray(0);
	glUseProgram(0);
	glFrontFace(GL_CCW);

#pragma endregion

#pragma region Get Linear Depth Texture
	//get the recently created depth buffer texture from the FBO and linearise it
	glBindTexture(GL_TEXTURE_2D, m_reflectionQuad_fbo_depth_texture);
	//Create a glfloat (float) buffer/array to store the depth texture
	GLfloat* pixels = new GLfloat[m_windowWidth * m_windowHeight];
	//get the depth texture data from the gpu
	glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, pixels);
	//linerise each pixel value
	float zNear = 0.1f;
	float zFar = 100.f;
	for (int i = 0; i < m_windowWidth * m_windowHeight; ++i)
	{
		float depthSample = pixels[i];
		float zLinear = (2.0 * zNear) / (zFar + zNear - depthSample * (zFar - zNear));
		pixels[i] = zLinear;
	}

	glBindTexture(GL_TEXTURE_2D, m_reflectionQuad_fbo_linear_depth);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_windowWidth, m_windowHeight, GL_DEPTH_COMPONENT, GL_FLOAT, pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	delete[] pixels;
#pragma endregion

	//bind the framebuffer for reading
	//switch back to rendering to the back buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#pragma region Second Pass Render -- Draw the Model non-reflected
	/*//bing our shader program
	glUseProgram(m_reflectionQuad_fbo_programID);
	//bind our vertex array object
	glBindVertexArray(m_reflectionQuad_vao[0]);

	//get our shaders uniform location for our projectionViewMatrix and then use glUniformMatrix4fv to fill it with the correct data
	projectionViewUniform = glGetUniformLocation(m_reflectionQuad_fbo_programID, "ProjectionView");
	glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(a_projectionViewMatrix * viewMatrix));

	//pass throught the view matrix
	viewMatrixUniform = glGetUniformLocation(m_reflectionQuad_fbo_programID, "ViewMatrix");
	glUniformMatrix4fv(viewMatrixUniform, 1, false, glm::value_ptr(viewMatrix));

	//pass our camera position to our fragment shader uniform
	cameraPosUniform = glGetUniformLocation(m_reflectionQuad_fbo_programID, "cameraPosition");
	glUniform4fv(cameraPosUniform, 1, glm::value_ptr(a_cameraMatrix[3]));


	//pass the directional light direction to our fragment shader
	lightDirUniform = glGetUniformLocation(m_reflectionQuad_fbo_programID, "lightDirection");
	glUniform4fv(lightDirUniform, 1, glm::value_ptr(lightDir));

	//Draw our FBX Model

	// for each mesh in the model...
	for (unsigned int i = 0; i < m_reflectionQuad_fbxModel->getMeshCount(); ++i)
	{
		// get the current mesh
		FBXMeshNode* pMesh = m_reflectionQuad_fbxModel->getMeshByIndex(i);
		// send the Model
		glm::mat4 m4Model = pMesh->m_globalTransform;// *m_modelMatrix;
		unsigned int modelUniform = glGetUniformLocation(m_reflectionQuad_fbo_programID, "Model");
		glUniformMatrix4fv(modelUniform, 1, false, glm::value_ptr(m4Model));
		//send the normal matrix
		//this is only beneficial if a model has a non-uniform scale or non-orthoganal model matrix
		glm::mat4 m4Normal = glm::transpose(glm::inverse(pMesh->m_globalTransform));// *m_modelMatrix;
		unsigned int normalMatrixUniform = glGetUniformLocation(m_reflectionQuad_fbo_programID, "NormalMatrix");
		glUniformMatrix4fv(normalMatrixUniform, 1, false, glm::value_ptr(m4Normal));

		// Send the vertex data to the VBO
		glBindBuffer(GL_ARRAY_BUFFER, m_reflectionQuad_vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, pMesh->m_vertices.size() * sizeof(FBXVertex), pMesh->m_vertices.data(), GL_STATIC_DRAW);

		// send the index data to the IBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_reflectionQuad_ibo[0]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, pMesh->m_indices.size() * sizeof(unsigned int), pMesh->m_indices.data(), GL_STATIC_DRAW);

		glDrawElements(GL_TRIANGLES, pMesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}
	glBindVertexArray(0);
	glUseProgram(0);*/
#pragma endregion

#pragma region Fullscreen Render Pass
	

#pragma endregion 
}

void ReflectionQuad::DrawFullscreenQuad(glm::mat4 a_projectionViewMatrix, glm::mat4 a_cameraMatrix)
{
	//render the plane into the scene using the "full screen" shader
	glUseProgram(m_reflectionQuad_fs_programID);

	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse(a_cameraMatrix);

	unsigned int projectionViewUniform = glGetUniformLocation(m_reflectionQuad_fs_programID, "ProjectionView");
	glUniformMatrix4fv(projectionViewUniform, 1,
		false, glm::value_ptr(a_projectionViewMatrix * viewMatrix));

	glm::mat4 m4Model = glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);// *m_modelMatrix
	unsigned int modelUniform = glGetUniformLocation(m_reflectionQuad_fs_programID, "Model");
	glUniformMatrix4fv(modelUniform, 1, false, glm::value_ptr(m4Model));

	unsigned int windowWUniform = glGetUniformLocation(m_reflectionQuad_fs_programID, "windowWidth");
	unsigned int windowHUniform = glGetUniformLocation(m_reflectionQuad_fs_programID, "windowHeight");
	glUniform1i(windowWUniform, m_windowWidth);
	glUniform1i(windowHUniform, m_windowHeight);

	glBindVertexArray(m_reflectionQuad_vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, m_reflectionQuad_vbo[1]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_reflectionQuad_ibo[1]);
	unsigned int texUniformID = glGetUniformLocation(m_reflectionQuad_fs_programID, "DiffuseTexture");
	glUniform1i(texUniformID, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_reflectionQuad_fbo_texture);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}
