#include <stdio.h>
#include <string>
#include <fstream>

#include <kj/Common.h>
#include <kj/Client/OpenGL.h>
#include <kj/Client/Model.h>
#include <kj/Client/Shader.h>



/// ---- Utils ----

std::string StringFromFile( const char* path )
{
	std::ifstream f(path);
	std::string r;
	
	while( f.good() )
	{
		std::string line;
		std::getline(f, line);
		r.append(line + "\n");
	}
	
	return r;
}

void ShowShaderLog( Handle handle )
{
	GLint length = 0;
	glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &length);
	
	char* log = NULL;
	if(length)
	{
		log = new char[length];
		glGetShaderInfoLog(handle, length, NULL, log);
	}
	
	if(log)
	{
		Log("%s", log);
		delete[] log;
	}
}

void ShowProgramLog( Handle handle )
{
	GLint length = 0;
	glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &length);

	char* log = NULL;
	if(length)
	{
		log = new char[length];
		glGetProgramInfoLog(handle, length, NULL, log);
	}
	
	if(log)
	{
		Log("%s", log);
		delete[] log;
	}
}

Handle CreateShaderObject( const char* file, int type )
{
	Handle handle = glCreateShader(type);
	
	std::string source = StringFromFile(file);
	if(source.empty())
		return 0;
	
	const char* shaderSource = source.c_str();
	int shaderLength = source.length();
	glShaderSource(handle, 1, &shaderSource, &shaderLength);
	
	glCompileShader(handle);
	
	
	GLint state;
	glGetShaderiv(handle, GL_COMPILE_STATUS, &state);
	ShowShaderLog(handle);
	if(state)
		Log("Compiled shader object '%s' successfully", file);
	else
		Log("Error compiling shader object '%s'", file);
	
	if(!state)
		return 0;
	
	return handle;
}


/// ---- Shader ----

Shader::Shader() :
	m_Name(-1)
{
}

Shader::~Shader()
{
	clear();
}

void Shader::clear()
{
	if(m_Name != -1)
	{
		glDeleteProgram(m_Name);
		m_Name = -1;
	}
}

void Shader::updateUniformLocations()
{
	m_UniformLocations.clear();
	
	int uniformCount = -1;
	glGetProgramiv(m_Name, GL_ACTIVE_UNIFORMS, &uniformCount); 
	
	for(int i = 0; i < uniformCount; ++i)
	{
		int nameLength = -1;
		char name[100];
		
		int size = -1;
		GLenum type = GL_ZERO;
		
		glGetActiveUniform(m_Name, GLuint(i), sizeof(name)-1, &nameLength, &size, &type, name);
		
		name[nameLength] = '\0';
		GLuint location = glGetUniformLocation(m_Name, name);
		
		m_UniformLocations[name] = location;
	}
}

bool Shader::load( const char* vert, const char* frag )
{
	clear();
	
	Handle vertObject = CreateShaderObject(vert, GL_VERTEX_SHADER);
	if(!vertObject)
		return false;
	
	Handle fragObject = CreateShaderObject(frag, GL_FRAGMENT_SHADER);
	if(!fragObject)
		return false;
	
	Handle program = glCreateProgram();
	m_Name = program;
	glAttachShader(program, vertObject);
	glAttachShader(program, fragObject);
	BindVertexAttributes(this);
	
	glLinkProgram(program);
	{
		GLint state;
		glGetProgramiv(program, GL_LINK_STATUS, &state);
		ShowProgramLog(program);
		if(state)
			Log("Linked shader program successfully (vert = '%s'  frag = '%s')", vert, frag);
		else
			Error("Error linking shader program (vert = '%s'  frag = '%s')", vert, frag);
		
		if(!state)
			return false;
	}
	
	glValidateProgram(program);
	{
		GLint state;
		glGetProgramiv(program, GL_VALIDATE_STATUS, &state);
		ShowProgramLog(program);
		if(state)
			Log("Validated shader program successfully (vert = '%s'  frag = '%s')", vert, frag);
		else
			Log("Error validating shader program (vert = '%s'  frag = '%s')", vert, frag);
		
// 		if(!state)
// 			return false;
	}
	
	updateUniformLocations();
	
	CheckGl();
	
	return true;
}

void Shader::bind() const
{
	glUseProgram(m_Name);
}

int Shader::getUniformLocation( const char* uniformName ) const
{
	std::map<std::string,int>::const_iterator i = m_UniformLocations.find(uniformName);
	if(i != m_UniformLocations.end())
		return i->second;
	else
		return -1;
}

void Shader::setUniform( const char* name, int value ) const
{
	bind();
	int location = getUniformLocation(name);
	if(location != -1)
		glUniform1i(location, value);
}

void Shader::setUniform( const char* name, float value ) const
{
	bind();
	int location = getUniformLocation(name);
	if(location != -1)
		glUniform1f(location, value);
}

void Shader::setUniform( const char* name, int length, const float* values ) const
{
	bind();
	int location = getUniformLocation(name);
	if(location == -1)
		return;
	
	switch(length)
	{
		case 1: glUniform1fv(location, 1, values); break;
		case 2: glUniform2fv(location, 1, values); break;
		case 3: glUniform3fv(location, 1, values); break;
		case 4: glUniform4fv(location, 1, values); break;
		default: ;
	}
}