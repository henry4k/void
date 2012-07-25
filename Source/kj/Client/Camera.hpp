#include <string.h>
#include <math.h>

struct Camera
{
	float radius;
	float rx, ry, rz; // rotation
	
	float px, py, pz; // position
	float ux, uy, uz; // up vector
};

void InitCamera( Camera* cam )
{
	memset(cam, 0, sizeof(Camera));
	cam->radius = 20;
}

void UpdateCamera( Camera* cam )
{
	// Input
	
	if(KeyState('Q')) cam->rz += 0.01;
	if(KeyState('E')) cam->rz -= 0.01;
	if(KeyState('W')) cam->ry += 0.01;
	if(KeyState('S')) cam->ry -= 0.01;
	if(KeyState('A')) cam->rx += 0.01;
	if(KeyState('D')) cam->rx -= 0.01;
	
	
	// Calculation
	
	// up vector
	cam->ux = 0; // sin(cam->rz);
	cam->uy = 1; // cos(cam->rz);
	cam->uz = 0;
	
	
	// position
	
	cam->px = 0;
	cam->py = 0;
	cam->pz = 0;
	
// 	cam->px += sin(cam->ry)*cam->radius;
// 	cam->py += cos(cam->ry)*cam->radius;
	
	cam->pz += sin(cam->rx)*cam->radius;
	cam->px += cos(cam->rx)*cam->radius;
}