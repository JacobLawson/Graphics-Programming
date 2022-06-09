#include "CT6025_Assessment_Enviroment_Renderer.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <iostream>
#include <imgui.h>

#include "Camera.h"
#include "Skybox.h"
#include "Texture.h"
#include "WaterQuad.h"
#include "ReflectionQuad.h"
#include "Model.h"
#include "ParticleEmitter.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

#include "Application_Log.h"


//#ifndef STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_IMPLEMENTATION
//#endif
//#include <stb_image.h>

#define SHADOW_BUFFER_SIZE 1024

CT6025_Assessment_Enviroment_Renderer::CT6025_Assessment_Enviroment_Renderer()
{
	m_bRenderWireFrame = false;
}

CT6025_Assessment_Enviroment_Renderer::~CT6025_Assessment_Enviroment_Renderer()
{
}

bool CT6025_Assessment_Enviroment_Renderer::onCreate()
{
	// initialise the Gizmos helper class
	Gizmos::create();

	//Camera
	m_mainCamera = new Camera;
	m_mainCamera->setMatrix(glm::inverse(glm::lookAt(glm::vec3(10, 10, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0))));

	// create a perspective projection matrix with a 90 degree field-of-view and widescreen aspect ratio
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, (float)m_windowWidth / (float)m_windowHeight, 0.1f, 1000.0f);

	//Directional Light
	m_directionalLight = new DirectionalLight;
	m_directionalLight->SetLightPos(glm::vec4(50.f, 25.f, 0.f, 1.f));

	//Point Light
	m_pointLight = new PointLight;
	m_pointLight->SetLightPos(glm::vec4(-0.67f, 0.6f, -1.47f, 1.f));

	//Spot Light
	m_spotLight = new SpotLight;
	m_spotLight->SetLightPos(glm::vec4(0.f, 3.f, 0.f, 0.f));

	//sky box
	m_skyBox = new SkyBox;
	m_skyBox->GenerateSkyBoxBuffers();
	//Set Images
	std::vector<std::string> textureFaces = { "./resources/skybox/right.jpg", "./resources/skybox/left.jpg",
											  "./resources/skybox/top.jpg", "./resources/skybox/bottom.jpg",
											  "./resources/skybox/front.jpg", "./resources/skybox/back.jpg" };
	m_skyBox->LoadSkyBox(textureFaces);

	//Water
	m_water = new WaterQuad;
	m_water->CreateQuad(60, 60);
	m_water->LoadTexture("./resources/images/Water/Water_COLOR.jpg", "./resources/images/Water/Water_NORM.jpg", "./resources/images/Water/Water_SPEC.jpg",
						 "./resources/images/Water/Water_DISP.png", "./resources/images/Water/Water_OCC.jpg");

	//Ember Particles
	m_EmberParticleEmitter = new ParticleEmitter;
	m_EmberParticleEmitter->InitialiseParticles();
	m_EmberParticleEmitter->LoadParticleTextures("./resources/images/position.png", "./resources/images/starAlpha.png");

	//Scene Model
	m_sceneModel = new Model;
	//m_sceneModel->Load("./resources/models/ruinedtank/tank.fbx");
	m_sceneModel->Load("./resources/models/scene/source/Scene.obj");
	m_sceneModel->Initialise(m_windowWidth, m_windowHeight, 1024);

	m_reflection = new ReflectionQuad;
	m_reflection->SetModel(m_sceneModel);
	m_reflection->CreateModelBuffer();
	m_reflection->CreateQuad(2, 2);
	m_reflection->CreateOutputFrameBuffer(m_windowWidth, m_windowHeight);

	SetupShaders();	

	// set the clear colour and enable depth testing and backface culling
	glClearColor(0.25f, 0.25f, 0.25f, 1.f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	return true;
}

