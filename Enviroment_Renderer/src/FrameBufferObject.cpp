#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#include "..\include\FrameBufferObject.h"

FrameBufferObject::FrameBufferObject() 
{
	m_textureAttachment = -1;
	CreateFrameBuffer();
}

FrameBufferObject::~FrameBufferObject()
{
	Cleanup();
}

bool FrameBufferObject::CreateFrameBuffer()
{
	glGenFramebuffers(1, &m_frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
	return true;
}

void FrameBufferObject::EndCreate()
{
	// Here we tell the framebuffer which color attachments we will be drawing to and how many
	if (m_textureAttachment != -1)
	{
		GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, drawBuffers);
	}
	else
	{
		glDrawBuffer(GL_NONE);
	}
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

bool FrameBufferObject::Cleanup()
{
	glDeleteFramebuffers(1, &m_frameBuffer);
	glDeleteTextures(1, &m_textureAttachment);
	glDeleteRenderbuffers(1, &m_depthBuffer);

	return true;
}

bool FrameBufferObject::CreateTextureAttachment(unsigned int a_width, unsigned int a_height)
{
	if (m_textureAttachment != -1)
	{
		// create a texture to be attached to the framebuffer, stored in the derived app class as a member
		glGenTextures(1, &m_textureAttachment);
		// bind the texture for editing
		glBindTexture(GL_TEXTURE_2D, m_textureAttachment);
		// create the texture. Note, the width and height are the dimensions of the screen and the final
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, a_width, a_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		// set the filtering if we intend to sample within a shader
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// attach it as a GL_COLOR_ATTACHMENT
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_textureAttachment, 0);
		return true;
	}
}

bool FrameBufferObject::CreateDepthTextureAttachment(unsigned int a_width, unsigned int a_height)
{
	// create a texture to be attached to the framebuffer, stored in the derived app class as a member
	glGenTextures(1, &m_depthTextureAttachment);
	// bind the texture for editing
	glBindTexture(GL_TEXTURE_2D, m_depthTextureAttachment);
	// create the texture. Note, the width and height are the dimensions of the screen and the final
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, a_width, a_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	// set the filtering if we intend to sample within a shader
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// attach it as a GL_COLOR_ATTACHMENT
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthTextureAttachment, 0);
	return true;
}

bool FrameBufferObject::CreateRenderBufferAttachement(unsigned int a_width, unsigned int a_height)
{
	glGenRenderbuffers(1, &m_renderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_renderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, a_width, a_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_renderBuffer);
	return true;
}

bool FrameBufferObject::CreateDepthBufferAttachment(unsigned int a_width, unsigned int a_height)
{
	glGenRenderbuffers(1, &m_depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, a_width, a_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer);
	return true;
}

bool FrameBufferObject::BindFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
	glEnable(GL_DEPTH_TEST);
	return true;
}

bool FrameBufferObject::UnBindFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	return false;
}
