#ifndef __VOID_MAP_H__
#define __VOID_MAP_H__

#include <stdint.h>
#include <map>
#include <vector>
#include <string>

#include <tools4k/Vector.h>
#include <tools4k/Aabb.h>

#include <void/Common.h>
#include <void/Singleton.h>

using tools4k::vec3i;
using tools4k::aabb3i;


enum VoxelCollisionShapes
{
	VCSHAPE_NONE, // This voxel is empty
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

class Map : public SingletonClass<Map>
{
	public:
		Map();
		~Map();
		
		/**
		 * Reads a voxel at the given coordinates.
		 * When the voxel does not exist (i.e. there is no chunk created yet)
		 * this function returns InvalidVoxel.
		 * InvalidVoxel is an internal voxel that has the type InvalidVoxelType (-1).
		 * @see getVoxelType()
		 * @see setVoxel()
		 */
		Voxel getVoxel( vec3i pos ) const;

		/**
		 * Sets the voxel at the given coordinates.
		 */
		void setVoxel( vec3i pos, Voxel voxel );
		
		/**
		 * Returns the next free voxel type id.
		 * This value stays constant between calls to createVoxelType().
		 * Will return VoxelType::InvalidId when no ids are left.
		 */
		int getFreeVoxelType() const; // TODO: Warum ist das eigentlich öffentlich? Irgendwie braucht man ja nur createVoxelType() ...

		/**
		 * Allocates a new type. // <- Bad english :(
		 * When id is VoxelType::InvalidId a new type id is generated.
		 */
		virtual int createVoxelType( const char* name, int id = VoxelType::InvalidId );
		
		/**
		 * Returns the id that is associated with this name
		 * or VoxelType::InvalidId when there is no voxel with that name.
		 */
		int getTypeIdByName( const char* name ) const;
		
		/**
		 * Returns the voxel type structure
		 * or NULL when something went wrong.
		 */
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
