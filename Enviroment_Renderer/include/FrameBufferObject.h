#ifndef __FRAME_BUFFER_OBJECT_H_
#define __FRAME_BUFFER_OBJECT_H_

class FrameBufferObject
{
public:
	FrameBufferObject();
	~FrameBufferObject();

	//openGL functions
	bool CreateFrameBuffer();
	void EndCreate();
	bool Cleanup();
	bool CreateTextureAttachment(unsigned int a_width, unsigned int a_height);
	bool CreateDepthTextureAttachment(unsigned int a_width, unsigned int a_height);

	bool CreateRenderBufferAttachement(unsigned int a_width, unsigned int a_height);
	bool CreateDepthBufferAttachment(unsigned int a_width, unsigned int a_height);

	bool BindFrameBuffer();
	bool UnBindFrameBuffer();

	unsigned int GetTexture() { return m_textureAttachment; }
	unsigned int GetDepthTexture() { return m_depthTextureAttachment; }

protected:
	unsigned int m_frameBuffer;
	unsigned int m_depthBuffer;
	unsigned int m_renderBuffer;

	unsigned int m_textureAttachment;
	unsigned int m_depthTextureAttachment;
};

#endif //__FRAME_BUFFER_OBJECT_H_