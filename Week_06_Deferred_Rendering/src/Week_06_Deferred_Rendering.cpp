#include "Week_06_Deferred_Rendering.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <iostream>
#include <SVertex.h>

#include "Application_Log.h"

Week_06_Deferred_Rendering::Week_06_Deferred_Rendering()
{

}

Week_06_Deferred_Rendering::~Week_06_Deferred_Rendering()
{

}

bool Week_06_Deferred_Rendering::onCreate()
{

	//switch back to rendering to the back buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//bind m_fbo_texture as a texture input so we can use it when rendering to the back buffer
	glBindTexture(GL_TEXTURE_2D, m_fbo);


	// initialise the Gizmos helper class
	Gizmos::create();

	//load our model
	m_fbxModel = new FBXFile();
	m_fbxModel->load("./resources/models/stanford/Dragon.fbx", FBXFile::UNITS_DECIMETER);

	/*
	make the usual rending calls:
	- clear the screen
	- set a shader program
	- set uniforms
	- bind VAO
	- call GLDRAW
	*/


	//load the shaders for this program
	//vertex pass shaders
	unsigned int vertexShader = Utility::loadShader("./resources/shaders/geompass_vertex.glsl", GL_VERTEX_SHADER);
	unsigned int fragmentShader = Utility::loadShader("./resources/shaders/geompass_fragment.glsl", GL_FRAGMENT_SHADER);
	//define inputs and outputs variables in the shaders
	const char* szInputs[] = { "Position", "Colour", "Normal", "Tex1" };
	const char* szOutputs[] = { "Diffuse", "Normal", "World" };

	//generate program
	m_geomPassProgramID = Utility::createProgram(vertexShader, 0, 0, 0, fragmentShader, 4, szInputs, 3, szOutputs);

	//free shaders
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//Generate our OpenGL Vertex and Index Buffers for rendering our FBX Model Data
	// OPENGL: genorate the VBO, IBO and VAO
	glGenBuffers(2, m_vbo);
	glGenBuffers(2, m_ibo);
	glGenVertexArrays(2, m_vao);

	// OPENGL: Bind  VAO, and then bind the VBO and IBO to the VAO for first pass render
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
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char *)0) + FBXVertex::PositionOffset);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char *)0) + FBXVertex::ColourOffset);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char *)0) + FBXVertex::NormalOffset);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char *)0) + FBXVertex::TexCoord1Offset);


	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// finally, were done describing our mesh to the shader
	//We can describe the next mesh
	SVertex vertexData[4];
	vertexData[0] = SVertex(glm::vec4(-1.f, -1.f, 0.f, 1.f), glm::vec2(0.f, 0.f));
	vertexData[1] = SVertex(glm::vec4( 1.f, -1.f, 0.f, 1.f), glm::vec2(1.f, 0.f));
	vertexData[2] = SVertex(glm::vec4(-1.f,  1.f, 0.f, 1.f), glm::vec2(0.f, 1.f));
	vertexData[3] = SVertex(glm::vec4( 1.f,  1.f, 0.f, 1.f), glm::vec2(1.f, 1.f));

	unsigned int elements[6] = { 0, 1, 2, 1, 3, 2 };

	//Generate our openGL Vertex and index buffers for rendering our FBX Model Data
	//GOPENGL: Bind VAO, and then bind the VBO and IBO to the VAO
	glBindVertexArray(m_vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(SVertex), vertexData, GL_STATIC_DRAW);

	//Send the index data to the IBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), elements, GL_STATIC_DRAW);

	//Enable the attribute locations that will be used on our shaders
	glEnableVertexAttribArray(0); //pos
	glEnableVertexAttribArray(1); //tex1

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

	//Generate the frame buffer object
	CreateFrameBufferObject(m_fbo, m_fbo_diffuse, m_fbo_normal, m_fbo_worldPosition, m_fbo_depth);

	// set the clear colour and enable depth testing and backface culling
	glClearColor(0.25f, 0.25f, 0.25f, 1.f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	m_lightPos = glm::vec4(50.f, 25.f, 0.f, 1.f);

	return true;
}

void Week_06_Deferred_Rendering::Update(float a_deltaTime)
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
		FBXMeshNode *mesh = m_fbxModel->getMeshByIndex(i);
		// if you move an object around within your scene
		// children nodes are not updated until this function is called.
		mesh->updateGlobalTransform();
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
}

