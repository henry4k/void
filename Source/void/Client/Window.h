#ifndef WINDOW_HPP
#define WINDOW_HPP

class Window
{
	public:
		// ...
};

class IWindowEventListener
{
	public:
		virtual void onResizeWindow( Window* wnd ) = 0;
		virtual void onMouseMove( Window* wnd ) = 0;
		virtual void onKeyAction( Window* wnd, int key, int action ) = 0;
};

bool CreateWindow( const char* name, IWindowEventListener* eventListener );
void DestroyWindow();

bool SwapBuffers();

float Time();
float TimeDelta();

int WindowWidth();
int WindowHeight();
float WindowAspect();

int MouseX();
int MouseY();

int KeyState( int key );

#endif