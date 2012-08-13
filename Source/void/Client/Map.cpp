#include <tools4k/Vector.h>
#include <void/Resource.h>
#include <void/Client/OpenGL.h>
#include <void/Client/Map.h>

using tools4k::vec2f;
using tools4k::vec3f;
using tools4k::vec3i;


Vertex CreateVertex(
	float px, float py, float pz,
	float tx, float ty,
	float nx, float ny, float nz
	)
{
	Vertex v;
	v.position.x = px;
	v.position.y = py;
	v.position.z = pz;
	v.texCoord.x = tx;
	v.texCoord.y = ty;
	v.normal.x = nx;
	v.normal.y = ny;
	v.normal.z = nz;
	return v;
}

const int AverageVerticesPerVoxel = 6*4; // block with 6 visible faces

struct VoxelModel
{
	Vertex* vertices[VFACE_COUNT];
	int vertexCount[VFACE_COUNT];
};

VoxelModel CubeModel;
Vertex CubeTop[] =
{
	CreateVertex(0,0,1, 0,0, 0,-1,0),
	CreateVertex(1,0,1, 1,0, 0,-1,0),
	CreateVertex(1,1,1, 1,1, 0,-1,0),
	CreateVertex(0,1,1, 0,1, 0,-1,0)
};
Vertex CubeBottom[] =
{
	CreateVertex(0,0,0, 0,0, 0,-1,0),
	CreateVertex(0,1,0, 0,1, 0,-1,0),
	CreateVertex(1,1,0, 1,1, 0,-1,0),
	CreateVertex(1,0,0, 1,0, 0,-1,0)
};
Vertex CubeFront[] =
{
	CreateVertex(0,1,0, 0,0, 0,-1,0),
	CreateVertex(0,1,1, 0,1, 0,-1,0),
	CreateVertex(1,1,1, 1,1, 0,-1,0),
	CreateVertex(1,1,0, 1,0, 0,-1,0)
};
Vertex CubeBack[] =
{
	CreateVertex(0,0,0, 0,0, 0,-1,0),
	CreateVertex(1,0,0, 1,0, 0,-1,0),
	CreateVertex(1,0,1, 1,1, 0,-1,0),
	CreateVertex(0,0,1, 0,1, 0,-1,0)
};
Vertex CubeLeft[] =
{
	CreateVertex(0,0,0, 0,0, 0,-1,0),
	CreateVertex(0,0,1, 0,1, 0,-1,0),
	CreateVertex(0,1,1, 1,1, 0,-1,0),
	CreateVertex(0,1,0, 1,0, 0,-1,0)
};
Vertex CubeRight[] =
{
	CreateVertex(1,0,0, 0,0, 0,-1,0),
	CreateVertex(1,1,0, 1,0, 0,-1,0),
	CreateVertex(1,1,1, 1,1, 0,-1,0),
	CreateVertex(1,0,1, 0,1, 0,-1,0)
};

bool CreateVoxelModels()
{
	CubeModel.vertices[VFACE_NONE]    = NULL;
	CubeModel.vertexCount[VFACE_NONE] = 0;
	
	CubeModel.vertices[VFACE_FRONT]    = CubeFront;
	CubeModel.vertexCount[VFACE_FRONT] = sizeof(CubeFront)/sizeof(Vertex);
	
	CubeModel.vertices[VFACE_BACK]    = CubeBack;
	CubeModel.vertexCount[VFACE_BACK] = sizeof(CubeBack)/sizeof(Vertex);
	
	CubeModel.vertices[VFACE_TOP]    = CubeTop;
	CubeModel.vertexCount[VFACE_TOP] = sizeof(CubeTop)/sizeof(Vertex);
	
	CubeModel.vertices[VFACE_BOTTOM]    = CubeBottom;
	CubeModel.vertexCount[VFACE_BOTTOM] = sizeof(CubeBottom)/sizeof(Vertex);
	
	CubeModel.vertices[VFACE_LEFT]    = CubeLeft;
	CubeModel.vertexCount[VFACE_LEFT] = sizeof(CubeLeft)/sizeof(Vertex);
	
	CubeModel.vertices[VFACE_RIGHT]    = CubeRight;
	CubeModel.vertexCount[VFACE_RIGHT] = sizeof(CubeRight)/sizeof(Vertex);

	return true;
}
bool CreatedVoxelModels = CreateVoxelModels();



/// ---- TileMap ----

TileMap::TileMap() :
	diffuse(NULL),
	tileSize(0)
{
}

TileMap::~TileMap()
{
}


/// ---- ClientMap ----

ClientMap::ClientMap()
{
	memset(m_ClientTypes, 0, sizeof(m_ClientTypes));
}

ClientMap::~ClientMap()
{
	for(int j = 0; j < VoxelType::IdCount; j++)
		if(m_ClientTypes[j])
			delete m_ClientTypes[j];
	
	std::vector<TileMap>::const_iterator i = m_TileMaps.begin();
	for(; i != m_TileMaps.end(); i++)
		if(i->diffuse)
			ReleaseResource(i->diffuse);
}



