#ifndef CLIENT_MAP_HPP
#define CLIENT_MAP_HPP

#include <void/Map.h>
#include <void/Singleton.h>
#include <void/Client/Model.h>
#include <void/Client/Texture.h>

enum VoxelRendering
{
	VRENDER_INVISIBLE, // No vertices will be generated
	VRENDER_SOLID,         // No transparency
	VRENDER_ALPHA          // ...
};

enum VoxelFace
{
	VFACE_NONE,
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
	TileMap();
	~TileMap();
	
	Texture2dFile* diffuse;
	
	int width;
	int tileSize;
};

struct VoxelMesh
{
	Vertex* vertices[VFACE_COUNT];
	int vertexCount[VFACE_COUNT];
};

struct ClientVoxelType
{
	int tileMap;
	int renderMode;
	int mesh;
	int faces[VFACE_COUNT];
};

class ClientMap : public Map, public SingletonClass<ClientMap>
{
	public:
		ClientMap();
		~ClientMap();
		
		int createTileMap();
		TileMap* getTileMap( int tileMap );
		const TileMap* getTileMap( int tileMap ) const;
		
		int createVoxelType( const char* name, int id = VoxelType::InvalidId );
		ClientVoxelType* getClientVoxelType( int typeId );
		const ClientVoxelType* getClientVoxelType( int typeId ) const;
		
		void updateModel( aabb3i cube );
		void draw() const;
		
	protected:
		bool createMesh( Mesh* mesh, int tileMap, aabb3i cube ) const;
		
		std::vector<TileMap> m_TileMaps;
		ClientVoxelType* m_ClientTypes[VoxelType::IdCount];
		std::vector<VoxelMesh> m_VoxelMeshes;
		
		std::vector<Model> m_Model;
		
};

#endif
