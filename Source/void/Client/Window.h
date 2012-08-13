#ifndef WINDOW_HPP
#define WINDOW_HPP

typedef void (*ResizeFn)();
typedef void (*RenderFn)();
typedef void (*KeyboardFn)( int key, int action );
typedef void (*MouseFn)();

bool CreateWindow( const char* name, ResizeFn resizeFn, RenderFn renderFn, KeyboardFn keyboardFn, MouseFn mouseFn );
void FreeWindow();
void RunGameLoop();
void StopGameLoop();

float Time();
float TimeDelta();

int WindowWidth();
int WindowHeight();
float WindowAspect();

int MouseX();
int MouseY();

int KeyState( int key );

#endif