void CT6025_Assessment_Enviroment_Renderer::Update(float a_deltaTime)
{
	// update our camera matrix using the keyboard/mouse
	m_mainCamera->Update(a_deltaTime);
	m_directionalLight->Update(a_deltaTime);
	m_spotLight->Update(a_deltaTime);
	m_water->Update(a_deltaTime);

	// clear all gizmos from last frame
	Gizmos::clear();

	// add an identity matrix gizmo
	Gizmos::addTransform(glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1));

	m_directionalLight->VisualisePosition();
	m_pointLight->VisualisePosition();
	m_spotLight->VisualisePosition();

	//Update the shadow projection view matrix
	glm::mat4 depthViewMatrix = glm::lookAt(glm::vec3(m_directionalLight->GetLightPos().xyz), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 depthProjectionMatrix = glm::ortho<float>(-30, 30, -30, 30, -100, 100);

	m_sceneModel->Update(a_deltaTime, depthViewMatrix, depthProjectionMatrix);
	
	//IMGUI Calls
	//Light Settings
	GUI_Lighting();
	//Rendering Settings
	GUI_Tesselation();

	if (m_bRenderWireFrame)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	

	static bool show_demo_window = true;
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

void CT6025_Assessment_Enviroment_Renderer::Draw()
{
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Draw Scene Model
	//use deffered rendering to render scene in several passes
	//1 - render reflection
	//2 - render shadows
	//3 - render models
	//reflection pass - draw model reflected
	m_reflection->DrawReflection(m_projectionMatrix, m_mainCamera->GetMatrix(), m_directionalLight->GetLightPos());
	m_reflection->DrawFullscreenQuad(m_projectionMatrix, m_mainCamera->GetMatrix());

	m_skyBox->Draw(m_projectionMatrix, m_mainCamera->GetMatrix());

	//shadows pass - draw the scene from the POV of the light
	glBindFramebuffer(GL_FRAMEBUFFER, m_sceneModel->GetModelFBO());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SHADOW_BUFFER_SIZE, SHADOW_BUFFER_SIZE);
	glCullFace(GL_FRONT);
	m_sceneModel->Draw(m_projectionMatrix, m_mainCamera->GetMatrix(), m_directionalLight, m_pointLight, m_spotLight, m_sceneModel->GetShadowShaderProgram());
	glCullFace(GL_BACK);

	//model pass - Draw the scene from the regular POV of the camera
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, m_windowWidth, m_windowHeight);
	m_sceneModel->Draw(m_projectionMatrix, m_mainCamera->GetMatrix(), m_directionalLight, m_pointLight, m_spotLight, m_sceneModel->GetShaderProgram());

	m_water->Draw(m_projectionMatrix, m_mainCamera->GetMatrix(), m_directionalLight->GetLightPos());

	m_EmberParticleEmitter->Draw(m_projectionMatrix, m_mainCamera->GetMatrix());
	
	DrawGizmos();
	
}

void CT6025_Assessment_Enviroment_Renderer::Destroy()
{
	//Clean up to stop memory leaks
	Gizmos::destroy();
	delete m_mainCamera;
	delete m_skyBox;
	delete m_spotLight;
	delete m_sceneModel;
	delete m_pointLight;
	delete m_reflection;
	delete m_directionalLight;
	delete m_water;

	glDeleteProgram(m_particleProgramID);
	glDeleteProgram(m_skyboxShader);
	glDeleteProgram(m_modelShader);
	glDeleteProgram(m_shadowShader);
	glDeleteProgram(m_waterShader);
	glDeleteProgram(m_particle_tfProgram);
}

