#include <kj/Common.hpp>
#include <kj/Map.hpp>

int main()
{
	Map map;
	
	Voxel voxel;
	voxel.typeId = 1337;
	map.setVoxel(12,24,42, voxel);
	voxel = map.getVoxel(12,24,42);
	Log("%d == 1337", voxel.typeId);
	
	return 0;
}