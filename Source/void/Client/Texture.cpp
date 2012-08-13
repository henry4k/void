#include <stdio.h>
#include <string.h>
#include <png.h>
#include <string>

#include <void/Common.h>
#include <void/Client/OpenGL.h>
#include <void/Client/Texture.h>


/// ---- Image ----

bool Image::loadPngImage( const char* file )
{
	//header for testing if it is a png
	png_byte header[8];

	//open file as binary
	FILE* fp = fopen(file, "rb");
	if(!fp)
		return false;

	//read the header
	size_t read = fread(header, 1, 8, fp);
	if(read != 8 && ferror(fp))
		return false;
	
	//test if png
	if(png_sig_cmp(header, 0, 8) != 0)
	{
		fclose(fp);
		return false;
	}

	//create png struct
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!png_ptr)
	{
		fclose(fp);
		return false;
	}

	//create png info struct
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if(!info_ptr)
	{
		png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
		fclose(fp);
		return false;
	}

	//create png info struct
	png_infop end_info = png_create_info_struct(png_ptr);
	if(!end_info)
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
		fclose(fp);
		return false;
	}

	//png error stuff, not sure libpng man suggests this.
	if(setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		fclose(fp);
		return false;
	}

	//init png reading
	png_init_io(png_ptr, fp);

	//let libpng know you already read the first 8 bytes
	png_set_sig_bytes(png_ptr, 8);

	// read all the info up to the image data
	png_read_info(png_ptr, info_ptr);

	//variables to pass to get info
	int bit_depth, color_type;
	png_uint_32 twidth, theight;

	// Only 8-bit!
	png_set_strip_16(png_ptr);

	// OGL doesn't likes indexed data. (Extensions?)
	png_set_expand(png_ptr);

	// get info about png
	png_get_IHDR(png_ptr, info_ptr, &twidth, &theight, &bit_depth, &color_type, NULL, NULL, NULL);

	//update width and height based on png info
	m_Width = int(twidth);
	m_Height = int(theight);

	switch(color_type)
	{
		case PNG_COLOR_TYPE_GRAY:       m_Bpp = 1; break;
		case PNG_COLOR_TYPE_GRAY_ALPHA: m_Bpp = 2; break;
		case PNG_COLOR_TYPE_RGB:        m_Bpp = 3; break;
		case PNG_COLOR_TYPE_RGB_ALPHA:  m_Bpp = 4; break;
		case PNG_COLOR_TYPE_PALETTE:
		{
			m_Bpp = 3;
			int trans = 0;
			png_get_tRNS(png_ptr, info_ptr, NULL, &trans, NULL);
			if(trans > 0)
				m_Bpp += 1;
		} break;
		default:
			m_Bpp = 0;
			Log("Something went wrong; Can't detect bpp");
	}
// 	m_Bpp *= bit_depth / 8;

	// Update the png info struct.
	png_read_update_info(png_ptr, info_ptr);

	// Row size in bytes.
	int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

	// Allocate the image_data as a big block, to be given to opengl
	png_byte* image_data = new png_byte[rowbytes*theight];
	if(!image_data)
	{
		//clean up memory and close stuff
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		fclose(fp);
		return false;
	}

	//row_pointers is for pointing to image_data for reading the png with libpng
	png_bytep* row_pointers = new png_bytep[theight];
	if(!row_pointers)
	{
		//clean up memory and close stuff
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		delete[] image_data;
		fclose(fp);
		return false;
	}

	// set the individual row_pointers to point at the correct offsets of image_data
	for(png_uint_32 i = 0; i < theight; ++i)
		row_pointers[theight - 1 - i] = image_data + i * rowbytes;

	//read the png into image_data through row_pointers
	png_read_image(png_ptr, row_pointers);

	m_Data = reinterpret_cast<char*>(image_data);

	//clean up memory and close stuff
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	delete[] row_pointers;
	fclose(fp);

	return true;
}

Image::Image() :
	m_Width(0), m_Height(0),
	m_Bpp(0),
	m_Format(0), m_Type(0),
	m_Data(NULL)
{
}