void CT6025_Assessment_Enviroment_Renderer::SetupShaders()
{
	unsigned int vertexShader = 0, fragmentShader = 0, tfVertexShader = 0, tfGeometryShader = 0, geometryShader = 0, tessCntrlShader = 0, tessEvalShader = 0;

	//skybox
	vertexShader = Utility::loadShader("./resources/shaders/Skybox_vertex.glsl", GL_VERTEX_SHADER);
	fragmentShader = Utility::loadShader("./resources/shaders/Skybox_fragment.glsl", GL_FRAGMENT_SHADER);
	const char* skyBoxInputs[] = { "aPos" };
	const char* skyBoxOutputs[] = { "FragColor" };
	//bind the shaders to create our shader program
	m_skyboxShader = Utility::createProgram(vertexShader,0 ,0 , 0,fragmentShader, 1, skyBoxInputs, 1, skyBoxOutputs);
	m_skyBox->SetShader(m_skyboxShader);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//Model
	vertexShader = Utility::loadShader("./resources/shaders/vertex.glsl", GL_VERTEX_SHADER);
	fragmentShader = Utility::loadShader("./resources/shaders/fragment.glsl", GL_FRAGMENT_SHADER);
	//Define the input and output varialbes in the shaders
	//Note: these names are taken from the glsl files -- added in inputs for UV coordinates
	const char* szInputs[] = { "Position", "Colour", "Normal","Tex1" };
	const char* szOutputs[] = { "FragColor" };
	//bind the shaders to create our shader program
	m_modelShader = Utility::createProgram(vertexShader,0 ,0 ,0,fragmentShader, 4, szInputs, 1, szOutputs);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//Shadow
	vertexShader = Utility::loadShader("./resources/shaders/vertex_shadow.glsl", GL_VERTEX_SHADER);
	fragmentShader = Utility::loadShader("./resources/shaders/fragment_shadow.glsl", GL_FRAGMENT_SHADER);
	//Define the input and output varialbes in the shaders
	//Note: these names are taken from the glsl files -- added in inputs for UV coordinates
	const char* szInputs2[] = { "Position", "Colour", "Normal","Tex1" };
	const char* szOutputs2[] = { "depth" };
	//bind the shaders to create our shader program
	m_shadowShader = Utility::createProgram(vertexShader, 0, 0, 0, fragmentShader, 4, szInputs2, 1, szOutputs2);
	m_sceneModel->SetShader(m_modelShader, m_shadowShader);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//Water
	//Load the shaders for this program
	vertexShader = Utility::loadShader("./resources/shaders/Water_vertex.glsl", GL_VERTEX_SHADER);
	tessCntrlShader = Utility::loadShader("./resources/shaders/Water_tessControl.glsl", GL_TESS_CONTROL_SHADER);
	tessEvalShader = Utility::loadShader("./resources/shaders/Water_tessEval.glsl", GL_TESS_EVALUATION_SHADER);
	fragmentShader = Utility::loadShader("./resources/shaders/Water_fragment.glsl", GL_FRAGMENT_SHADER);
	//Define the input and output varialbes in the shaders
	//Note: these names are taken from the glsl files -- added in inputs for UV coordinates
	const char* szWaterInputs[] = { "Position", "Normal", "Tangent","UV" };
	const char* szWaterOutputs[] = { "FragColor" };
	//bind the shaders to create our shader program
	m_waterShader = Utility::createProgram(vertexShader, tessCntrlShader, tessEvalShader, 0, fragmentShader, 4, szWaterInputs, 1, szWaterOutputs);
	m_water->SetShader(m_waterShader);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteShader(tessCntrlShader);
	glDeleteShader(tessEvalShader);

	//Particles
	tfVertexShader = Utility::loadShader("./resources/shaders/particle_tf_vertex.glsl", GL_VERTEX_SHADER);
	tfGeometryShader = Utility::loadShader("./resources/shaders/particle_tf_geom.glsl", GL_GEOMETRY_SHADER);
	const char* tfInputs[] = { "particleType", "particlePos", "particleVelocity", "particleAge" };
	const char* tfOutputs[] = { "Type", "Position", "Velocity", "Age" };
	m_particle_tfProgram = Utility::createTransformFeedbackProgram(tfVertexShader, 0, 0, tfGeometryShader, 4, tfInputs, 4, tfOutputs);

	vertexShader = Utility::loadShader("./resources/shaders/particle_vertex.glsl", GL_VERTEX_SHADER);
	fragmentShader = Utility::loadShader("./resources/shaders/particle_fragment.glsl", GL_FRAGMENT_SHADER);
	geometryShader = Utility::loadShader("./resources/shaders/particle_geometry.glsl", GL_GEOMETRY_SHADER);
	const char* particle_szInputs[] = { "particleType", "particlePos", "particleVelocity", "particleAge" };
	const char* particle_szOutputs[] = { "FragColor" };
	//bind the shaders to create our shader program
	m_particleProgramID = Utility::createProgram(vertexShader, 0, 0, geometryShader, fragmentShader, 4, particle_szInputs, 1, particle_szOutputs);
	m_EmberParticleEmitter->SetShader(m_particleProgramID, m_particle_tfProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteShader(tfVertexShader);
	glDeleteShader(tfGeometryShader);
	
	//Reflection
	//Load the shaders for this program
	vertexShader = Utility::loadShader("./resources/shaders/reflection_vertex.glsl", GL_VERTEX_SHADER);
	fragmentShader = Utility::loadShader("./resources/shaders/reflection_fragment.glsl", GL_FRAGMENT_SHADER);
	const char* szReflectionInputs[] = { "Position", "Colour", "Normal","Tex1" };
	const char* szReflectionOutputs[] = { "FragColor" };
	//bind the shaders to create our shader program
	m_reflectionFBO_programID = Utility::createProgram(vertexShader, 0, 0, 0, fragmentShader, 4, szReflectionInputs, 1, szReflectionOutputs);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	vertexShader = Utility::loadShader("./resources/shaders/reflectionFS_vertex.glsl", GL_VERTEX_SHADER);
	fragmentShader = Utility::loadShader("./resources/shaders/reflectionFS_fragment.glsl", GL_FRAGMENT_SHADER);
	//Define the input and output varialbes in the shaders
	//Note: these names are taken from the glsl files -- added in inputs for UV coordinates
	const char* szReflectionInputs2[] = { "Position", "Tex1" };
	const char* szReflectionOutputs2[] = { "FragColor" };
	//bind the shaders to create our shader program
	m_reflection_programID = Utility::createProgram(vertexShader, 0, 0, 0, fragmentShader, 2, szReflectionInputs2, 1, szReflectionOutputs2);
	m_reflection->SetShader(m_reflectionFBO_programID, m_reflection_programID);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void CT6025_Assessment_Enviroment_Renderer::GUI_Lighting()
{
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 window_size = ImVec2(400.f, 250.f);
	ImVec2 windowPos = ImVec2(io.DisplaySize.x * 0.01f, io.DisplaySize.y * 0.05f);
	ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);
	ImGui::Begin("Light Properties");
	{
		//directional Scene Light
		ImGui::ColorEdit3("Light Ambient Colour", glm::value_ptr(m_directionalLight->m_lightAmbient));
		ImGui::ColorEdit3("Light Diffuse Colour", glm::value_ptr(m_directionalLight->m_lightDiffuse));
		ImGui::ColorEdit3("Light Specular Colour", glm::value_ptr(m_directionalLight->m_lightSpecular));
		ImGui::DragFloat("Specular Power", &m_directionalLight->m_lightSpecularPower, 1.f, 1.f, 128.f, "%.0f", 1.f);

		//Point Light
		ImGui::ColorEdit3("PointLight Ambient Colour", glm::value_ptr(m_pointLight->m_lightAmbient));
		ImGui::ColorEdit3("PointLight Diffuse Colour", glm::value_ptr(m_pointLight->m_lightDiffuse));
		ImGui::ColorEdit3("PointLight Specular Colour", glm::value_ptr(m_pointLight->m_lightSpecular));
		ImGui::DragFloat("PointLight Specular Power", &m_pointLight->m_lightSpecularPower, 1.f, 1.f, 128.f, "%.0f", 1.f);
		ImGui::DragFloat("PointLight Constant", &m_pointLight->m_constant, 1.f, 1.f, 60.f, "%.0f", 1.f);
		ImGui::DragFloat("PointLight Linear", &m_pointLight->m_linear, 1.f, 1.f, 60.f, "%.0f", 1.f);
		ImGui::DragFloat("PointLight Quadratic", &m_pointLight->m_quadratic, 1.f, 1.f, 60.f, "%.0f", 1.f);

		//Spotlight
		ImGui::ColorEdit3("Spotlight Ambient Colour", glm::value_ptr(m_spotLight->m_lightAmbient));
		ImGui::ColorEdit3("Spotlight Diffuse Colour", glm::value_ptr(m_spotLight->m_lightDiffuse));
		ImGui::ColorEdit3("Spotlight Specular Colour", glm::value_ptr(m_spotLight->m_lightSpecular));
		ImGui::DragFloat("Spotlight Specular Power", &m_spotLight->m_lightSpecularPower, 1.f, 1.f, 128.f, "%.0f", 1.f);
		ImGui::DragFloat("Spotlight angle", &m_spotLight->m_angle, 1.f, 1.f, 360.f, "%.0f", 1.f);
		ImGui::DragFloat("Spotlight Constant", &m_spotLight->m_constant, 1.f, 1.f, 5.f, "%.0f", 1.f);
		ImGui::DragFloat("Spotlight Linear", &m_spotLight->m_linear, 1.f, 1.f, 5.f, "%.0f", 1.f);
		ImGui::DragFloat("Spotlight Quadratic", &m_spotLight->m_quadratic, 1.f, 1.f, 5.f, "%.0f", 1.f);
	}
	ImGui::End();
}

void CT6025_Assessment_Enviroment_Renderer::GUI_Tesselation()
{
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 window_size = ImVec2(550.f, 250.f);
	ImVec2 windowPos = ImVec2((io.DisplaySize.x * 0.01f) + (m_windowWidth-700), io.DisplaySize.y * 0.05f);
	ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);
	ImGui::Begin("Rendering Variables");
	ImGui::Text("Particle Count: %u", m_EmberParticleEmitter->GetParticleCount());
	ImGui::Checkbox("RenderWireFrame", &m_bRenderWireFrame);
	ImGui::Text("Shader Control Uniforms");
	ImGui::SliderInt("Inner Tessellation Value", m_water->GetInnerTessEdgeValue(), 0, 5);
	ImGui::SliderInt("Outer Tessellation Value", m_water->GetOuterTessEdgeValue(), 0, 5);
	ImGui::SliderFloat("Displacement Scale", m_water->GetDisplacementValue(), 0.f, 5.f);
	//Draw Frame rate
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
}

