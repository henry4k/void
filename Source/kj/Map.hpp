#ifndef MAP_HPP
#define MAP_HPP

#include <stdint.h>
#include <map>
#include <vector>
#include <string>
#include "Common.hpp"


enum VoxelShapes
{
	VSHAPE_NONE = 0, // This voxel is empty
	VSHAPE_CUBE      // This voxel is a cube
};

enum VoxelRendering
{
	VRENDER_INVISIBLE = 0, // No vertices will be generated at all
	VRENDER_SOLID,         // No transparency at all
	VRENDER_ALPHA          // ...
};

enum VoxelFace
{
	VFACE_NONE = 0,
	VFACE_FRONT,
	VFACE_BACK,
	VFACE_TOP,
	VFACE_BOTTOM,
	VFACE_LEFT,
	VFACE_RIGHT,
	VFACE_COUNT,
};



struct TileMap
{
	std::string fileName;
};

struct VoxelType
{
	int tileMap;
	std::string name;
	int shape;
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
		
		Voxel getVoxel(int x, int y, int z);
		void setVoxel(int x, int y, int z, Voxel voxel);
		
		const VoxelType* getVoxelType( int typeId ) const;
		
	private:
		VoxelChunk* getChunkAt( int chunkX, int chunkY, int chunkZ, bool create );
		Voxel* voxelAt( int x, int y, int z, bool create );
		
		std::vector<TileMap> m_TileMaps;
		std::vector<VoxelType> m_Types;
		std::map<uint64_t,VoxelChunk*> m_Chunks;
};







#endif