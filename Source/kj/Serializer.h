#ifndef __SERIALIZER_H__
#define __SERIALIZER_H__

#include <string>
#include <vector>

#include <tools4k/Byteorder.h>
#include <tools4k/Types.h>

using tools4k::Int8;
using tools4k::UInt8;
using tools4k::Int16;
using tools4k::UInt16;
using tools4k::Int32;
using tools4k::UInt32;
using tools4k::Int64;
using tools4k::UInt64;


/// Write binary data to a arbitrary destination
class Serializer
{
	public:
		Serializer();
		
		bool good() const;
		
		
		
		/// ---- Input ----
		
		void readBlock( const char* name );
		
		std::string readString( const char* name );
		
		template< class T >
		T read( const char* name )
		{
			readBlock(name);
			char a[sizeof(T)];
			readRaw(a, sizeof(T));
			
			if(m_LittleEndian)
				tools4k::LittleEndian(a);
			else
				tools4k::BigEndian(a);
			return *reinterpret_cast<T*>(a);
		}
		
// 		template<>
// 		std::string read<std::string>( const char* name )
// 		{
// 			return readString(name);
// 		}
		
		template< class T >
		void readStaticArray( const char* name, T* data, int length )
		{
			readBlock(name);
			for(int i = 0; i < length; i++)
				data[i] = read<T>(NULL);
		}
		
		template< class T >
		std::vector<T> readDynamicArray( const char* name )
		{
			readBlock(name);
			std::vector<T> data;
			data.resize( read<UInt16>(NULL) );
			for(int i = 0; i < data.size(); i++)
				data[i] = read<T>(NULL);
			return data;
		}
		
		std::string getTrace() const;
		
		
		
		/// ---- Output ----
		
		void writeBlock( const char* name );
		
		void write( const char* name, Int8 value );
		void write( const char* name, UInt8 value );
		
		void write( const char* name, const std::string& value );
		void write( const char* name, const char* value ) { write(name, std::string(value)); }
		
		template< class T >
		void write( const char* name, T value )
		{
			writeBlock(name);
			if(m_LittleEndian)
				tools4k::LittleEndian(value);
			else
				tools4k::BigEndian(value);
			writeRaw(reinterpret_cast<const char*>(&value), sizeof(value));
		}
		
		template< class T, class DataType >
		void writeArray( const char* name, const T* data, int length, const bool dynamicLength = false )
		{
			writeBlock(name);
			if(dynamicLength)
				write(NULL, UInt16(length));
			for(int i = 0; i < length; i++)
				write(NULL, DataType(data[i]));
		}
		
		template< class T >
		void writeArray( const char* name, const T* data, int length, const bool dynamicLength = false )
		{
			writeBlock(name);
			if(dynamicLength)
				write(NULL, UInt16(length));
			for(int i = 0; i < length; i++)
				write(NULL, T(data[i]));
		}
		
		void writeArray( const char* name, const char* data, int length, const bool dynamicLength = false )
		{
			writeBlock(name);
			if(dynamicLength)
				write(NULL, UInt16(length));
			writeRaw(data, length);
		}
		
	protected:
		void readRaw( char* target, int length );
		void writeRaw( const char* source, int length );
		
		virtual int onRead( char* target, int length ) { return 0; };
		virtual int onWrite( const char* source, int length ) { return 0; };
		
		bool m_Fail;
		bool m_EnableDebug;
		std::string m_DebugInfo;
		bool m_LittleEndian;
};

#endif