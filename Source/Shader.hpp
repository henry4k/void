#ifndef SHADER_HPP
#define SHADER_HPP

#include "Common.hpp"
#include "Texture.hpp"

Handle LoadShader( const char* vert, const char* frag );
void FreeShader( Handle handle );
void BindShader( Handle handle );

void SetUniform( Handle shader, const char* name, int value );
void SetUniform( Handle shader, const char* name, float value );
void SetUniform( Handle shader, const char* name, int length, float* value );


struct Material
{
	Handle shader;
	const Texture* textures;
	int textureCount;
};

void BindMaterial( const Material* material );

#endif