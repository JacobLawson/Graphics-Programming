#ifndef __SIMPLE_VERTEX_H_
#define __SIMPLE_VERTEX_H_

#include <glm/glm.hpp>

struct SVertex
{
	glm::vec4 pos;
	glm::vec4 colour;
};


class SimpleVertex
{
public:
	enum VertexAttributeFlags
	{
		POSITION = (1 << 0), //The Position of the Vertex
		NORMAL = (1 << 1), //The Normal for the Vertex
		TANGENT = (1 << 2),
		TEXCOORD = (1 << 3), //The UV Coordinates for the mesh
	};

	enum Offsets
	{
		PositionOffset = 0,
		NormalOffset = PositionOffset + sizeof(glm::vec4),
		TangentOffset = NormalOffset + sizeof(glm::vec4),
		TexCoordOffset = TangentOffset + sizeof(glm::vec4),
	};

	SimpleVertex();
	~SimpleVertex();

	glm::vec4 position;
	glm::vec4 normal;
	glm::vec4 tangent;
	glm::vec2 texCoord;
};

#endif // !__SIMPLE_VERTEX_H_

