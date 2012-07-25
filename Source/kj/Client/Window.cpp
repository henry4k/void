#include "../Common.hpp"
#include "OpenGL.hpp"
#include "Model.hpp"
#include "Window.hpp"


bool Running = true;
int WindowWidth_ = 800;
int WindowHeight_ = 600;
int MouseX_ = 0;
int MouseY_ = 0;
float TimeDelta_;
ResizeFn   WindowResizeCb   = NULL;
RenderFn   WindowRenderCb   = NULL;
KeyboardFn WindowKeyboardCb = NULL;
MouseFn    WindowMouseCb    = NULL;


void WindowResize( int width, int height )
{
	if((width == WindowWidth_) && (height && WindowHeight_))
		return;
	WindowWidth_ = width;
	WindowHeight_ = height;
	if(WindowResizeCb)
		WindowResizeCb();
}

void WindowKeyAction( int key, int action )
{
	if(action && (key == GLFW_KEY_ESC))
		Running = false;
	if(WindowKeyboardCb)
		WindowKeyboardCb(key, action);
}

void WindowMouseAction( int x, int y )
{
	MouseX_ = x;
	MouseY_ = y;
	if(WindowMouseCb)
		WindowMouseCb();
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

bool CreateWindow( const char* name, ResizeFn resizeFn, RenderFn renderFn, KeyboardFn keyboardFn, MouseFn mouseFn )
{
	if(!glfwInit())
	{
		Error("Failed to initialize GLFW");
		return false;
	}
	
	if(!glfwOpenWindow(WindowWidth_, WindowHeight_, 8, 8, 8, 8, 24, 0, GLFW_WINDOW))
	{
		Error("Failed to open GLFW window");
		FreeWindow();
		return false;
	}
	
	if(!renderFn)
	{
		Error("No render function supplied");
		FreeWindow();
		return false;
	}
	
	WindowRenderCb   = renderFn;
	WindowResizeCb   = resizeFn;
	WindowKeyboardCb = keyboardFn;
	WindowMouseCb    = mouseFn;
	
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
		FreeWindow();
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
	
	if(WindowResizeCb)
		WindowResizeCb();
	
	return true;
}

void FreeWindow()
{
	glfwTerminate();
}

void RunGameLoop()
{
	double lastTime = glfwGetTime();
	while(Running)
	{
		double curTime = glfwGetTime();
		TimeDelta_ = curTime - lastTime;
		lastTime = curTime;
		
		WindowRenderCb();
		
		glfwSwapBuffers();
		
		if(!glfwGetWindowParam(GLFW_OPENED))
			Running = false;
	}
}

void StopGameLoop()
{
	Running = false;
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