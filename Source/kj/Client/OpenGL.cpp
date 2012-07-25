#include <kj/Common.hpp>
#include <kj/Client/OpenGL.hpp>


void CheckGl_( const char* file, int line )
{
	GLenum e = glGetError();
	if(e != GL_NO_ERROR)
		Error_("%s @ %d : %s", file, line, (const char*)gluErrorString(e));
}


#if defined(GLEW_STATIC)
#include <GL/glew.c>
#endif