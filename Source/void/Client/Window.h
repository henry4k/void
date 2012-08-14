#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <tools4k/Vector.h>
using tools4k::vec2i;


class Window;

class IWindowEventListener
{
	public:
		virtual void onResizeWindow( Window* wnd ) = 0;
		virtual void onMouseMove( Window* wnd ) = 0;
		virtual void onKeyAction( Window* wnd, int key, int action ) = 0;
};

class Window
{
	public:
		Window( IWindowEventListener* eventListener, vec2i size );
		virtual ~Window();
		
		void setName( const char* name );
		
		vec2i size() const;
		float aspect() const;
		
		vec2i mousePos() const;
		
		int keyState( int key ) const;
		
		void swapBuffers() const;
		
		bool isOpen() const;
		
	private:
		IWindowEventListener* m_EventListener;
		vec2i m_Size;
		vec2i m_MousePos;
		
		static Window* s_Singleton;
		static void OnResize( int width, int height );
		static void OnKeyAction( int key, int action );
		static void OnMouseMove( int x, int y );
};

#endif