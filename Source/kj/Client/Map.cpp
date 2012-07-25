#include <kj/Client/OpenGL.hpp>
#include <kj/Client/Map.hpp>


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

const int MaxVerticesPerVoxel = 6*4; // block with 6 visible faces

struct VoxelModel
{
	Vertex* vertices[VFACE_COUNT];
	int vertexCount[VFACE_COUNT];
};

VoxelModel CubeModel;
Vertex CubeFront[] =
{
	CreateVertex(0,0,0, 0,0, 0,-1,0),
	CreateVertex(1,0,0, 1,0, 0,-1,0),
	CreateVertex(1,1,0, 1,1, 0,-1,0),
	CreateVertex(0,1,0, 0,1, 0,-1,0)
};

bool CreateVoxelModels()
{
	CubeModel.vertices[VFACE_NONE]    = NULL;
	CubeModel.vertexCount[VFACE_NONE] = 0;
	
	CubeModel.vertices[VFACE_FRONT]    = CubeFront;
	CubeModel.vertexCount[VFACE_FRONT] = sizeof(CubeFront)/sizeof(Vertex);
	
	CubeModel.vertices[VFACE_BACK]    = NULL;
	CubeModel.vertexCount[VFACE_BACK] = 0;
	
	CubeModel.vertices[VFACE_TOP]    = NULL;
	CubeModel.vertexCount[VFACE_TOP] = 0;
	
	CubeModel.vertices[VFACE_BOTTOM]    = NULL;
	CubeModel.vertexCount[VFACE_BOTTOM] = 0;
	
	CubeModel.vertices[VFACE_LEFT]    = NULL;
	CubeModel.vertexCount[VFACE_LEFT] = 0;
	
	CubeModel.vertices[VFACE_RIGHT]    = NULL;
	CubeModel.vertexCount[VFACE_RIGHT] = 0;
}
bool CreatedVoxelModels = CreateVoxelModels();

struct GeneratorContext
{
	const Map* map;
	
	int max;
	Vertex* vertex;
	int i;
	
	int start[3];
	int size[3];
	
	// current voxel
	int op[3];
	int ap[3];
	Voxel voxel;
	const VoxelType* voxelType;
};

void AddCube( GeneratorContext* ctx )
{
	for(int face = 0; face < VFACE_COUNT; face++)
	{
		for(int vertex = 0; vertex < CubeModel.vertexCount[face]; vertex++)
		{
			Vertex* s = &CubeModel.vertices[face][vertex];
			Vertex* d = &ctx->vertex[ctx->i];
			
			float txoff = ctx.voxelType->faces[face*2+0];
			float tyoff = ctx.voxelType->faces[face*2+1];
			
			d->position.x = ctx->op[0] + s->position.x;
			d->position.y = ctx->op[1] + s->position.y;
			d->position.z = ctx->op[2] + s->position.z;
			d->texCoord.x = s->texCoord.x;
			d->texCoord.y = s->texCoord.y;
			d->normal.x = s->normal.x;
			d->normal.y = s->normal.y;
			d->normal.z = s->normal.z;
			
			ctx->i++;
		}
	}
}

void CreateMeshFromMap( Mesh* mesh, const Map* map, int xstart, int ystart, int zstart, int width, int height, int depth )
{
	GeneratorContext ctx;
	ctx.map = map;
	ctx.max = width*height*depth*MaxVerticesPerVoxel;
	ctx.vertex = Alloc(Vertex, ctx.max);
	ctx.i = 0;
	ctx.start[0] = xstart;
	ctx.start[1] = ystart;
	ctx.start[2] = zstart;
	ctx.size[0] = width;
	ctx.size[1] = height;
	ctx.size[2] = depth;
	ctx.op[0] = 0;
	ctx.op[1] = 0;
	ctx.op[2] = 0;
	
	for(ctx.op[0] = 0; ctx.op[0] < ctx.size[0]; ctx.op[0]++) 
	for(ctx.op[1] = 0; ctx.op[1] < ctx.size[1]; ctx.op[1]++) 
	for(ctx.op[2] = 0; ctx.op[2] < ctx.size[2]; ctx.op[2]++) 
	{
		ctx.ap[0] = ctx.op[0]+ctx.start[0];
		ctx.ap[1] = ctx.op[1]+ctx.start[1];
		ctx.ap[2] = ctx.op[2]+ctx.start[2];
		
		ctx.voxel = GetVoxelAt(map, ctx.ap[0],ctx.ap[1],ctx.ap[2]);
		ctx.voxelType = GetVoxelType(map, ctx.voxel.typeId);
		
		switch(ctx.voxelType->shape)
		{
			case VSHAPE_CUBE:
				AddCube(&ctx);
				break;
			
			case VSHAPE_NONE:
			default:
				// Do nothing
				;
		}
	}
	
	mesh->vertexCount = ctx.i;
	mesh->vertices = Realloc(Vertex, ctx.vertex, mesh->vertexCount);
	
	mesh->indexCount = mesh->vertexCount;
	mesh->indices = Alloc(unsigned short, mesh->indexCount);
	for(int i = 0; i < mesh->indexCount; i++)
	{
		mesh->indices[i] = i;
	}
	
	mesh->primitiveType = GL_QUADS;
}