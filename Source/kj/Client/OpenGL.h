#ifndef OPENGL_HPP
#define OPENGL_HPP

#if defined(__APPLE__)
#	include <OpenGL/glext.h>
#	include <OpenGL/glfw.h>
#	include <OpenGL/glu.h>
#else
#	include <GL/glew.h>
#	include <GL/glfw.h>
#	include <GL/glu.h>
#endif

void CheckGl_( const char* file, int line );
#define CheckGl() CheckGl_(__FILE__, __LINE__)

#endif