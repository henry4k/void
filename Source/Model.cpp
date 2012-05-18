#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Model.hpp"


enum
{
	VERTEX_POSITION = 1, // begins with 1 because 0 is an invalid/reserved attribute name
	VERTEX_TEXCOORD,
	VERTEX_NORMAL,
	VERTEX_TANGENT,
	VERTEX_COLOR
};

void EnableVertexArrays()
{
	glEnableVertexAttribArray(VERTEX_POSITION);
	glEnableVertexAttribArray(VERTEX_TEXCOORD);
	glEnableVertexAttribArray(VERTEX_NORMAL);
	glEnableVertexAttribArray(VERTEX_TANGENT);
// 	glEnableVertexAttribArray(VERTEX_COLOR);
}

void BindVertexAttributes( Handle shader )
{
	glBindAttribLocation(shader, VERTEX_POSITION, "VertexPosition");
	glBindAttribLocation(shader, VERTEX_TEXCOORD, "VertexTexCoord");
	glBindAttribLocation(shader, VERTEX_NORMAL,   "VertexNormal");
	glBindAttribLocation(shader, VERTEX_TANGENT,  "VertexTangent");
// 	glBindAttribLocation(shader, VERTEX_COLOR,    "VertexColor");
}

void SetVertexAttributePointers()
{
	int offset = 0;
#define AttribPointer(Name,Count,TypeName,Type) glVertexAttribPointer( Name , Count , TypeName, GL_TRUE, sizeof(Vertex), (void*)offset); offset += sizeof( Type ) * Count;
	AttribPointer(VERTEX_POSITION,3,GL_FLOAT,float);
	AttribPointer(VERTEX_TEXCOORD,2,GL_FLOAT,float);
	AttribPointer(VERTEX_NORMAL,3,GL_FLOAT,float);
	AttribPointer(VERTEX_TANGENT,4,GL_FLOAT,float);
// 	AttribPointer(VERTEX_COLOR,4,GL_UNSIGNED_BYTE,unsigned char);
#undef AttribPointer
}



const int MaxLineLength = 512;

/*
enum PlyProperties
{
	PLY_X = 0,
	PLY_Y,
	PLY_Z,
	
	PLY_NX,
	PLY_NY,
	PLY_NZ,
	
	PLY_S,
	PLY_T,
	
	PLY_PROPERTY_COUNT
};
*/

struct MeshInfo
{
	FILE* file;
	char line[MaxLineLength];
	int get;
	int lineNumber;
	Mesh* mesh;
	
// 	int properties[PLY_PROPERTY_COUNT]; // property positions in file
// 	int curProperty;
// 	int propertiesRead;
};

bool ReadLine( MeshInfo* info )
{
	FILE* file = info->file;
	char* line = info->line;
	bool eof = false;
	int i = 0;
	for(; i < MaxLineLength-1; i++) // -1 wegen '\0' am ende
	{
		int c = fgetc(file);
		
		if(c == EOF)
		{
			eof = true;
			break;
		}
		
		if(c == '\n')
		{
			break;
		}
		
		line[i] = c;
	}
	
	line[i] = '\0';
	info->lineNumber++;
	info->get = 0;
	return !eof;
}

bool CompareLazy( const char* a, const char* b )
{
	for(;;)
	{
		if((*a == '\0') || (*b == '\0'))
			break;
		if(*a != *b)
			return false;
		a++;
		b++;
	}
	return true;
}

bool LineEqual( MeshInfo* info, const char* comparedString )
{
	ReadLine(info); // ignore eof etc.
	return CompareLazy(info->line, comparedString);
}

int ReadChar( MeshInfo* info )
{
	int c = info->line[info->get];
	if(c == '\0')
		return EOF;
	info->get++;
	return c;
}

void Rewind( MeshInfo* info, int chars )
{
	info->get -= chars;
	if(info->get < 0)
		info->get = 0;
}

bool ContainsChar( const char* haystack, char needle )
{
	for(; *haystack != '\0'; haystack++)
		if(needle == *haystack)
			return true;
	return false;
}

void EatChars( MeshInfo* info, const char* eat )
{
	for(;;)
	{
		int c = ReadChar(info);
		if((c == EOF) || !ContainsChar(eat, c))
			break;
	}
	Rewind(info, 1);
}

void EatWhitespace( MeshInfo* info )
{
	EatChars(info, " \t");
}

int ReadInt( MeshInfo* info )
{
	EatWhitespace(info);
	int r = atoi(&info->line[info->get]);
	EatChars(info, "0123456789+-."); // int must also eat '.'!
	return r;
}

double ReadFloat( MeshInfo* info )
{
	EatWhitespace(info);
	double r = atof(&info->line[info->get]);
	EatChars(info, "0123456789+-.");
	return r;
}

