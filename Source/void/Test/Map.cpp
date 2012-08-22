#include <void/Common.h>
#include <void/Map.h>

int main()
{
	Map map;
	
	Voxel voxel;
	voxel.typeId = 1337;
	map.setVoxel(vec3i(12,24,42), voxel);
	voxel = map.getVoxel(vec3i(12,24,42));
	Log("%d == 1337", voxel.typeId);
	
	return 0;
}