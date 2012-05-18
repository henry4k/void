#ifndef MODEL_HPP
#define MODEL_HPP

#include "Common.hpp"
#include "Math.hpp"


struct Vertex
{
	Vec3f position;
	Vec2f texCoord;
	Vec3f normal;
	Vec4f tangent;
};

void EnableVertexArrays();
void BindVertexAttributes( Handle shader );
void SetVertexAttributePointers();


struct Mesh
{
	Vertex* vertices;
	int vertexCount;
	
	unsigned short* indices;
	int indexCount;
};

bool LoadMesh( Mesh* mesh, const char* file );
void FreeMesh( const Mesh* mesh );


struct Model
{
	Handle vertexBuffer;
	Handle indexBuffer;
	int primitiveType;
	int size;
};

bool CreateModel( Model* model, const Mesh* mesh );
bool LoadModel( Model* model, const char* mesh );
void FreeModel( const Model* model );
void DrawModel( const Model* model );

#endif