#include <kj/Map.hpp>



VoxelType InitInvalidVoxelType()
{
	VoxelType r;
	r.shape = VSHAPE_NONE;
	return r;
}
VoxelType InvalidVoxelType = InitInvalidVoxelType();

Voxel InitInvalidVoxel()
{
	Voxel r;
	r.typeId = -1;
	r.extra[0] = 0;
	r.extra[1] = 0;
	return r;
}
Voxel InvalidVoxel = InitInvalidVoxel();


// ------------------------


/// ---- VoxelChunk ---- ///


const int VoxelChunkExtend = 64; // 64^3 * sizeof(Voxel) = 1024 KB
const int VoxelChunkSize = VoxelChunkExtend*VoxelChunkExtend*VoxelChunkExtend;

class VoxelChunk
{
	public:
		VoxelChunk();
		
		Voxel* getVoxel( int x, int y, int z );
		
	private:
		Voxel m_Voxels[VoxelChunkSize];
};

VoxelChunk::VoxelChunk()
{
	for(int i = 0; i < VoxelChunkSize; i++)
		m_Voxels[i] = InvalidVoxel;
}

Voxel* VoxelChunk::getVoxel( int x, int y, int z )
{
	int index = z*VoxelChunkExtend*VoxelChunkExtend + y*VoxelChunkExtend + x;
	if(index >= VoxelChunkSize)
		return NULL;
	else
		return &m_Voxels[index];
}



/// ---- Map ---- ///

Map::Map()
{
}

Map::~Map()
{
	std::map<uint64_t,VoxelChunk*>::const_iterator i = m_Chunks.begin();
	for(; i != m_Chunks.end(); i++)
		delete i->second;
}

VoxelChunk* Map::getChunkAt( int chunkX, int chunkY, int chunkZ, bool create )
{
	union
	{
		struct
		{
			int16_t x, y, z;
		} pos;
		uint64_t key;
	} convert;
	
	convert.pos.x = chunkX;
	convert.pos.y = chunkY;
	convert.pos.z = chunkZ;
	
	std::map<uint64_t,VoxelChunk*>::const_iterator i = m_Chunks.find(convert.key);
	if(i != m_Chunks.end())
	{
		return i->second;
	}
	else if(create)
	{
		VoxelChunk* chunk = new VoxelChunk();
		m_Chunks.insert( std::pair<uint64_t,VoxelChunk*>(convert.key,chunk) );
		return chunk;
	}
	else
	{
		return NULL;
	}
}

Voxel* Map::voxelAt( int x, int y, int z, bool create )
{
	int chunkX = x/VoxelChunkSize;
	int chunkY = y/VoxelChunkSize;
	int chunkZ = z/VoxelChunkSize;
	
	VoxelChunk* chunk = getChunkAt(chunkX, chunkY, chunkZ, create);
	if(!chunk)
		return &InvalidVoxel;
	
	int offX = x-chunkX;
	int offY = y-chunkY;
	int offZ = z-chunkZ;
	
	return chunk->getVoxel(offX, offY, offZ);
}

Voxel Map::getVoxel(int x, int y, int z)
{
	return *voxelAt(x,y,z,false);
}

void Map::setVoxel(int x, int y, int z, Voxel voxel)
{
	*voxelAt(x,y,z,true) = voxel;
}

const VoxelType* Map::getVoxelType( int typeId ) const
{
	if(typeId >= m_Types.size())
		return &InvalidVoxelType;
	return &m_Types[typeId];
}