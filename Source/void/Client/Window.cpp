#include <void/Common.h>
#include <void/Client/OpenGL.h>
#include <void/Client/Model.h>
#include <void/Client/Window.h>






const char* GetGlString( GLenum property )
{
	const char* str = (const char*)glGetString(property);
	if(!str)
	{
		CheckGl();
		return "Error!";
	}
	return str;
}

const char* GetGlewString( GLenum property )
{
	const char* str = (const char*)glewGetString(property);
	if(!str)
	{
		CheckGl();
		return "Error!";
	}
	return str;
}






Window::Window( IWindowEventListener* eventListener, vec2i size ) :
	m_EventListener(eventListener),
	m_Size(size),
	m_MousePos(size/2)
{
	if(s_Singleton)
		Error("The faq");
	s_Singleton = this;
	
	if(!glfwInit())
		Error("Failed to initialize GLFW");
	
	if(!glfwOpenWindow(size.x, size.y, 8, 8, 8, 8, 24, 0, GLFW_WINDOW))
		Error("Failed to open GLFW window");
	
	glfwSwapInterval(1); // use vsync
	
	glfwSetWindowSizeCallback(Window::OnResize);
	glfwSetKeyCallback(Window::OnKeyAction);
	glfwSetMousePosCallback(Window::OnMouseMove);
	
	glewExperimental = GL_FALSE;
	GLenum err = glewInit();
	if(err != GLEW_OK)
		Error("%s", glewGetErrorString(err));
	
	Log("OpenGL %s",    GetGlString(GL_VERSION));
	Log("  Vendor: %s", GetGlString(GL_VENDOR));
	Log("Renderer: %s", GetGlString(GL_RENDERER));
	Log("    GLSL: %s", GetGlString(GL_SHADING_LANGUAGE_VERSION));
	Log("    GLEW: %s", GetGlewString(GLEW_VERSION));
	
	#define CHECK_EXT(E) if(! GLEW_##E ) Error(#E" not supported!");
		CHECK_EXT(ARB_multitexture)
		CHECK_EXT(ARB_vertex_buffer_object)
		CHECK_EXT(ARB_shader_objects)
	#undef CHECK_EXT
	
	EnableVertexArrays();
	
	m_EventListener->onResizeWindow(this);
}

Window::~Window()
{
	glfwTerminate();
}

void Window::setName( const char* name )
{
	glfwSetWindowTitle(name);
}

vec2i Window::size() const
{
	return m_Size;
}

float Window::aspect() const
{
	return float(m_Size.x)/float(m_Size.y);
}

vec2i Window::mousePos() const
{
	return m_MousePos;
}

int Window::keyState( int key ) const
{
	return glfwGetKey(key);
}

void Window::swapBuffers() const
{
	glfwSwapBuffers();
}

bool Window::isOpen() const
{
	return (glfwGetWindowParam(GLFW_OPENED) == 1);
}

Window* Window::s_Singleton = NULL;

void Window::OnResize( int width, int height )
{
	s_Singleton->m_Size.x = width;
	s_Singleton->m_Size.y = height;
	s_Singleton->m_EventListener->onResizeWindow(s_Singleton);
}

void Window::OnKeyAction( int key, int action )
{
// 	if(action && (key == GLFW_KEY_ESC))
// 		Running = false; // TODO!
	s_Singleton->m_EventListener->onKeyAction(s_Singleton, key, action);
}

void Window::OnMouseMove( int x, int y )
{
	s_Singleton->m_MousePos.x = x;
	s_Singleton->m_MousePos.y = y;
	s_Singleton->m_EventListener->onMouseMove(s_Singleton);
}