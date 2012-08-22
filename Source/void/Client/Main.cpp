#include <void/Common.h>
#include <void/Client/OpenGL.h>
#include <void/Client/Map.h>
#include <void/Client/Shader.h>
#include <void/Client/Model.h>
#include <void/Client/Window.h>
#include <void/Client/Camera.h>
#include <void/Client/Client.h>
#include <void/Client/Object.h>
#include <void/Engine.h>


class ClientEngine : public Engine, private IWindowEventListener
{
	public:
		ClientEngine();
		bool initialize();
		virtual ~ClientEngine();
		
	private:
		bool renderScene();
		bool onUpdate( double timeDelta );
		
		void onResizeWindow( Window* wnd );
		void onMouseMove( Window* wnd );
		void onKeyAction( Window* wnd, int key, int action );
		
		Window m_Window;
		Client m_Client;
		ClientObjectManager m_ObjectManager;
		ClientMap m_ClientMap;
		PerspectivicCamera m_Camera;
		Shader m_Shader;
};


ClientEngine::ClientEngine() : 
	Engine(),
	m_Window(this, vec2i(800,600)),
	m_Client(),
	m_ObjectManager(),
	m_ClientMap(),
	m_Camera(),
	m_Shader()
{
	// Setup Squirrel
	m_Squirrel.setConst("__server__", 0);
	m_Squirrel.setConst("__client__", 1);
}

bool ClientEngine::initialize()
{
	// Setup Window
	m_Window.setName("void");
	
	// Connect to server
	ENetAddress address;
	if(enet_address_set_host(&address, "localhost") != 0)
		return false;
	address.port = 1234;
	if(!m_Client.connect(&address))
		return false;
	
	// Setup Resource Manager
	m_ResourceManager.registerResourceType<Texture2dFile>(RES_TEXTURE2D);
	
	// Setup OpenGL
	glClearDepth(1);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	// Setup camera
	m_Camera.setNear(0.1);
	m_Camera.setFar(100);
	m_Camera.setRotation(Quaternion(vec3f(1,0,0), -tools4k::Pi*0.5));
	m_Camera.setScreen(m_Window.size());
	m_Camera.setFov(90);
	
	// Setup shader
	if(!m_Shader.load("Shader.vert", "Shader.frag"))
		return false;
	m_Shader.setUniform("DiffuseMap", 0);
	
	// Setup map
	{
		int myTileMap = m_ClientMap.createTileMap();
		TileMap* tm = m_ClientMap.getTileMap(myTileMap);
		tm->diffuse = (Texture2dFile*)m_ResourceManager.loadResource(RES_TEXTURE2D, "tileset.png");
		tm->tileSize = 32;
		tm->width = 128;
		
		int id = m_ClientMap.createVoxelType("Rock");
		VoxelType* vt = m_ClientMap.getVoxelType(id);
		vt->collisionShape = VCSHAPE_CUBE;
		
		ClientVoxelType* cvt = m_ClientMap.getClientVoxelType(id);
		cvt->mesh = VCSHAPE_CUBE;
		cvt->renderMode = VRENDER_SOLID;
		cvt->tileMap = myTileMap;
		memset(cvt->faces, 0, sizeof(cvt->faces));
		cvt->faces[VFACE_BACK] = 1;
		cvt->faces[VFACE_TOP] = 4;
		
		Voxel voxel;
		voxel.typeId = id;
		m_ClientMap.setVoxel(vec3i(0,0,0), voxel);
		m_ClientMap.setVoxel(vec3i(1,1,1), voxel);
		
		m_ClientMap.updateModel(aabb3i(vec3i(0,0,0), vec3i(10,10,10)));
	}
	
	if(!loadPackage("Base"))
		return false;
	
	return true;
}

ClientEngine::~ClientEngine()
{
}

void ClientEngine::onResizeWindow( Window* wnd )
{
	glViewport(0, 0, m_Window.size().x, m_Window.size().y);
	m_Camera.setScreen(m_Window.size());
}

void ClientEngine::onMouseMove( Window* wnd )
{
	// ...
}

void ClientEngine::onKeyAction( Window* wnd, int key, int action )
{
	// ...
}

bool ClientEngine::renderScene()
{
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
	
	{
		static vec3f pos(0,-3,0);
		
		if(m_Window.keyState('W')) pos.z += 0.1;
		if(m_Window.keyState('S')) pos.z -= 0.1;
		if(m_Window.keyState('A')) pos.x += 0.1;
		if(m_Window.keyState('D')) pos.x -= 0.1;
		if(m_Window.keyState('Q')) pos.y += 0.1;
		if(m_Window.keyState('E')) pos.y -= 0.1;
		
		m_Camera.setPosition(pos);
	}
	
	m_Camera.update();
	m_Camera.upload();
	
	{
		glPushMatrix();
		
		m_Shader.bind();
		glScalef(1, 1, 1);
		m_ClientMap.draw();
		
		glPopMatrix();
	}
	
	CheckGl();
	
	m_Window.swapBuffers();
	return m_Window.isOpen();
}

bool ClientEngine::onUpdate(double timeDelta)
{
	m_Client.service();
	if(!renderScene())
		return false;
	m_ObjectManager.update();
	return true;
}



int main( int argc, char* argv[] )
{
	ClientEngine engine;
	if(!engine.initialize())
		return 0;
	while(engine.update())
		;
	return 0; 
}