Image::~Image()
{
	if(m_Data)
		delete[] m_Data;
}

bool Image::load( const char* file )
{
	// reset
	if(m_Data)
		delete[] m_Data;
	
	if(!loadPngImage(file))
	{
		Log("Can't load '%s'", file);
		return false;
	}
	
	switch(m_Bpp)
	{
		case 1:
			m_Format = GL_LUMINANCE;
			break;
			
		case 2:
			m_Format = GL_LUMINANCE_ALPHA;
			break;
			
		case 3:
			m_Format = GL_RGB;
			break;
			
		case 4:
			m_Format = GL_RGBA;
			break;
			
		default:
			Log("Can't load '%s' (Unknown BPP -> %d)", file, m_Bpp);
			delete[] m_Data;
			return false;
	}
	
	m_Type = GL_UNSIGNED_BYTE;
	
	return true;
}




/// ---- Texture ----

Texture::Texture() :
	m_Name(0),
	m_Type(0)
{
}

Texture::~Texture()
{
	clear();
}

void Texture::clear()
{
	if(m_Name)
	{
		glDeleteTextures(1, &m_Name);
		m_Name = 0;
	}
}

bool Texture::createTexture2d( int options, const Image* image )
{
	clear();
	
	Handle id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	if(options & TEX_MIPMAP)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (options & TEX_FILTER) ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (options & TEX_FILTER) ? GL_LINEAR : GL_NEAREST);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (options & TEX_FILTER) ? GL_LINEAR : GL_NEAREST);
	
	if(options & TEX_MIPMAP)
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	
	glTexImage2D(GL_TEXTURE_2D, 0, image->format(),  image->width(), image->height(), 0, image->format(), image->type(), image->data());
	
	glBindTexture(GL_TEXTURE_2D, 0); // unbind? .. warum?
	CheckGl();
	
	m_Name = id;
	m_Type = GL_TEXTURE_2D;
	return true;
}

bool Texture::loadTexture2d( int options, const char* file )
{
	Image image;
	if(!image.load(file))
		return false;
	return createTexture2d(options, &image);
}

bool Texture::createCubeMap( int options, const Image* images )
{
	clear();
	
	Handle id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	if(options & TEX_MIPMAP)
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, (options & TEX_FILTER) ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST);
	else
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, (options & TEX_FILTER) ? GL_LINEAR : GL_NEAREST);
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, (options & TEX_FILTER) ? GL_LINEAR : GL_NEAREST);
	
	if(options & TEX_MIPMAP)
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP, GL_TRUE);
	
	for(int i = 0; i < 6; i++)
	{
		GLenum target = GL_TEXTURE_CUBE_MAP_POSITIVE_X+i;
		glTexImage2D(target, 0, images[i].format(),  images[i].width(), images[i].height(), 0, images[i].format(), images[i].type(), images[i].data());
	}
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	
	CheckGl();
	
	m_Name = id;
	m_Type = GL_TEXTURE_CUBE_MAP;
	return true;
}

bool Texture::loadCubeMap( int options, const char* path, const char* extension )
{
	Image images[6];
	static const char* names[6] = { "Right","Left","Bottom","Top","Front","Back" };
	
	for(int i = 0; i < 6; i++)
	{
		std::string file = std::string(path)+std::string(names[i])+std::string(extension);
		
		if(!images[i].load(file.c_str()))
			return false;
	}
	return createCubeMap(options, images);
}


static const int MaxTextureLayers = 8;
GLenum CurTextureTargets[MaxTextureLayers];

int InitTextureLayers()
{
	for(int i = 0; i < 8; i++)
		CurTextureTargets[i] = -1;
	return 0;
}

void Texture::bind( int layer )
{
	static int unused = InitTextureLayers();
	glActiveTexture(GL_TEXTURE0+layer);
	if((CurTextureTargets[layer] != -1) && (CurTextureTargets[layer] != m_Type))
	{
		glBindTexture(CurTextureTargets[layer], 0);
		CurTextureTargets[layer] = m_Type;
	}
	glBindTexture(m_Type, m_Name);
}