void CT6025_Assessment_Enviroment_Renderer::UpdateGizmos()
{
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
}

void CT6025_Assessment_Enviroment_Renderer::DrawGizmos()
{
	//get the view matrix from the world - space camera matrix
	glm::mat4 viewMatrix = glm::inverse(m_mainCamera->GetMatrix());
	//draw the gizmos from this frame
	Gizmos::draw(viewMatrix, m_projectionMatrix);
}




//broken old deffered rendering calls
void CT6025_Assessment_Enviroment_Renderer::DefferedRendering()
{
	//unused deffered rendering because I couldn't get to work
	/*
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	//set up the FBO diffuse render texture bind to colour attachment 0
	glGenTextures(1, &m_fbo_diffuse_texture);
	glBindTexture(GL_TEXTURE_2D, m_fbo_diffuse_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_windowWidth, m_windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_fbo_diffuse_texture, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	//set up the FBO normals render texture bind to colour attachment 1
	glGenTextures(1, &m_fbo_normal_texture);
	glBindTexture(GL_TEXTURE_2D, m_fbo_normal_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_windowWidth, m_windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, m_fbo_normal_texture, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	//set up the FBO world space render texture bind to colour attachment 2
	glGenTextures(1, &m_fbo_worldspace_texture);
	glBindTexture(GL_TEXTURE_2D, m_fbo_worldspace_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_windowWidth, m_windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, m_fbo_worldspace_texture, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	//attach depth texture
	glGenTextures(1, &m_fbo_depth_texture);
	glBindTexture(GL_TEXTURE_2D, m_fbo_depth_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, m_windowWidth, m_windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_fbo_depth_texture, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, drawBuffers);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		if (log != nullptr)
		{
			log->addLog(LOG_LEVEL::LOG_ERROR, "Framebuffer status incomplete!");
		}
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

	//generate some random  postions
	for (int i = 0; i < 10; ++i)
	{
		m_randPositions[i] = glm::vec4((rand() % 40) - 20, (rand() % 5) - 3, (rand() % 40) - 20, 1.0f);
	}
	return true;
	*/
}

