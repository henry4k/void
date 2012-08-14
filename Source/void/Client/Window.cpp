#include <void/Common.h>
#include <void/Client/OpenGL.h>
#include <void/Client/Model.h>
#include <void/Client/Window.h>


bool Running = true;
int WindowWidth_ = 800;
int WindowHeight_ = 600;
int MouseX_ = 0;
int MouseY_ = 0;
float TimeDelta_;

IWindowEventListener* WindowEventListener;


void WindowResize( int width, int height )
{
	if((width == WindowWidth_) && (height && WindowHeight_))
		return;
	WindowWidth_ = width;
	WindowHeight_ = height;
	WindowEventListener->onResizeWindow(NULL);
}

void WindowKeyAction( int key, int action )
{
	if(action && (key == GLFW_KEY_ESC))
		Running = false;
	WindowEventListener->onKeyAction(NULL, key, action);
}

void WindowMouseAction( int x, int y )
{
	MouseX_ = x;
	MouseY_ = y;
	WindowEventListener->onMouseMove(NULL);
}

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

bool CreateWindow( const char* name, IWindowEventListener* eventListener )
{
	if(!glfwInit())
	{
		Error("Failed to initialize GLFW");
		return false;
	}
	
	if(!glfwOpenWindow(WindowWidth_, WindowHeight_, 8, 8, 8, 8, 24, 0, GLFW_WINDOW))
	{
		Error("Failed to open GLFW window");
		DestroyWindow();
		return false;
	}
	
	WindowEventListener = eventListener;
	
	glfwSetWindowTitle(name);
	
	glfwSwapInterval(1); // use vsync
	
	glfwSetWindowSizeCallback(WindowResize);
	glfwSetKeyCallback(WindowKeyAction);
	glfwSetMousePosCallback(WindowMouseAction);
	
	glewExperimental = GL_FALSE;
	GLenum err = glewInit();
	if(err != GLEW_OK)
	{
		Error("%s", glewGetErrorString(err));
		DestroyWindow();
		return false;
	}
	
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
	
	WindowEventListener->onResizeWindow(NULL);
	
	return true;
}

void DestroyWindow()
{
	glfwTerminate();
}

bool SwapBuffers()
{
	glfwSwapBuffers();
	if(!glfwGetWindowParam(GLFW_OPENED))
		Running = false;
	return Running;
}

float Time()
{
	return glfwGetTime();
}

float TimeDelta()
{
	return TimeDelta_;
}

int WindowWidth()
{
	return WindowWidth_;
}

int WindowHeight()
{
	return WindowHeight_;
}

float WindowAspect()
{
	return float(WindowWidth_)/float(WindowHeight_);
}

int MouseX()
{
	return MouseX_;
}

int MouseY()
{
	return MouseY_;
}

int KeyState( int key )
{
	return glfwGetKey(key);
}