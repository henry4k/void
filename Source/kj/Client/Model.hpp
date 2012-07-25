#ifndef MODEL_HPP
#define MODEL_HPP

#include "../Common.hpp"
#include <tools4k/Math.h>
#include <tools4k/Vector.h>

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
		~Mesh();
		
		bool load( const char* file );
		
		
		const Vertex* vertices() const { return m_Vertices; }
		int vertexCount() const { return m_VertexCount; }
		
		const unsigned short* indices() const { return m_Indices; }
		int indexCount() const { return m_IndexCount; }
		
		int pimitiveType() const { return m_PrimitiveType; }
		
	protected:
		void clear();
		
		Vertex* m_Vertices;
		int m_VertexCount;
		
		unsigned short* m_Indices;
		int m_IndexCount;
		
		int m_PrimitiveType;
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
		Handle m_VertexBuffer;
		Handle m_IndexBuffer;
		int m_PrimitiveType;
		int m_Size;
};

#endif