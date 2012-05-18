#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "Common.hpp"


struct Image
{
	int height, width;
	int bpp;
	int format, type;
	char* data;
};

bool LoadImage( Image* image, const char* file );
void FreeImage( const Image* image );


enum TextureOptions
{
	TEX_MIPMAP,
	TEX_FILTER
};

struct Texture
{
	Handle name;
	GLenum type;
};

bool CreateTexture2d( Texture* texture, int options, const Image* image );
bool LoadTexture2d( Texture* texture, int options, const char* file );

bool CreateTextureCube( Texture* texture, int options, const Image* images );
bool LoadTextureCube( Texture* texture, int options, const char* path, const char* extension ); // "Foo/Right.png" usw.

void BindTexture( const Texture* texture, int layer );
void FreeTexture( const Texture* texture );


#endif