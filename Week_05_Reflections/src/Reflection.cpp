#include "Reflection.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <iostream>
#include "Application_Log.h"

Reflection::Reflection()
{

}

Reflection::~Reflection()
{

}

bool Reflection::onCreate()
{
	// initialise the Gizmos helper class
	Gizmos::create();

	//Load the shaders for this program
	unsigned int vertexShader = Utility::loadShader("./resources/shaders/vertex.glsl", GL_VERTEX_SHADER);
	unsigned int fragmentShader = Utility::loadShader("./resources/shaders/fragment.glsl", GL_FRAGMENT_SHADER);
	//Define the input and output varialbes in the shaders
	//Note: these names are taken from the glsl files -- added in inputs for UV coordinates
	const char* szInputs[] = { "Position", "Colour", "Normal","Tex1" };
	const char* szOutputs[] = { "FragColor" };
	//bind the shaders to create our shader program
	m_fbo_program = Utility::createProgram(
		vertexShader,
		0,
		0,
		0,
		fragmentShader,
		4, szInputs, 1, szOutputs);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	vertexShader = Utility::loadShader("./resources/shaders/fs_vertex.glsl", GL_VERTEX_SHADER);
	fragmentShader = Utility::loadShader("./resources/shaders/fs_fragment.glsl", GL_FRAGMENT_SHADER);
	//Define the input and output varialbes in the shaders
	//Note: these names are taken from the glsl files -- added in inputs for UV coordinates
	const char* szInputs2[] = { "Position", "Tex1" };
	const char* szOutputs2[] = { "FragColor" };
	//bind the shaders to create our shader program
	m_programID = Utility::createProgram(
		vertexShader,
		0,
		0,
		0,
		fragmentShader,
		2, szInputs2, 1, szOutputs2);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	m_fbxModel = new FBXFile();
	m_fbxModel->load("./resources/models/stanford/Dragon.fbx", FBXFile::UNITS_DECIMETER);

	//Generate our OpenGL Vertex and Index Buffers for rendering our FBX Model Data
	// OPENGL: genorate the VBO, IBO and VAO
	glGenBuffers(2, m_vbo);
	glGenBuffers(2, m_ibo);
	glGenVertexArrays(2, m_vao);

	// OPENGL: Bind  VAO, and then bind the VBO and IBO to the VAO
	glBindVertexArray(m_vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo[0]);

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

	//we can describe the next mesh
	SVertex vertexData[4];
	vertexData[0] = SVertex(glm::vec4(-10.0f, 0.0f, -10.0f, 1.0f), glm::vec2(0.0f, 1.0f));
	vertexData[1] = SVertex(glm::vec4(-10.0f, 0.0f, 10.0f, 1.0f), glm::vec2(0.0f, 0.0f));
	vertexData[2] = SVertex(glm::vec4(10.0f, 0.0f, -10.0f, 1.0f), glm::vec2(1.0f, 1.0f));
	vertexData[3] = SVertex(glm::vec4(10.0f, 0.0f, 10.0f, 1.0f), glm::vec2(1.0f, 0.0f));

	unsigned int elements[6] = { 0, 1, 2, 1, 3, 2 };
	//Generate our OpenGL Vertex and Index Buffers for rendering our FBX Model Data
	// OPENGL: Bind  VAO, and then bind the VBO and IBO to the VAO
	glBindVertexArray(m_vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(SVertex), vertexData, GL_STATIC_DRAW);

	//send the index data to the IBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), elements, GL_STATIC_DRAW);

	//enable the attribute locations that will be used on our shader
	glEnableVertexAttribArray(0); //Pos
	glEnableVertexAttribArray(1); //Tex1

									  // tell our shaders where the information within our buffers lie
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(SVertex), ((char*)0) + SVertex::PositionOffset);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, sizeof(SVertex), ((char*)0) + SVertex::TexCoord1Offset);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// create a world-space matrix for a camera
	m_cameraMatrix = glm::inverse(glm::lookAt(glm::vec3(10, 10, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));

	// create a perspective projection matrix with a 90 degree field-of-view and widescreen aspect ratio
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, (float)m_windowWidth / (float)m_windowHeight, 0.1f, 1000.0f);

	m_modelMatrix = glm::mat4();

	Application_Log* log = Application_Log::Get();
	if (log != nullptr)
	{
		int maxAttachments = 0;
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxAttachments);
		log->addLog(LOG_LEVEL::LOG_INFO, "Num FBO colour attachments supported: %i", maxAttachments);
	}

	//Create out frame buffer object
	glGenFramebuffers(1, &m_fbo);

	//bind the framebuffer for edditing
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	//create a texture to be attached to the framebuffer,
	//stored in the derived app class as a number
	glGenTextures(1, &m_fbo_texture);

	//bind the texture for editing
	glBindTexture(GL_TEXTURE_2D, m_fbo_texture);

	//create the texture. Note, the width and height are the
	//dimensions of the screen and the final
	//data pointer is 0 as we aren't including any initial data
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_windowWidth, m_windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	//set the filtering if we intend to sample within a shader
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_fbo_texture, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//m_fbo_depth would be a member for the derived application class
	glGenTextures(1, &m_fbo_depth_texture);
	glBindTexture(GL_TEXTURE_2D, m_fbo_depth_texture);

	//notE the use of GL_DEPTH_COMPONENT32f and GL_DEPTH_COMPONENT
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, m_windowWidth, m_windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	//set the filtering if we intend to sample within a shader
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//attach the texture to the 0th colour attachment of the framebuffer
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_fbo_depth_texture, 0);

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
	glGenTextures(1, &m_fbo_linear_depth);
	glBindTexture(GL_TEXTURE_2D, m_fbo_linear_depth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_windowWidth,
		m_windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	// set the clear colour and enable depth testing and backface culling
	glClearColor(0.25f, 0.25f, 0.25f, 1.f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	m_lightPos = glm::vec4(50.f, 25.f, 0.f, 1.f);

	return true;
}

void Reflection::Update(float a_deltaTime)
{
	// update our camera matrix using the keyboard/mouse
	Utility::freeMovement(m_cameraMatrix, a_deltaTime, 10);

	// clear all gizmos from last frame
	Gizmos::clear();

	// add an identity matrix gizmo
	Gizmos::addTransform(glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1));

	// rotate point
	float s = sinf(a_deltaTime * 0.2f);
	float c = cosf(a_deltaTime * 0.2f);
	glm::vec4 temp = m_lightPos;
	m_lightPos.x = temp.x * c - temp.z * s;
	m_lightPos.z = temp.x * s + temp.z * c;
	m_lightPos.w = 0;
	m_lightPos = glm::normalize(m_lightPos) * 25.f;
	Gizmos::addBox(m_lightPos.xyz, glm::vec3(0.2f, 0.2f, 0.2f), true, glm::vec4(1.0f, 0.85f, 0.05f, 1.f));


	//Update our FBX model
	for (unsigned int i = 0; i < m_fbxModel->getMeshCount(); ++i)
	{
		// get the current mesh
		FBXMeshNode* mesh = m_fbxModel->getMeshByIndex(i);
		// if you move an object around within your scene
		// children nodes are not updated until this function is called.
		mesh->updateGlobalTransform();
	}

	//Imgui render view to show what is held in the FBO texture position
	ImGui::SetNextWindowPos(ImVec2(m_windowWidth - m_windowWidth * 0.3, m_windowHeight - m_windowHeight * 0.4));
	ImGui::SetNextWindowSize(ImVec2(m_windowWidth * 0.3, m_windowHeight * 0.4));
	ImGui::Begin("FrameBuffer");
	ImGui::BeginTabBar("FrameBuffer Textures");

	if (ImGui::BeginTabItem("Color Buffer"))
	{
		ImTextureID texID = (void*)(intptr_t)m_fbo_texture;
		ImGui::Image(texID, ImVec2(m_windowWidth * 0.25, m_windowHeight * 0.25), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem("Depth Buffer"))
	{
		ImTextureID texID = (void*)(intptr_t)m_fbo_linear_depth;
		ImGui::Image(texID, ImVec2(m_windowWidth * 0.25, m_windowHeight * 0.25), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
	ImGui::End();

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
}

void Reflection::Draw()
{
	//bind the frameBuffer for rendering 
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	// clear the backbuffer
	glClearColor(0.45f, 0.45f, 0.45f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse(m_cameraMatrix);

#pragma region First Pass Render
	//draw the model reflected around the origin to the framebuffer to sample in a later render pass
	glFrontFace(GL_CW);
	//bing our shader program
	glUseProgram(m_fbo_program);
	//bind our vertex array object
	glBindVertexArray(m_vao[0]);

	//get our shaders uniform location for our projectionViewMatrix and then use glUniformMatrix4fv to fill it with the correct data
	unsigned int projectionViewUniform = glGetUniformLocation(m_fbo_program, "ProjectionView");
	glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(m_projectionMatrix * viewMatrix));

	//pass throught the view matrix
	unsigned int viewMatrixUniform = glGetUniformLocation(m_fbo_program, "ViewMatrix");
	glUniformMatrix4fv(viewMatrixUniform, 1, false, glm::value_ptr(viewMatrix));

	//pass our camera position to our fragment shader uniform
	unsigned int cameraPosUniform = glGetUniformLocation(m_fbo_program, "cameraPosition");
	glUniform4fv(cameraPosUniform, 1, glm::value_ptr(m_cameraMatrix[3]));

	//pass the directional light direction to our fragment shader
	glm::vec4 lightDir = -m_lightPos;
	lightDir.w = 0.f;
	lightDir = glm::normalize(lightDir);
	unsigned int lightDirUniform = glGetUniformLocation(m_fbo_program, "lightDirection");
	glUniform4fv(lightDirUniform, 1, glm::value_ptr(lightDir));

	//Draw our FBX Model

	// for each mesh in the model...
	for (unsigned int i = 0; i < m_fbxModel->getMeshCount(); ++i)
	{
		// get the current mesh
		FBXMeshNode* pMesh = m_fbxModel->getMeshByIndex(i);

		glm::mat4 reflectionMatrix = glm::mat4(1.0);
		reflectionMatrix = glm::reflect3D(reflectionMatrix, glm::vec3(0, 1, 0));

		// send the Model
		glm::mat4 m4Model = pMesh->m_globalTransform;// *m_modelMatrix;
		unsigned int modelUniform = glGetUniformLocation(m_fbo_program, "Model");
		glUniformMatrix4fv(modelUniform, 1, false, glm::value_ptr(reflectionMatrix * m4Model));
		//send the normal matrix
		//this is only beneficial if a model has a non-uniform scale or non-orthoganal model matrix
		glm::mat4 m4Normal = glm::transpose(glm::inverse(pMesh->m_globalTransform));// *m_modelMatrix;
		unsigned int normalMatrixUniform = glGetUniformLocation(m_fbo_program, "NormalMatrix");
		glUniformMatrix4fv(normalMatrixUniform, 1, false, glm::value_ptr(m4Normal));

		// Send the vertex data to the VBO
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, pMesh->m_vertices.size() * sizeof(FBXVertex), pMesh->m_vertices.data(), GL_STATIC_DRAW);

		// send the index data to the IBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo[0]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, pMesh->m_indices.size() * sizeof(unsigned int), pMesh->m_indices.data(), GL_STATIC_DRAW);

		glDrawElements(GL_TRIANGLES, pMesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}

	glBindVertexArray(0);
	glUseProgram(0);
	glFrontFace(GL_CCW);

#pragma endregion

#pragma region Get Linear Depth Texture
	//get the recently created depth buffer texture from the FBO and linearise it
	glBindTexture(GL_TEXTURE_2D, m_fbo_depth_texture);
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

	glBindTexture(GL_TEXTURE_2D, m_fbo_linear_depth);
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
	//bing our shader program
	glUseProgram(m_fbo_program);
	//bind our vertex array object
	glBindVertexArray(m_vao[0]);

	//get our shaders uniform location for our projectionViewMatrix and then use glUniformMatrix4fv to fill it with the correct data
	projectionViewUniform = glGetUniformLocation(m_fbo_program, "ProjectionView");
	glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(m_projectionMatrix * viewMatrix));

	//pass throught the view matrix
	viewMatrixUniform = glGetUniformLocation(m_fbo_program, "ViewMatrix");
	glUniformMatrix4fv(viewMatrixUniform, 1, false, glm::value_ptr(viewMatrix));

	//pass our camera position to our fragment shader uniform
	cameraPosUniform = glGetUniformLocation(m_fbo_program, "cameraPosition");
	glUniform4fv(cameraPosUniform, 1, glm::value_ptr(m_cameraMatrix[3]));


	//pass the directional light direction to our fragment shader
	lightDirUniform = glGetUniformLocation(m_fbo_program, "lightDirection");
	glUniform4fv(lightDirUniform, 1, glm::value_ptr(lightDir));

	//Draw our FBX Model

	// for each mesh in the model...
	for (unsigned int i = 0; i < m_fbxModel->getMeshCount(); ++i)
	{
		// get the current mesh
		FBXMeshNode* pMesh = m_fbxModel->getMeshByIndex(i);
		// send the Model
		glm::mat4 m4Model = pMesh->m_globalTransform;// *m_modelMatrix;
		unsigned int modelUniform = glGetUniformLocation(m_fbo_program, "Model");
		glUniformMatrix4fv(modelUniform, 1, false, glm::value_ptr(m4Model));
		//send the normal matrix
		//this is only beneficial if a model has a non-uniform scale or non-orthoganal model matrix
		glm::mat4 m4Normal = glm::transpose(glm::inverse(pMesh->m_globalTransform));// *m_modelMatrix;
		unsigned int normalMatrixUniform = glGetUniformLocation(m_fbo_program, "NormalMatrix");
		glUniformMatrix4fv(normalMatrixUniform, 1, false, glm::value_ptr(m4Normal));

		// Send the vertex data to the VBO
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, pMesh->m_vertices.size() * sizeof(FBXVertex), pMesh->m_vertices.data(), GL_STATIC_DRAW);

		// send the index data to the IBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo[0]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, pMesh->m_indices.size() * sizeof(unsigned int), pMesh->m_indices.data(), GL_STATIC_DRAW);

		glDrawElements(GL_TRIANGLES, pMesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}
	glBindVertexArray(0);
	glUseProgram(0);
#pragma endregion

	//render the plane into the scene using the "full screen" shader
	//this will render the plane into the scene under the model using the texture from the fbo render pass wehere we flipped everything
	//we could have rendered this prior to drawing the model the right way up as the depth buffer would sort things out when drawing vertices and rasterising

#pragma region Fullscreen Render Pass
	glUseProgram(m_programID);

	projectionViewUniform = glGetUniformLocation(m_programID, "ProjectionView");
	glUniformMatrix4fv(projectionViewUniform, 1,
		false, glm::value_ptr(m_projectionMatrix * viewMatrix));

	glm::mat4 m4Model = glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);// *m_modelMatrix
	unsigned int modelUniform = glGetUniformLocation(m_programID, "Model");
	glUniformMatrix4fv(modelUniform, 1, false, glm::value_ptr(m4Model));

	unsigned int windowWUniform = glGetUniformLocation(m_programID, "windowWidth");
	unsigned int windowHUniform = glGetUniformLocation(m_programID, "windowHeight");
	glUniform1i(windowWUniform, m_windowWidth);
	glUniform1i(windowHUniform, m_windowHeight);

	glBindVertexArray(m_vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo[1]);
	unsigned int texUniformID = glGetUniformLocation(m_programID, "DiffuseTexture");
	glUniform1i(texUniformID, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glUseProgram(0);

#pragma endregion 

	// draw the gizmos from this frame
	Gizmos::draw(viewMatrix, m_projectionMatrix);
}

void Reflection::Destroy()
{
	m_fbxModel->unload();
	delete m_fbxModel;

	glDeleteBuffers(2, m_vbo);
	glDeleteBuffers(2, m_ibo);
	glDeleteVertexArrays(2, m_vao);
	glDeleteProgram(m_fbo_program);
	glDeleteProgram(m_programID);


	Gizmos::destroy();
}