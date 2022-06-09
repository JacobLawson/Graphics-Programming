#ifndef __TEXTURE_H_
#define __TEXTURE_H_

#include <string>
#include <vector>

class Texture
{
public:
	Texture();
	~Texture();

	//Function to load a texture from file
	bool Load(std::string a_filename);
	void unload();

	//Get Cubemap
	unsigned int LoadCubeMap(std::vector<std::string> a_textureFaces, unsigned int* cubeMapFace_ID);

	//Get file name
	const std::string& GetFileName() const { return m_filename; }
	unsigned int GetTextureID() const { return m_textureID; }
	void GetDimensions(unsigned int& a_w, unsigned int& a_h) const;

protected:
	std::string m_filename;
	unsigned int m_width;
	unsigned int m_height;
	unsigned int m_textureID;
};

inline void Texture::GetDimensions(unsigned int& a_w, unsigned int& a_h) const
{
	a_w = m_width; a_h = m_height;
};

#endif //__TEXTURE_H_