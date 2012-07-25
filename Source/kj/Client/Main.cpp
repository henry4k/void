#include <kj/Common.h>
#include <kj/Client/OpenGL.h>
#include <kj/Client/Map.h>
#include <kj/Client/Shader.h>
#include <kj/Client/Model.h>
#include <kj/Client/Window.h>
#include <kj/Client/Camera.h>


Camera	g_Camera;
Handle	g_Shader;
Map	g_Map;
Model	g_MapModel;


void OnKeyAction( int key, int action )
{
	// ...
}

void OnMouseMove()
{
	// ...
}

void UpdateCamera()
{
	glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(90, WindowAspect(), 0.01, 40);
		
		UpdateCamera(&g_Camera);
		gluLookAt(
			g_Camera.px, g_Camera.py, g_Camera.pz,
			0, 0, 0,
			g_Camera.ux, g_Camera.uy, g_Camera.uz
		);
	glMatrixMode(GL_MODELVIEW);
}

void OnResize()
{
	glViewport(0, 0, WindowWidth(), WindowHeight());
}

void OnRender()
{
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
	
	UpdateCamera();
	glLoadIdentity();
	
	{
		glPushMatrix();
	
		BindShader(g_Shader);
		DrawModel(&g_MapModel);
		
		glPopMatrix();
	}
	
	CheckGl();
}

bool Initialize()
{
	if(!CreateWindow("Dungeon", OnResize, OnRender, OnKeyAction, OnMouseMove))
		return false;
	
	glClearDepth(1);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	InitCamera(&g_Camera);
	
	{
		g_Shader = LoadShader("Shader.vert", "Shader.frag");
		if(!g_Shader)
			return false;
		
		CreateMap(&g_Map);
		Voxel voxel;
		voxel.typeId = 0;
		SetVoxelAt(&g_Map, 1,1,1, voxel);
		
		Mesh mesh;
		CreateMeshFromMap(&mesh, &g_Map, 0,0,0, 10,10,10);
		CreateModel(&g_MapModel, &mesh);
		FreeMesh(&mesh);
	}
	
	CheckGl();
	return true;
}

void Terminate()
{
	FreeShader(g_Shader);
	FreeModel(&g_MapModel);
	FreeMap(&g_Map);
	FreeWindow();
}

int main( int argc, char* argv[] )
{
	if(Initialize())
		RunGameLoop();
	Terminate();
	return 0; 
}