bool ContinuesWith( MeshInfo* info, const char* text ) // eats the given text if equal
{
	const char* curLine = &info->line[info->get];
	int rewind = 0;
	
	for(;;)
	{
		int c = ReadChar(info);
		
		rewind++;
		
		if(c != *text)
			break;
		
		text++;
		
		if((*text == '\0') || (c == '\0'))
		{
			rewind = -1;
			break;
		}
	}
	if(rewind > 0)
		Rewind(info, rewind);
	
	return rewind == -1;
}

/*
bool CheckProperty( MeshInfo* info, const char* name, int id )
{
	if(!ContinuesWith(info, name))
		return false;
	
	info->properties[id] = info->curProperty;
	info->propertiesRead++;
	
	return true;
}
*/

bool ReadHeader( MeshInfo* info )
{
	if(!LineEqual(info, "ply"))
	{
		Log("Incorrect magic bytes");
		return false;
	}
	
	if(!LineEqual(info, "format ascii 1.0"))
	{
		Log("Incorrect format");
		return false;
	}
	
	for(;;)
	{
		if(!ReadLine(info))
		{
			Log("Unexpected EOF");
			return false;
		}
		
		/*
		if(ContinuesWith(info, "element vertex "))
		{
			info->vertexCount = ReadInt(info);
			
			// read properties
			info->curProperty = -1;
			info->propertiesRead = 0;
			for(;;)
			{
				if(!ReadLine(info))
					return false;
				
				if(!ContinuesWith(info, "property "))
					break;
				info->curProperty++;
				
#define CHECK_PROPERTY(N,I) if(!CheckProperty(info, N , I )) return false;
				CHECK_PROPERTY("float x", PLY_X);
				CHECK_PROPERTY("float y", PLY_Y);
				CHECK_PROPERTY("float z", PLY_Z);
				CHECK_PROPERTY("float nx", PLY_NX);
				CHECK_PROPERTY("float ny", PLY_NY);
				CHECK_PROPERTY("float nz", PLY_NZ);
				CHECK_PROPERTY("float s", PLY_S);
				CHECK_PROPERTY("float t", PLY_T);
#undef CHECK_PROPERTY
			}
			
			if(info->propertiesRead != PLY_PROPERTY_COUNT)
			{
				Log("Some properties are missing");
				return false;
			}
		}
		*/
		
		if(ContinuesWith(info, "element vertex "))
		{
			info->mesh->vertexCount = ReadInt(info);
			continue;
		}
		
		if(ContinuesWith(info, "element face "))
		{
			info->mesh->indexCount = ReadInt(info)*3;
			continue;
		}
		
		if(ContinuesWith(info, "end_header"))
		{
			break;
		}
	}
	
	return true;
}

bool ReadVertices( MeshInfo* info )
{
	Mesh* mesh = info->mesh;
	
	mesh->vertices = new Vertex[mesh->vertexCount];
	for(int i = 0; i < mesh->vertexCount; i++)
	{
		Vertex* vertex = &mesh->vertices[i];
		
		ReadLine(info);
		
	#define READ_VERTEX(P) vertex-> P = ReadFloat(info);
		READ_VERTEX(position.x)
		READ_VERTEX(position.y)
		READ_VERTEX(position.z)
		READ_VERTEX(normal.x)
		READ_VERTEX(normal.y)
		READ_VERTEX(normal.z)
		READ_VERTEX(texCoord.x)
		READ_VERTEX(texCoord.y)
	#undef READ_VERTEX
	}
	
	mesh->indices = new unsigned short[mesh->indexCount];
	for(int i = 0; i < mesh->indexCount; i += 3)
	{
		ReadLine(info);
		
		if(ReadInt(info) != 3)
			Log("Invalid face (only triangles are supported)");
		
		mesh->indices[i] = ReadInt(info);
		mesh->indices[i+1] = ReadInt(info);
		mesh->indices[i+2] = ReadInt(info);
	}
	
	return true;
}

/*
generateNormalAndTangent(float3 v1, float3 v2, text2 st1, text2 st2)
	{
		float3 normal = v1.crossProduct(v2);
		
		float coef = 1/ (st1.u * st2.v - st2.u * st1.v);
		float3 tangent;

		tangent.x = coef * ((v1.x * st2.v)  + (v2.x * -st1.v));
		tangent.y = coef * ((v1.y * st2.v)  + (v2.y * -st1.v));
		tangent.z = coef * ((v1.z * st2.v)  + (v2.z * -st1.v));
		
		float3 binormal = normal.crossProduct(tangent);
	}
*/

struct Triangle
{
	unsigned short index[3];
};

