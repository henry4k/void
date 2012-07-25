#ifndef CLIENT_MAP_HPP
#define CLIENT_MAP_HPP

#include <kj/Map.hpp>
#include <kj/Client/Model.hpp>
#include <kj/Client/Texture.hpp>


struct ClientMaterial
{
	Handle texture;
	// Shader .. ?
	// Blending .. ?
};

struct ClientVoxelType
{
	int material;
	// i
};

void CreateMeshFromMap( Mesh* mesh, const Map* map, int x, int y, int z, int w, int h, int d );

#endif