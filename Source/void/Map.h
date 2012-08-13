#ifndef MAP_HPP
#define MAP_HPP

#include <stdint.h>
#include <map>
#include <vector>
#include <string>
#include <void/Common.h>


enum VoxelCollisionShapes
{
	VCSHAPE_NONE = 0, // This voxel is empty
	VCSHAPE_CUBE      // This voxel is a cube
};


class VoxelType
{
	public:
		static const int IdCount = (1 << 12); // 4096
		static const int InvalidId = -1;
		
		VoxelType( int id, const char* name ) :
			m_Id(id),
			m_Name(name)
		{
		}
		
		int id() const { return m_Id; }
		const char* name() const { return m_Name.c_str(); }
		
		int collisionShape;
		
	private:
		int m_Id;
		std::string m_Name;
};

struct Voxel
{
	unsigned short typeId;
	unsigned char extra[2]; //flipX, flipY, ...
};

class VoxelChunk;

class Map
{
	public:
		Map();
		~Map();
		
		Voxel getVoxel(int x, int y, int z) const;
		void setVoxel(int x, int y, int z, Voxel voxel);
		
		
		int getFreeVoxelType() const;
		virtual int createVoxelType( const char* name, int id = VoxelType::InvalidId );
		
		int getTypeIdByName( const char* name ) const;
		
		VoxelType* getVoxelType( int typeId );
		const VoxelType* getVoxelType( int typeId ) const;
		
	private:
		VoxelChunk* getChunkAt( int chunkX, int chunkY, int chunkZ ) const;
		VoxelChunk* createChunkAt( int chunkX, int chunkY, int chunkZ );
		
		VoxelType* m_Types[VoxelType::IdCount];
		std::map<std::string,int> m_TypeIdByName;
		
		std::map<uint64_t,VoxelChunk*> m_Chunks;
};







#endif