void CalculateTangentArray( int vertexCount, Vertex* vertex, int triangleCount, const Triangle* triangle )
{
	Vec3f* tan1 = new Vec3f[vertexCount * 2];
	Vec3f* tan2 = tan1 + vertexCount;
	memset(tan1, 0, sizeof(Vec3f) * vertexCount * 2);
	
	for( int a = 0; a < triangleCount; a++ )
	{
		int i1 = triangle->index[0];
		int i2 = triangle->index[1];
		int i3 = triangle->index[2];
		
		const Vec3f& v1 = vertex[i1].position;
		const Vec3f& v2 = vertex[i2].position;
		const Vec3f& v3 = vertex[i3].position;
		
		const Vec2f& w1 = vertex[i1].texCoord;
		const Vec2f& w2 = vertex[i2].texCoord;
		const Vec2f& w3 = vertex[i3].texCoord;
		
		float x1 = v2.x - v1.x;
		float x2 = v3.x - v1.x;
		float y1 = v2.y - v1.y;
		float y2 = v3.y - v1.y;
		float z1 = v2.z - v1.z;
		float z2 = v3.z - v1.z;
		
		float s1 = w2.x - w1.x;
		float s2 = w3.x - w1.x;
		float t1 = w2.y - w1.y;
		float t2 = w3.y - w1.y;
		
		float r = 1.0F / (s1 * t2 - s2 * t1);
		Vec3f sdir;
		sdir.x = (t2 * x1 - t1 * x2) * r;
		sdir.y = (t2 * y1 - t1 * y2) * r;
		sdir.z = (t2 * z1 - t1 * z2) * r;
		
		Vec3f tdir;
		tdir.x = (s1 * x2 - s2 * x1) * r;
		tdir.y = (s1 * y2 - s2 * y1) * r;
		tdir.z = (s1 * z2 - s2 * z1) * r;
		
		tan1[i1] = Add(tan1[i1], sdir);
		tan1[i2] = Add(tan1[i2], sdir);
		tan1[i3] = Add(tan1[i3], sdir);
		
		tan2[i1] = Add(tan2[i1], tdir);
		tan2[i2] = Add(tan2[i2], tdir);
		tan2[i3] = Add(tan2[i3], tdir);
		
		triangle++;
	}
	
	for( int a = 0; a < vertexCount; a++ )
	{
		const Vec3f& n = vertex[a].normal;
		const Vec3f& t = tan1[a];
		
		// Gram-Schmidt orthogonalize
		Vec3f* tangent3 = (Vec3f*)&vertex[a].tangent;
		*tangent3 = Normalized(Sub(t,Mul(n,Dot(n,t))));
		
		// Calculate handedness
		vertex[a].tangent.w = (Dot(Cross(n, t), tan2[a]) < 0.0F) ? -1.0F : 1.0F;
	}
	
	delete[] tan1;
}

void FreeMeshInfo( const MeshInfo* info )
{
	if(info->file)
		fclose(info->file);
}

bool LoadMesh( Mesh* mesh, const char* file )
{
	FILE* f = fopen(file, "r");
	if(!f)
	{
		Log("Can't read mesh '%s'", file);
		return false;
	}
	
	mesh->vertices = 0;
	mesh->vertexCount = 0;
	mesh->indices = 0;
	mesh->indexCount = 0;
	
	MeshInfo info;
	info.file = f;
	info.lineNumber = 0;
	info.mesh = mesh;
	
	if(!ReadHeader(&info))
	{
		Log("Failed while parsing line %d", info.lineNumber);
		FreeMeshInfo(&info);
		FreeMesh(mesh);
		return false;
	}
	
	if(!ReadVertices(&info))
	{
		Log("Failed while parsing line %d", info.lineNumber);
		FreeMeshInfo(&info);
		FreeMesh(mesh);
		return false;
	}
	
	CalculateTangentArray(mesh->vertexCount, mesh->vertices, mesh->indexCount/3, (Triangle*)mesh->indices);
	
	FreeMeshInfo(&info);
	return true;
}

void FreeMesh( const Mesh* mesh )
{
	if(mesh->vertices)
		delete[] mesh->vertices;
	
	if(mesh->indices)
		delete[] mesh->indices;
}




bool CreateModel( Model* model, const Mesh* mesh )
{
	glGenBuffers(1, &model->vertexBuffer);
	glGenBuffers(1, &model->indexBuffer);
	
	glBindBuffer(GL_ARRAY_BUFFER, model->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, mesh->vertexCount*sizeof(Vertex), mesh->vertices, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indexCount*sizeof(unsigned short), mesh->indices, GL_STATIC_DRAW);
	
	model->size = mesh->indexCount;
	model->primitiveType = GL_TRIANGLES;
	
	CheckGl();
	return true;
}

bool LoadModel( Model* model, const char* mesh )
{
	Mesh m;
	if(!LoadMesh(&m, mesh))
		return false;
	
	bool r = CreateModel(model, &m);
	
	FreeMesh(&m);
	return r;
}

void FreeModel( const Model* model )
{
	glDeleteBuffers(1, &model->vertexBuffer);
	glDeleteBuffers(1, &model->indexBuffer);
}

void DrawModel( const Model* model )
{
	glBindBuffer(GL_ARRAY_BUFFER, model->vertexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->indexBuffer);
	
	SetVertexAttributePointers();
	glDrawElements(model->primitiveType, model->size, GL_UNSIGNED_SHORT, 0);
}