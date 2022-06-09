#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Error.h"

#include "Utilities.h"
#include "Application_Log.h"
#include <stb_image.h>

static double s_prevTime = 0;
static float s_totalTime = 0;
static float s_deltaTime = 0;

void Utility::resetTimer()
{
	s_prevTime = glfwGetTime();
	s_totalTime = 0;
	s_deltaTime = 0;
}

float Utility::tickTimer()
{
	double currentTime = glfwGetTime();
	s_deltaTime = (float)(currentTime - s_prevTime);
	s_totalTime += s_deltaTime;
	s_prevTime = currentTime;
	return s_deltaTime;
}

float Utility::getDeltaTime()
{
	return s_deltaTime;
}

float Utility::getTotalTime()
{
	return s_totalTime;
}

bool Utility::LoadImageFromFile(std::string a_filePath, unsigned int& a_textureID)
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
			log->addLog(LOG_LEVEL::LOG_ERROR, "Successfully loaded texture : %s", a_filePath.c_str());
		}
		return true;
	}
	else
	{
		Application_Log* log = Application_Log::Get();
		if (log != nullptr)
		{
			log->addLog(LOG_LEVEL::LOG_ERROR, "Failed to load texture : %s", a_filePath.c_str());
		}
	}
	return false;
}

// loads a shader from a file and creates it for the specified stage
unsigned int Utility::loadShader(const char* a_filename, unsigned int a_type)
{
	int success = GL_FALSE;

	unsigned char* source = fileToBuffer(a_filename);
	unsigned int handle = glCreateShader(a_type);

	glShaderSource(handle, 1, (const char**)&source, 0);
	glCompileShader(handle);
	//the buffer returned from fileToBuffer needs to be deleted as it was allocated.
	delete[] source;

	glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;		
		glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetShaderInfoLog(handle, infoLogLength, 0, infoLog);

		Application_Log* log = Application_Log::Create();
		if (log != nullptr)
		{

			log->addLog(LOG_LEVEL::LOG_ERROR, "Error: Failed to compile shader!\n : %s", infoLog);
		}
		
		delete[] infoLog;
		return 0;
	}

	return handle;
}

// creates a shader program, links the specified shader stages to it, and binds the specified input/output attributes
unsigned int Utility::createProgram(unsigned int a_vertexShader, unsigned int a_controlShader, unsigned int a_evaluationShader, unsigned int a_geometryShader, unsigned int a_fragmentShader,
								  unsigned int a_inputAttributeCount /* = 0 */, const char** a_inputAttributes /* = nullptr */,
								  unsigned int a_outputAttributeCount /* = 0 */, const char** a_outputAttributes /* = nullptr */)
{
	int success = GL_FALSE;

	// create a shader program and attach the shaders to it
	unsigned int handle = glCreateProgram();
	if ( a_vertexShader != 0 )		glAttachShader(handle, a_vertexShader);
	if (a_controlShader != 0)		glAttachShader(handle, a_controlShader);
	if (a_evaluationShader != 0)	glAttachShader(handle, a_evaluationShader);
	if (a_geometryShader != 0)		glAttachShader(handle, a_geometryShader);
	if (a_fragmentShader != 0)		glAttachShader(handle, a_fragmentShader);

	// specify vertex input attributes
	for ( unsigned int i = 0 ; i < a_inputAttributeCount ; ++i )
		glBindAttribLocation(handle, i, a_inputAttributes[i]);

	// specify fragment shader outputs
	for ( unsigned int i = 0 ; i < a_outputAttributeCount ; ++i )
		glBindFragDataLocation(handle, i, a_outputAttributes[i]);

	// link the program together and log errors
	glLinkProgram(handle);

	glGetProgramiv(handle, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;		
		glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetProgramInfoLog(handle, infoLogLength, 0, infoLog);
		Application_Log* log = Application_Log::Create();
		if (log != nullptr)
		{

			log->addLog(LOG_LEVEL::LOG_ERROR, "Error: Failed to link shader program!\n : %s", infoLog);
		}
		
		delete[] infoLog;
		return 0;
	}

	return handle;
}