void Week_06_Deferred_Rendering::Draw()
{
	//bind the framebuffer for rendering
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse(m_cameraMatrix);

#pragma region First Pass Render
	//bind our shader program
	glUseProgram(m_fboProgramID);
	//bind our vertex array object
	glBindVertexArray(m_vao[0]);

	//get our shaders uniform location for our projectionViewMatrix and then use glUniformMatrix4fv to fill it with the correct data
	unsigned int projectionViewUniform = glGetUniformLocation(m_fboProgramID, "ProjectionView");
	glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(m_projectionMatrix * viewMatrix));

	//pass throught the view matrix
	unsigned int viewMatrixUniform = glGetUniformLocation(m_fboProgramID, "ViewMatrix");
	glUniformMatrix4fv(viewMatrixUniform, 1, false, glm::value_ptr(viewMatrix));

	//pass our camera position to our fragment shader uniform
	unsigned int cameraPosUniform = glGetUniformLocation(m_fboProgramID, "cameraPosition");
	glUniform4fv(cameraPosUniform, 1, glm::value_ptr(m_cameraMatrix[3]));


	//pass the directional light direction to our fragment shader
	glm::vec4 lightDir = -m_lightPos;
	lightDir.w = 0.f;
	lightDir = glm::normalize(lightDir);
	unsigned int lightDirUniform = glGetUniformLocation(m_fboProgramID, "lightDirection");
	glUniform4fv(lightDirUniform, 1, glm::value_ptr(lightDir));

	//Draw our FBX Model

	// for each mesh in the model...
	for (unsigned int i = 0; i < m_fbxModel->getMeshCount(); ++i)
	{
		// get the current mesh
		FBXMeshNode *pMesh = m_fbxModel->getMeshByIndex(i);

		// send the Model
		glm::mat4 m4Model = pMesh->m_globalTransform;// *m_modelMatrix;
		unsigned int modelUniform = glGetUniformLocation(m_fboProgramID, "Model");
		glUniformMatrix4fv(modelUniform, 1, false, glm::value_ptr(m4Model));
		//send the normal matrix
		//this is only beneficial if a model has a non-uniform scale or non-orthoganal model matrix
		glm::mat4 m4Normal = glm::transpose(glm::inverse(pMesh->m_globalTransform));// *m_modelMatrix;
		unsigned int normalMatrixUniform = glGetUniformLocation(m_fboProgramID, "NormalMatrix");
		glUniformMatrix4fv(normalMatrixUniform, 1, false, glm::value_ptr(m4Normal));

		// Bind the texture to one of the ActiveTextures
		// if your shader supported multiple textures, you would bind each texture to a new Active Texture ID here
		//bind our textureLocation variable from the shaders and set it's value to 0 as the active texture is texture 0
		unsigned int texUniformID = glGetUniformLocation(m_fboProgramID, "DiffuseTexture");
		glUniform1i(texUniformID, 0);
		//set our active texture, and bind our loaded texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, pMesh->m_material->textureIDs[FBXMaterial::DiffuseTexture]);

		// Send the vertex data to the VBO
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, pMesh->m_vertices.size() * sizeof(FBXVertex), pMesh->m_vertices.data(), GL_STATIC_DRAW);

		// send the index data to the IBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo[0]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, pMesh->m_indices.size() * sizeof(unsigned int), pMesh->m_indices.data(), GL_STATIC_DRAW);

		glDrawElements(GL_TRIANGLES, pMesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}

	// draw the gizmos from this frame
	Gizmos::draw(viewMatrix, m_projectionMatrix);
#pragma endregion

	//bind the framebuffer for rendering
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.25f, 0.25f, 0.25f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_fsProgramID);
	glBindVertexArray(m_vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo[1]);

	unsigned int texUniformLocation = glGetUniformLocation(m_fsProgramID, "u_diffuseTexture");
	glUniform1i(texUniformLocation, 0);
	//set our active texture and bind our loaded texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_fbo);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glUseProgram(0);
}

void Week_06_Deferred_Rendering::Destroy()
{

	m_fbxModel->unload();
	delete m_fbxModel;

	glDeleteBuffers(2, m_vbo);
	glDeleteBuffers(2, m_ibo);
	glDeleteVertexArrays(2, m_vao);
	glDeleteProgram(m_fboProgramID);
	glDeleteProgram(m_fsProgramID);

	Gizmos::destroy();
}

void Week_06_Deferred_Rendering::CreateFrameBufferObject(unsigned int a_fbo, unsigned int a_diffuse, unsigned int a_normal, unsigned int a_worldPos, unsigned int a_depth)
{
	//Create our frame buffer object
	int maxAttachments = 0;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxAttachments);
	glGenFramebuffers(1, &a_fbo);
	//bind the framebuffer for editing
	glBindFramebuffer(GL_FRAMEBUFFER, a_fbo);
	//Create a texture to be attatched to the framebuffer stored in the derived app class as a member
	//generate the textures we need, these should be member variables in the class for your app
	glGenTextures(1, &a_diffuse);
	glGenTextures(1, &a_normal);
	glGenTextures(1, &a_worldPos);
	glGenTextures(1, &a_depth);

	//creating and binding the textures for the diffuse data to GL_COLOR_ATTATCHMENT0
	glBindTexture(GL_TEXTURE_2D, a_diffuse);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_windowWidth, m_windowHeight, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, a_diffuse, 0);

	//creating and binding the textures for the normal data to GL_COLOR_ATTATCHMENT1
	glBindTexture(GL_TEXTURE_2D, a_normal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_windowWidth, m_windowHeight, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, a_normal, 0);

	//creating and binding the textures for the worldpos data to GL_COLOR_ATTATCHMENT2
	glBindTexture(GL_TEXTURE_2D, a_worldPos);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_windowWidth, m_windowHeight, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, a_worldPos, 0);

	//creating and binding the textures for the depth data to
	glBindTexture(GL_TEXTURE_2D, a_depth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, m_windowWidth, m_windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, a_depth, 0);
	glBindTexture(GL_TEXTURE_2D, 0)
		;
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(1, drawBuffers);

	//if status != GL_FRAMEBUFFER_COMPLETE there has been an error creating it
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		Application_Log* log = Application_Log::Get();
		if (log != nullptr)
		{
			log->addLog(LOG_LEVEL::LOG_ERROR, "ERROR CREATING FRAMEBUFFER OBJECT");
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