void CT6025_Assessment_Enviroment_Renderer::DrawDefferedRendering()
{
	/*old deffered rendering draw calls - didn't work so unused
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	// clear the backbuffer
	glClearColor(0.45, 0.45, 0.45, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse(m_cameraMatrix);

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
	//lets draw a few random dragons in the scene

	for (int j = 0; j < 10; ++j)
	{

		// for each mesh in the model...
		for (unsigned int i = 0; i < m_fbxModel->getMeshCount(); ++i)
		{
			// get the current mesh
			FBXMeshNode* pMesh = m_fbxModel->getMeshByIndex(i);

			glm::mat4 m4Model = pMesh->m_globalTransform;
			m4Model[3] = m_randPositions[j];
			unsigned int modelUniform = glGetUniformLocation(m_fbo_program, "Model");
			glUniformMatrix4fv(modelUniform, 1, false, glm::value_ptr(m4Model));
			//send the normal matrix
			//this is only beneficial if a model has a non-uniform scale or non-orthoganal model matrix
			glm::mat4 m4Normal = glm::transpose(glm::inverse(m4Model));// *m_modelMatrix;
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
	}
	glBindVertexArray(0);
	glUseProgram(0);

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
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_windowWidth,
		m_windowHeight, GL_DEPTH_COMPONENT, GL_FLOAT, pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	delete[] pixels;

	//bind the framebuffer for rendering
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.25, 0.25, 0.25, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Clear out everything on the back buffer, this is now our render target


	glUseProgram(m_programID);

	//pass our camera position to our fragment shader uniform
	glm::vec3 cm = m_cameraMatrix[3];
	cameraPosUniform = glGetUniformLocation(m_programID, "camera_pos");
	glUniform3fv(cameraPosUniform, 1, glm::value_ptr(cm));

	glm::vec3 ld = lightDir.xyz;
	lightDirUniform = glGetUniformLocation(m_programID, "lightDir");
	glUniform3fv(lightDirUniform, 1, glm::value_ptr(ld));

	glm::vec2 screenDim = glm::vec2(m_windowWidth, m_windowHeight);
	unsigned int screenSizeUniform = glGetUniformLocation(m_programID, "screenSize");
	glUniform2fv(screenSizeUniform, 1, glm::value_ptr(screenDim));


	glBindVertexArray(m_vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo[1]);
	unsigned int texUniformID = glGetUniformLocation(m_programID, "DiffuseTexture");
	glUniform1i(texUniformID, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_fbo_diffuse_texture);
	texUniformID = glGetUniformLocation(m_programID, "NormalTexture");
	glUniform1i(texUniformID, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_fbo_normal_texture);
	texUniformID = glGetUniformLocation(m_programID, "WorldTexture");
	glUniform1i(texUniformID, 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_fbo_worldspace_texture);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glUseProgram(0);
	*/
}