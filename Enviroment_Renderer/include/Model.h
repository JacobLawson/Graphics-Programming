#ifndef __Model_H_
#define __Model_H_

#include <FBXFile.h>


class Model
{
public:
	Model();
	~Model();

	void LoadModel(FBXFile* a_model);
	void Update(float a_deltaTime);
	void Draw(unsigned int a_programID);

	FBXFile* UseModel() { return m_model; }

protected:
	FBXFile* m_model;

	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_ibo;
};


#endif //__Model_H_