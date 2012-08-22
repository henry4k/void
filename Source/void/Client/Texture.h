#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <tools4k/Vector.h>
using tools4k::vec2i;

#include <void/Common.h>
#include <void/Resource.h>
#include <void/Client/OpenGL.h>

class Image
{
	public:
		Image();
		~Image();
		
		bool load( const char* file );
		
		vec2i size() const { return m_Size; }
		int bpp() const { return m_Bpp; }
		int format() const { return m_Format; }
		int type() const { return m_Type; }
		const char* data() const { return m_Data; }
		
	protected:
		vec2i m_Size;
		int m_Bpp;
		int m_Format, m_Type;
		char* m_Data;
		
		bool loadPngImage( const char* file );
};


enum TextureOptions
{
	TEX_MIPMAP = (1 << 0),
	TEX_FILTER = (1 << 1),
	TEX_DEFAULT = 0
};

class Texture
{
	public:
		virtual ~Texture();
		
		
		void bind( int layer );
		
		Handle name() const { return m_Name; }
		virtual GLenum type() const = 0;
		
	protected:
		Texture();
		void clear();
		
		Handle m_Name;
};

class Texture2d : public Texture
{
	public:
		Texture2d();
		
		/**
		 * Creates a new 2d texture using the given image instance
		 */
		bool create( const Image* image, int options = TEX_DEFAULT );
		
		/**
		 * Load a 2d texture from a file
		 */
		bool load( const char* file, int options = TEX_DEFAULT );
		
		vec2i size() const { return m_Size; }
		
		GLenum type() const { return GL_TEXTURE_2D; }
		
	protected:
		vec2i m_Size;
};

class TextureCubeMap : public Texture
{
	public:
		TextureCubeMap();
		
		/** 
		 * Creates a new cubemap using an array of 6 images
		 * TODO: Explain how the images must be ordered
		 */
		bool create( const Image* images, int options = TEX_DEFAULT );
		
		/**
		 * Load a cubemap from a folder
		 * Right, Left, Bottom, Top, Front, Back
		 */
		bool load( const char* path, const char* extension, int options = TEX_DEFAULT );
		
		GLenum type() const { return GL_TEXTURE_CUBE_MAP; }
};

class Texture2dFile : public Resource, public Texture2d
{
	public:
		bool onLoad()
		{
			return load(filePath().c_str());
		}
};

#endif
