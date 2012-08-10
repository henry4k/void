#include <kj/Common.h>
#include <kj/Serializer.h>



Serializer::Serializer() :
	m_Fail(false),
	m_LittleEndian(true),
	m_EnableDebug(true)
{
}

std::string Serializer::getTrace() const
{
	const int maxLength = 64;
	
	int length;
	std::string info;
	
	if(m_DebugInfo.size() >= maxLength)
	{
		length = maxLength;
		info = "...";
	}
	else
	{
		length = m_DebugInfo.size();
	}
	
	info += m_DebugInfo.substr(m_DebugInfo.size()-length);
	return info;
}

void Serializer::readBlock( const char* name )
{
	if(!name || !m_EnableDebug)
		return;
	
	std::string realName = readString(NULL);
	
	if(!m_DebugInfo.empty())
		m_DebugInfo.push_back('|');
	m_DebugInfo += realName;
	
	if(realName != name)
		Error("Expected block '%s' but got '%s'\n%s", name, realName.c_str(), getTrace().c_str());
}

std::string Serializer::readString( const char* name )
{
	readBlock(name);
	
	UInt16 length = read<UInt16>(NULL);
	
	if(m_Fail)
		return std::string();
	
	std::string str;
	str.resize(length);
	readStaticArray(NULL, &str[0], length);
	return str;
}

void Serializer::writeBlock( const char* name )
{
	if(!name || !m_EnableDebug)
		return;
	
	write(NULL, std::string(name));
}

void Serializer::write( const char* name, Int8 value )
{
	writeBlock(name);
	writeRaw(reinterpret_cast<const char*>(&value), 1 );
}

void Serializer::write( const char* name, UInt8 value )
{
	writeBlock(name);
	writeRaw(reinterpret_cast<const char*>(&value), 1);
}

void Serializer::write( const char* name, const std::string& value )
{
	writeBlock(name);
	write(NULL, UInt16(value.length()));
	writeArray(NULL, value.data(), value.length(), false);
}




void Serializer::readRaw( char* target, int length )
{
	if(onRead(target, length) != length)
		Error("Read fail. (trace: %s)", getTrace().c_str());
}

void Serializer::writeRaw( const char* source, int length )
{
	if(onWrite(source, length) != length)
		Error("Write fail.");
}