unsigned int Utility::createTransformFeedbackProgram(unsigned int a_vertexShader, unsigned int a_controlShader, unsigned int a_evaluationShader, unsigned int a_geometryShader,
	unsigned int a_inputAttributeCount /*= 0*/, const char** a_inputAttributes /*= nullptr*/,	unsigned int a_outputAttributeCount /*= 0*/, const char** a_outputAttributes /*= nullptr*/)
{
	int success = GL_FALSE;

	// create a shader program and attach the shaders to it
	unsigned int handle = glCreateProgram();
	if (a_vertexShader != 0)		glAttachShader(handle, a_vertexShader);
	if (a_controlShader != 0)		glAttachShader(handle, a_controlShader);
	if (a_evaluationShader != 0)	glAttachShader(handle, a_evaluationShader);
	if (a_geometryShader != 0)		glAttachShader(handle, a_geometryShader);
	
	// specify vertex input attributes
	for (unsigned int i = 0; i < a_inputAttributeCount; ++i)
		glBindAttribLocation(handle, i, a_inputAttributes[i]);

	// specify transform feedback outputs
	glTransformFeedbackVaryings(handle, a_outputAttributeCount, a_outputAttributes, GL_INTERLEAVED_ATTRIBS);

	// link the program together and log errors
	glLinkProgram(handle);

	glGetProgramiv(handle, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetProgramInfoLog(handle, infoLogLength, 0, infoLog);
		Application_Log* log = Application_Log::Create();
		if (log != nullptr)
		{

			log->addLog(LOG_LEVEL::LOG_ERROR, "Error: Failed to link transform feedback shader program!\n : %s", infoLog);
		}
		
		delete[] infoLog;
		return 0;
	}

	return handle;
}

unsigned char* Utility::fileToBuffer(const char* a_sPath)
{
	// open file for text reading
	FILE* pFile = fopen(a_sPath,"rb");
	if (pFile == nullptr)
	{
		Application_Log* log = Application_Log::Create();
		if (log != nullptr)
		{

			log->addLog(LOG_LEVEL::LOG_ERROR, "Error: Unable to open file '%s' for reading : %s", a_sPath);
		}
		//printf("Error: Unable to open file '%s' for reading!\n",a_sPath);
		return nullptr;
	}

	// get number of bytes in file
	fseek(pFile, 0, SEEK_END);
	unsigned int uiLength = (unsigned int)ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	// allocate buffer and read file contents
	unsigned char* acBuffer = new unsigned char[uiLength + 1];
	memset(acBuffer,0,uiLength + 1);
	fread(acBuffer, sizeof(unsigned char), uiLength, pFile);

	fclose(pFile);
	return acBuffer;
}

void Utility::freeMovement(glm::mat4& a_transform, float a_deltaTime, float a_speed, const glm::vec3& a_up /* = glm::vec3(0,1,0) */)
{
	GLFWwindow* window = glfwGetCurrentContext();

	// Get the camera's forward, right, up, and location vectors
	glm::vec4 vForward = a_transform[2];
	glm::vec4 vRight = a_transform[0];
	glm::vec4 vUp = a_transform[1];
	glm::vec4 vTranslation = a_transform[3];

	float frameSpeed = glfwGetKey(window,GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? a_deltaTime * a_speed * 2 : a_deltaTime * a_speed;	

	// Translate camera
	if (glfwGetKey(window,'W') == GLFW_PRESS)
	{
		vTranslation -= vForward * frameSpeed;
	}
	if (glfwGetKey(window,'S') == GLFW_PRESS)
	{
		vTranslation += vForward * frameSpeed;
	}
	if (glfwGetKey(window,'D') == GLFW_PRESS)
	{
		vTranslation += vRight * frameSpeed;
	}
	if (glfwGetKey(window,'A') == GLFW_PRESS)
	{
		vTranslation -= vRight * frameSpeed;
	}
	if (glfwGetKey(window,'Q') == GLFW_PRESS)
	{
		vTranslation += vUp * frameSpeed;
	}
	if (glfwGetKey(window,'E') == GLFW_PRESS)
	{
		vTranslation -= vUp * frameSpeed;
	}

	a_transform[3] = vTranslation;

	// check for camera rotation
	static bool sbMouseButtonDown = false;
	if (glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
	{
		static double siPrevMouseX = 0;
		static double siPrevMouseY = 0;

		if (sbMouseButtonDown == false)
		{
			sbMouseButtonDown = true;
			glfwGetCursorPos(window,&siPrevMouseX,&siPrevMouseY);
		}

		double mouseX = 0, mouseY = 0;
		glfwGetCursorPos(window,&mouseX,&mouseY);

		double iDeltaX = mouseX - siPrevMouseX;
		double iDeltaY = mouseY - siPrevMouseY;

		siPrevMouseX = mouseX;
		siPrevMouseY = mouseY;

		glm::mat4 mMat;
		
		// pitch
		if (iDeltaY != 0)
		{
			mMat = glm::axisAngleMatrix( vRight.xyz(), (float)-iDeltaY / 150.0f );
			vRight = mMat * vRight;
			vUp = mMat * vUp;
			vForward = mMat * vForward;
		}

		// yaw
		if (iDeltaX != 0)
		{
			mMat = glm::axisAngleMatrix( a_up, (float)-iDeltaX / 150.0f );
			vRight = mMat * vRight;
			vUp = mMat * vUp;
			vForward = mMat * vForward;
		}

		a_transform[0] = vRight;
		a_transform[1] = vUp;
		a_transform[2] = vForward;
	}
	else
	{
		sbMouseButtonDown = false;
	}
}
