#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>

#include <tools4k/Math.h>
#include <tools4k/Vector.h>
#include <kj/Common.h>

using tools4k::vec2f;
using tools4k::vec3f;
using tools4k::vec4f;

class Shader;

struct Vertex
{
	vec3f position;
	vec2f texCoord;
	vec3f normal;
	vec4f tangent;
};

void EnableVertexArrays();
void BindVertexAttributes( const Shader* shader );
void SetVertexAttributePointers();


class Mesh
{
	public:
		Mesh();
		void clear();
		
		bool load( const char* file );
		
		int primitiveType;
		std::vector<Vertex> vertices;
		std::vector<unsigned short> indices;
};


class Model
{
	public:
		Model();
		~Model();
		
		bool create( const Mesh* mesh );
		bool load( const char* file );
		
		void draw() const;
		
		Handle vertexBuffer() const { return m_VertexBuffer; }
		Handle indexBuffer() const { return m_IndexBuffer; }
		int primitiveType() const { return m_PrimitiveType; }
		int size() const { return m_Size; }
		
	protected:
		void clear();
		
		Handle m_VertexBuffer;
		Handle m_IndexBuffer;
		int m_PrimitiveType;
		int m_Size;
};

#endif