int ClientMap::createTileMap()
{
	m_TileMaps.resize(m_TileMaps.size()+1);
	return m_TileMaps.size()-1;
}

TileMap* ClientMap::getTileMap( int tileMap )
{
	return &m_TileMaps[tileMap];
}

const TileMap* ClientMap::getTileMap( int tileMap ) const
{
	return &m_TileMaps[tileMap];
}





int ClientMap::createVoxelType(const char* name, int id)
{
	id = Map::createVoxelType(name, id);
	if(id == VoxelType::InvalidId)
		return VoxelType::InvalidId;
	
	m_ClientTypes[id] = new ClientVoxelType;
	return id;
}

ClientVoxelType* ClientMap::getClientVoxelType( int typeId )
{
	if(typeId >= VoxelType::IdCount)
		return NULL;
	return m_ClientTypes[typeId];
}

const ClientVoxelType* ClientMap::getClientVoxelType( int typeId ) const
{
	if(typeId >= VoxelType::IdCount)
		return NULL;
	return m_ClientTypes[typeId];
}





struct GeneratorContext
{
	const Map* map;
	
	std::vector<Vertex> vertices;
	
	
	aabb3i cube;
	
	// current voxel
	vec3i offset;
	vec3i absolute;
	Voxel voxel;
	const VoxelType* voxelType;
	const ClientVoxelType* clientVoxelType;
	const TileMap* tileMap;
};

vec2f CalcTexCoord( vec2f originalTexCoord, const TileMap* tm, int tileIndex )
{
	const int tilesPerRow = tm->width / tm->tileSize;
	const float tileTexSize = double(tm->tileSize) / double(tm->width);
	
	vec2f r =
		originalTexCoord*tileTexSize +
		vec2f(
			(tileIndex % tilesPerRow)*tileTexSize,
			(tileIndex / tilesPerRow)*tileTexSize
		);
	
	Log("%f/%f => %f/%f", originalTexCoord.x, originalTexCoord.y, r.x, r.y);
	
	return r;
}

void AddCube( GeneratorContext* ctx )
{
	for(int face = 0; face < VFACE_COUNT; face++)
	{
		for(int vertex = 0; vertex < CubeModel.vertexCount[face]; vertex++)
		{
			Vertex* s = &CubeModel.vertices[face][vertex];
			Vertex d;
			
			d.position = ctx->offset + s->position;
			d.texCoord = CalcTexCoord(s->texCoord, ctx->tileMap, ctx->clientVoxelType->faces[face]);
			d.normal = s->normal;
			
			ctx->vertices.push_back(d);
		}
	}
}


bool ClientMap::createMesh( Mesh* mesh, int tileMap, aabb3i cube ) const
{
	vec3i size = cube.size();
	
	GeneratorContext ctx;
	ctx.map = this;
	
	
	ctx.vertices.reserve(size.volume()*AverageVerticesPerVoxel);
	
	ctx.cube = cube;
	ctx.offset = vec3i(0,0,0);
	
	for(ctx.offset.x = 0; ctx.offset.x < size.x; ctx.offset.x++) 
	for(ctx.offset.y = 0; ctx.offset.y < size.y; ctx.offset.y++) 
	for(ctx.offset.z = 0; ctx.offset.z < size.z; ctx.offset.z++) 
	{
		ctx.absolute = cube.min + ctx.offset;
		
		ctx.voxel = getVoxel(ctx.absolute.x,ctx.absolute.y,ctx.absolute.z);
		if(ctx.voxel.typeId == VoxelType::InvalidId)
			continue;
		
		ctx.voxelType = getVoxelType(ctx.voxel.typeId);
		if(!ctx.voxelType)
			continue;
		
		ctx.clientVoxelType = getClientVoxelType(ctx.voxel.typeId);
		if(ctx.clientVoxelType->tileMap != tileMap)
			continue;
		
		ctx.tileMap = &m_TileMaps[ctx.clientVoxelType->tileMap];
		
		switch(ctx.voxelType->collisionShape)
		{
			case VCSHAPE_CUBE:
				AddCube(&ctx);
				break;
			
			case VCSHAPE_NONE:
			default:
				// Do nothing
				;
		}
	}
	
	
	mesh->vertices.clear();
	mesh->vertices.swap(ctx.vertices);
	
	Log("Vertex count: %d", mesh->vertices.size());
	
	mesh->indices.resize(mesh->vertices.size());
	for(int i = 0; i < mesh->indices.size(); i++)
		mesh->indices[i] = i;
	
	mesh->primitiveType = GL_QUADS;
	
	return true;
}

void ClientMap::updateModel( aabb3i cube )
{
	Mesh mesh;
	m_Model.resize(m_TileMaps.size());
	for(int i = 0; i < m_Model.size(); i++)
	{
		createMesh(&mesh, i, cube);
		m_Model[i].create(&mesh);
	}
}

void ClientMap::draw() const
{
	for(int i = 0; i < m_Model.size(); i++)
	{
		m_TileMaps[i].diffuse->bind(0);
		m_Model[i].draw();
	}
}