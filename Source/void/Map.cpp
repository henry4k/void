#include <void/Map.h>


/*
VoxelType InitInvalidVoxelType()
{
	VoxelType r(VoxelType::InvalidId, "Invalid");
	r.collisionShape = VCSHAPE_CUBE; // VCSHAPE_NONE;
	return r;
}
VoxelType InvalidVoxelType = InitInvalidVoxelType();
*/

Voxel InitInvalidVoxel()
{
	Voxel r;
	r.typeId = VoxelType::InvalidId;
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
		
		Voxel getVoxel( int x, int y, int z ) const;
		void setVoxel( int x, int y, int z, Voxel v );
		
	private:
		Voxel m_Voxels[VoxelChunkSize];
};

VoxelChunk::VoxelChunk()
{
	for(int i = 0; i < VoxelChunkSize; i++)
		m_Voxels[i] = InvalidVoxel;
}

Voxel VoxelChunk::getVoxel( int x, int y, int z ) const
{
	int index = z*VoxelChunkExtend*VoxelChunkExtend + y*VoxelChunkExtend + x;
	if(index >= VoxelChunkSize)
		return InvalidVoxel;
	else
		return m_Voxels[index];
}

void VoxelChunk::setVoxel( int x, int y, int z, Voxel v )
{
	int index = z*VoxelChunkExtend*VoxelChunkExtend + y*VoxelChunkExtend + x;
	m_Voxels[index] = v;
}



/// ---- Map ---- ///

Map::Map()
{
	memset(m_Types, 0, sizeof(m_Types));
}

Map::~Map()
{
	std::map<uint64_t,VoxelChunk*>::const_iterator i = m_Chunks.begin();
	for(; i != m_Chunks.end(); i++)
		delete i->second;
	
	for(int j = 0; j < VoxelType::IdCount; j++)
		if(m_Types[j])
			delete m_Types[j];
}

VoxelChunk* Map::getChunkAt( int chunkX, int chunkY, int chunkZ ) const
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
	else
	{
		return NULL;
	}
}

VoxelChunk* Map::createChunkAt( int chunkX, int chunkY, int chunkZ )
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
	else
	{
		VoxelChunk* chunk = new VoxelChunk();
		m_Chunks.insert( std::pair<uint64_t,VoxelChunk*>(convert.key,chunk) );
		return chunk;
	}
}

Voxel Map::getVoxel( vec3i pos ) const
{
	int chunkX = pos.x/VoxelChunkSize;
	int chunkY = pos.y/VoxelChunkSize;
	int chunkZ = pos.z/VoxelChunkSize;
	
	VoxelChunk* chunk = getChunkAt(chunkX, chunkY, chunkZ);
	if(!chunk)
		return InvalidVoxel;
	
	int offX = pos.x-chunkX;
	int offY = pos.y-chunkY;
	int offZ = pos.z-chunkZ;
	
	return chunk->getVoxel(offX, offY, offZ);
}

void Map::setVoxel( vec3i pos, Voxel voxel )
{
	int chunkX = pos.x/VoxelChunkSize;
	int chunkY = pos.y/VoxelChunkSize;
	int chunkZ = pos.z/VoxelChunkSize;
	
	VoxelChunk* chunk = createChunkAt(chunkX, chunkY, chunkZ);
	
	int offX = pos.x-chunkX;
	int offY = pos.y-chunkY;
	int offZ = pos.z-chunkZ;
	
	chunk->setVoxel(offX, offY, offZ, voxel);
}

int Map::getFreeVoxelType() const
{
	// Find first free id
	
	int id = 0;
	for(;; id++)
	{
		if(id >= VoxelType::IdCount)
			return VoxelType::InvalidId;
		
		if(m_Types[id] == NULL)
			break;
	}
	
	return id;
}

int Map::createVoxelType( const char* name, int id )
{
	if(id == VoxelType::InvalidId)
	{
		id = getFreeVoxelType();
		if(id == VoxelType::InvalidId)
			return VoxelType::InvalidId;
	}
	
	if(id >= VoxelType::IdCount)
		return VoxelType::InvalidId;
	
	if(m_Types[id])
		return VoxelType::InvalidId;
	
	if(m_TypeIdByName.count(name))
		return VoxelType::InvalidId;
	
	m_Types[id] = new VoxelType(id, name);
	m_TypeIdByName[name] = id;
	return id;
}

int Map::getTypeIdByName(const char* name) const
{
	std::map<std::string,int>::const_iterator i = m_TypeIdByName.find(name);
	if(i != m_TypeIdByName.end())
		return i->second;
	else
		return VoxelType::InvalidId;
}

VoxelType* Map::getVoxelType( int typeId )
{
	if(typeId >= VoxelType::IdCount)
		return NULL;
	return m_Types[typeId];
}

const VoxelType* Map::getVoxelType( int typeId ) const
{
	if(typeId >= VoxelType::IdCount)
		return NULL;
	return m_Types[typeId];
}
