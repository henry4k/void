#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <void/Common.h>
#include <void/Resource.h>
#include <void/Client/OpenGL.h>

class Image
{
	public:
		Image();
		~Image();
		
		bool load( const char* file );
		
		int height() const { return m_Height; }
		int width() const { return m_Width; }
		int bpp() const { return m_Bpp; }
		int format() const { return m_Format; }
		int type() const { return m_Type; }
		const char* data() const { return m_Data; }
		
	protected:
		int m_Height, m_Width;
		int m_Bpp;
		int m_Format, m_Type;
		char* m_Data;
		
		bool loadPngImage( const char* file );
};


enum TextureOptions
{
	TEX_MIPMAP = (1 << 0),
	TEX_FILTER = (1 << 1)
};

class Texture
{
	public:
		Texture();
		~Texture();
		
		
		/** Creates a new 2d texture using the given image instance
		**/
		bool createTexture2d( int options, const Image* image );
		
		/** Load a 2d texture from a file
		**/
		bool loadTexture2d( int options, const char* file );
		
		
		/** Creates a new cubemap using an array of 6 images
			TODO: Explain how the images must be ordered
		**/
		bool createCubeMap( int options, const Image* images );
		
		/** Load a cubemap from a folder
			Right, Left, Bottom, Top, Front, Back
		**/
		bool loadCubeMap( int options, const char* path, const char* extension );
		
		
		void bind( int layer );
		
		Handle name() const { return m_Name; }
		GLenum type() const { return m_Type; }
		
	protected:
		void clear();
		
		Handle m_Name;
		GLenum m_Type;
};

class TextureFile : public Resource, public Texture
{
	public:
		bool load( const char* path )
		{
			return loadTexture2d(/*TEX_MIPMAP|TEX_FILTER*/ 0, path);
		}
};

#endif