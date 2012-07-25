#include <stdio.h>

#include <kj/Common.h>
#include <kj/Client/OpenGL.h>
#include <kj/Client/Model.h>
#include <kj/Client/Shader.h>



/// ---- Utils ----

char* LoadFile( const char* path, int* sizeOut )
{
	FILE* f = fopen(path, "r");
	if(!f)
	{
		Log("Can't open file '%s'", path);
		return 0;
	}
	
	fseek(f, 0, SEEK_END);
	int size = ftell(f);
	fseek(f, 0, SEEK_SET);
	
	char* b = new char[size];
	if(fread(b, 1, size, f) != size)
	{
		Error("Can't read file '%s'", path);
		delete[] b;
		fclose(f);
		return 0;
	}
	fclose(f);
	
	*sizeOut = size;
	return b;
}

void FreeFile( const char* fileData )
{
	delete[] fileData;
}

void ShowShaderLog( Handle handle )
{
	GLint length = 0;
	glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &length);
	
	char* log = 0;
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

	char* log = 0;
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
	
	int size;
	const char* source = LoadFile(file, &size);
	if(!source)
		return 0;
	
	glShaderSource(handle, 1, &source, &size);
	FreeFile(source);
	
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
	
	m_Name = program;
	
	updateUniformLocations();
	
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
	int location = getUniformLocation(name);
	if(location != -1)
		glUniform1i(location, value);
}

void Shader::setUniform( const char* name, float value ) const
{
	int location = getUniformLocation(name);
	if(location != -1)
		glUniform1f(location, value);
}

void Shader::setUniform( const char* name, int length, const float* values ) const
{
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