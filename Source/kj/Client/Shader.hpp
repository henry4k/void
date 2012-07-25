#ifndef SHADER_HPP
#define SHADER_HPP

#include <map>
#include <string>

#include "../Common.hpp"
#include "Texture.hpp"



class Shader
{
	public:
		Shader();
		~Shader();
		
		bool load( const char* vert, const char* frag );
		void bind() const;
		
		void setUniform( const char* name, int value ) const;
		void setUniform( const char* name, float value ) const;
		void setUniform( const char* name, int length, const float* value ) const;
		
	protected:
		void clear();
		void updateUniformLocations();
		int getUniformLocation( const char* uniformName ) const;
		
		Handle m_Name;
		std::map<std::string, int> m_UniformLocations;
};

#endif