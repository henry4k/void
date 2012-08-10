#include <kj/Common.h>
#include <kj/Client/OpenGL.h>
#include <kj/Client/Map.h>
#include <kj/Client/Shader.h>
#include <kj/Client/Model.h>
#include <kj/Client/Window.h>
#include <kj/Client/Camera.h>
#include <kj/Client/Client.h>


PerspectivicCamera g_Camera;
Shader    g_Shader;
ClientMap g_Map;
Client    g_Client;


void OnKeyAction( int key, int action )
{
	// ...
}

void OnMouseMove()
{
	// ...
}

void OnResize()
{
	glViewport(0, 0, WindowWidth(), WindowHeight());
	g_Camera.setScreen( vec2f(WindowWidth(), WindowHeight()) );
}

void OnRender()
{
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
	
	{
		static vec3f pos(0,-3,0);
		
		if(KeyState('W')) pos.z += 0.1;
		if(KeyState('S')) pos.z -= 0.1;
		if(KeyState('A')) pos.x += 0.1;
		if(KeyState('D')) pos.x -= 0.1;
		if(KeyState('Q')) pos.y += 0.1;
		if(KeyState('E')) pos.y -= 0.1;
		
		g_Camera.setPosition(pos);
	}
	
	g_Camera.update();
	g_Camera.upload();
	
	{
		glPushMatrix();
		
		g_Shader.bind();
		glScalef(1, 1, 1);
		g_Map.draw();
		
		glPopMatrix();
	}
	
	CheckGl();
	
	g_Client.service();
}

bool Initialize()
{
	if(!CreateWindow("Dungeon", OnResize, OnRender, OnKeyAction, OnMouseMove))
		return false;
	
	InitializeResourceManager();
	RegisterResourceType<TextureFile>(RES_TEXTURE);
	
	glClearDepth(1);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	{
		g_Camera.setNear(0.1);
		g_Camera.setFar(100);
		g_Camera.setRotation(Quaternion(vec3f(1,0,0), -tools4k::Pi*0.5));
		g_Camera.setScreen( vec2f(WindowWidth(), WindowHeight()) );
		g_Camera.setFov(90);
	}
	
	{
		if(!g_Shader.load("Shader.vert", "Shader.frag"))
			return false;
		g_Shader.setUniform("DiffuseMap", 0);
		
		int myTileMap = g_Map.createTileMap();
		TileMap* tm = g_Map.getTileMap(myTileMap);
		tm->diffuse = (TextureFile*)LoadResource(RES_TEXTURE, "tileset.png");
		tm->tileSize = 32;
		tm->width = 128;
		
		int id = g_Map.createVoxelType("Rock");
		VoxelType* vt = g_Map.getVoxelType(id);
		vt->collisionShape = VCSHAPE_CUBE;
		
		ClientVoxelType* cvt = g_Map.getClientVoxelType(id);
		cvt->mesh = VCSHAPE_CUBE;
		cvt->renderMode = VRENDER_SOLID;
		cvt->tileMap = myTileMap;
		memset(cvt->faces, 0, sizeof(cvt->faces));
		cvt->faces[VFACE_BACK] = 2;
		cvt->faces[VFACE_TOP] = 3;
		
		Voxel voxel;
		voxel.typeId = id;
		g_Map.setVoxel(0,0,0, voxel);
		g_Map.setVoxel(1,1,1, voxel);
// 		g_Map.setVoxel(9,9,9, voxel);
// 		g_Map.setVoxel(10,10,10, voxel);
		
		g_Map.updateModel(aabb3i(vec3i(0,0,0), vec3i(10,10,10)));
	}
	
	CheckGl();
	
	ENetAddress address;
	enet_address_set_host(&address, "localhost");
	address.port = 1234;
	g_Client.connect(&address);
	
	return true;
}

void Terminate()
{
	TerminateResourceManager();
	FreeWindow();
}

int main( int argc, char* argv[] )
{
	if(Initialize())
		RunGameLoop();
	Terminate();
	return 0; 
}