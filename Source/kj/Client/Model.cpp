#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <kj/Client/OpenGL.h>
#include <kj/Client/Model.h>
#include <kj/Client/Shader.h>




/// ---- Vertex ----

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

void BindVertexAttributes( const Shader* shader )
{
	Handle name = shader->name();
	glBindAttribLocation(name, VERTEX_POSITION, "VertexPosition");
	glBindAttribLocation(name, VERTEX_TEXCOORD, "VertexTexCoord");
	glBindAttribLocation(name, VERTEX_NORMAL,   "VertexNormal");
	glBindAttribLocation(name, VERTEX_TANGENT,  "VertexTangent");
// 	glBindAttribLocation(name, VERTEX_COLOR,    "VertexColor");
}

void SetVertexAttributePointers()
{
	long offset = 0;
#define AttribPointer(Name,Count,TypeName,Type) glVertexAttribPointer( Name , Count , TypeName, GL_TRUE, sizeof(Vertex), (void*)offset); offset += sizeof( Type ) * Count;
	AttribPointer(VERTEX_POSITION,3,GL_FLOAT,float);
	AttribPointer(VERTEX_TEXCOORD,2,GL_FLOAT,float);
	AttribPointer(VERTEX_NORMAL,3,GL_FLOAT,float);
	AttribPointer(VERTEX_TANGENT,4,GL_FLOAT,float);
// 	AttribPointer(VERTEX_COLOR,4,GL_UNSIGNED_BYTE,unsigned char);
#undef AttribPointer
}





/// ---- Mesh ----


const int MaxLineLength = 512;

struct MeshInfo
{
	FILE* file;
	char line[MaxLineLength];
	int get;
	int lineNumber;
	Mesh* mesh;
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
		
		if(ContinuesWith(info, "element vertex "))
		{
			info->mesh->vertices.resize( ReadInt(info) );
			continue;
		}
		
		if(ContinuesWith(info, "element face "))
		{
			info->mesh->indices.resize( ReadInt(info)*3 );
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
	
	for(int i = 0; i < mesh->vertices.size(); i++)
	{
		Vertex& vertex = mesh->vertices[i];
		
		ReadLine(info);
		
	#define READ_VERTEX(P) vertex. P = ReadFloat(info);
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
	
	for(int i = 0; i < mesh->indices.size(); i += 3)
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
	vec3f* tan1 = new vec3f[vertexCount*2];
	vec3f* tan2 = tan1 + vertexCount;
	memset(tan1, 0, sizeof(vec3f) * vertexCount * 2);
	
	for( int a = 0; a < triangleCount; a++ )
	{
		int i1 = triangle->index[0];
		int i2 = triangle->index[1];
		int i3 = triangle->index[2];
		
		const vec3f& v1 = vertex[i1].position;
		const vec3f& v2 = vertex[i2].position;
		const vec3f& v3 = vertex[i3].position;
		
		const vec2f& w1 = vertex[i1].texCoord;
		const vec2f& w2 = vertex[i2].texCoord;
		const vec2f& w3 = vertex[i3].texCoord;
		
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
		vec3f sdir;
		sdir.x = (t2 * x1 - t1 * x2) * r;
		sdir.y = (t2 * y1 - t1 * y2) * r;
		sdir.z = (t2 * z1 - t1 * z2) * r;
		
		vec3f tdir;
		tdir.x = (s1 * x2 - s2 * x1) * r;
		tdir.y = (s1 * y2 - s2 * y1) * r;
		tdir.z = (s1 * z2 - s2 * z1) * r;
		
		tan1[i1] = tan1[i1] + sdir;
		tan1[i2] = tan1[i2] + sdir;
		tan1[i3] = tan1[i3] + sdir;
		
		tan2[i1] = tan2[i1] + tdir;
		tan2[i2] = tan2[i2] + tdir;
		tan2[i3] = tan2[i3] + tdir;
		
		triangle++;
	}
	
	for( int a = 0; a < vertexCount; a++ )
	{
		const vec3f& n = vertex[a].normal;
		const vec3f& t = tan1[a];
		
		// Gram-Schmidt orthogonalize
		vec3f* tangent3 = (vec3f*)&vertex[a].tangent;
		*tangent3 = (t - n*n.dot(t)).normalize();
		
		// Calculate handedness
		vertex[a].tangent.w = (n.cross(t).dot(tan2[a]) < 0.0F) ? -1.0F : 1.0F;
	}
	
	delete[] tan1;
}

void FreeMeshInfo( const MeshInfo* info )
{
	if(info->file)
		fclose(info->file);
}






Mesh::Mesh() :
	primitiveType(0)
{
}

void Mesh::clear()
{
	primitiveType = 0;
	vertices.clear();
	indices.clear();
}

bool Mesh::load( const char* file )
{
	clear();
	
	FILE* f = fopen(file, "r");
	if(!f)
	{
		Log("Can't read mesh '%s'", file);
		return false;
	}
	
	primitiveType = GL_TRIANGLES;
	
	MeshInfo info;
	info.file = f;
	info.lineNumber = 0;
	info.mesh = this;
	
	if(!ReadHeader(&info))
	{
		Log("Failed while parsing line %d", info.lineNumber);
		FreeMeshInfo(&info);
		clear();
		return false;
	}
	
	if(!ReadVertices(&info))
	{
		Log("Failed while parsing line %d", info.lineNumber);
		FreeMeshInfo(&info);
		clear();
		return false;
	}
	
	CalculateTangentArray(vertices.size(), vertices.data(), indices.size()/3, (Triangle*)indices.data());
	
	FreeMeshInfo(&info);
	return true;
}



/// ---- Model ----

Model::Model() :
	m_VertexBuffer(-1),
	m_IndexBuffer(-1),
	m_PrimitiveType(0),
	m_Size(0)
{
}

Model::~Model()
{
	clear();
}

void Model::clear()
{
	if(m_VertexBuffer != -1)
	{
		glDeleteBuffers(1, &m_VertexBuffer);
		m_VertexBuffer = -1;
	}
	
	if(m_IndexBuffer != -1)
	{
		glDeleteBuffers(1, &m_IndexBuffer);
		m_IndexBuffer = -1;
	}
}

bool Model::create( const Mesh* mesh )
{
	glGenBuffers(1, &m_VertexBuffer);
	glGenBuffers(1, &m_IndexBuffer);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size()*sizeof(Vertex), mesh->vertices.data(), GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices.size()*sizeof(unsigned short), mesh->indices.data(), GL_STATIC_DRAW);
	
	m_Size = mesh->indices.size();
	m_PrimitiveType = mesh->primitiveType;
	
	CheckGl();
	return true;
}

bool Model::load( const char* file )
{
	Mesh mesh;
	if(!mesh.load(file))
		return false;
	return create(&mesh);
}

void Model::draw() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
	
	SetVertexAttributePointers();
	glDrawElements(m_PrimitiveType, m_Size, GL_UNSIGNED_SHORT, 0);
}