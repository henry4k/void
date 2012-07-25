#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "Json.h"

#define JSON_BUFFER_STEP_SIZE 128


// DEBUG
#	include <stdio.h>
#	define QUOTEME_(x) #x
#	define QUOTEME(x) QUOTEME_(x)
#	define PRINT_ERROR_LINE() puts(QUOTEME(__FILE__)": ERROR @ "QUOTEME(__LINE__) );

#define ERROR(E,V) if(!( E )) { PRINT_ERROR_LINE() return V; }
#define PUSH(C) ERROR((++(P->stackPos) < JSON_PARSER_STACK_SIZE), JSON_E_STACK) P->stack[P->stackPos] = C ;
#define POP(C) ERROR((P->stack[P->stackPos] == C ), JSON_E_SYNTAX) --(P->stackPos);
#define CALLBACK(...) if(!P->callback(P->context, __VA_ARGS__)) return JSON_E_NONE;
#define CHECK_EXPECTED(C) ERROR(IsExpected( C , P->expected), JSON_E_SYNTAX)

#define GET_MIN(A,B) ( ( ( A ) < ( B ) ) ? ( A ) : ( B ) )

enum
{
	JSON_C_ARRAY,
	JSON_C_OBJECT,
	JSON_C_DATA, // String, Int, Float, Bool, Null
	JSON_C_VALUE, // Data || Array || Object
	JSON_C_KEY
};

int IsWhiteSpace( char Ch )
{
	return ((Ch == ' ') || (Ch == '\t') || (Ch == '\n')) ? (1) : (0);
}

int IsExpected( const int Class, const int Expected )
{
	if(Expected == JSON_C_VALUE)
		return (Class != JSON_C_KEY)?(1):(0);
	return (Expected == Class);
}

int StringOnlyConsistsOf( const char* ValidChars, const char* String, int MaxLength )
{
	int ok;
	const char* j;
	int i = 0;
	for(; *String; ++String)
	{
		if(MaxLength && (i++ >= MaxLength))
			break;
		ok = 0;
		for(j = ValidChars; *j; ++j)
		{
			if(*String == *j)
				ok = 1;
		}
		if(!ok)
			return 0;
	}
	return 1;
}

int StringContainsOneOf( const char* Chars, const char* String, int MaxLength )
{
	const char* j;
	int i = 0;
	for(; *String; ++String)
	{
		if(MaxLength && (i++ >= MaxLength))
			break;
		for(j = Chars; *j; ++j)
		{
			if(*String == *j)
				return 1;
		}
	}
	return 0;
}

int StringContains( const char Char, const char* String, int MaxLength )
{
	int i = 0;
	for(; *String; ++String)
	{
		if(MaxLength && (i++ >= MaxLength))
			break;
		if(*String == Char)
			return 1;
	}
	return 0;
}

int StringContainsFn( int (*fnTest)( char Ch ), const char* String, int MaxLength )
{
	int i = 0;
	for(; *String; ++String)
	{
		if(MaxLength && (i++ >= MaxLength))
			break;
		if(fnTest(*String))
			return 1;
	}
	return 0;
}

void RewindBuffer( tJsonReader* P )
{
	P->bufferSize = 0;
}

void AppendBuffer( tJsonReader* P, const char* String, int Length )
{
	// Sicherstellen, dass genügend Platz vorhanden ist.
	if((P->bufferSize+Length) > P->bufferCapacity)
	{
		int needed = P->bufferSize+Length;
		int new_size = needed + ( JSON_BUFFER_STEP_SIZE - ( needed % JSON_BUFFER_STEP_SIZE ) );
		P->bufferCapacity = new_size;
		realloc(P->buffer, new_size);
	}
	
	// Kopieren
	memcpy(P->buffer+P->bufferSize, String, Length);
	P->bufferSize += Length;
}

int ProcessString( tJsonReader* P )
{
	AppendBuffer(P, "", 1);
	--P->bufferSize; // Ist ja nur die '\0' !

	int type = JSON_T_STRING;

	switch(P->expected)
	{
		case JSON_C_KEY:
			type = JSON_T_KEY;
			break;
		
		// case JSON_C_VALUE:
		default:

			if(!P->isQuotedString)
			{
				// true
				if( strncmp(P->buffer, "true", GET_MIN(P->bufferSize, (sizeof("true")-1)) ) == 0 )
					type = JSON_T_TRUE;
				
				// false
				else if( strncmp(P->buffer, "false", GET_MIN(P->bufferSize, (sizeof("false")-1)) ) == 0 )
					type = JSON_T_FALSE;
				
				// null
				else if( strncmp(P->buffer, "null", GET_MIN(P->bufferSize, (sizeof("null")-1)) ) == 0 )
					type = JSON_T_NULL;
					
				// number
				else if( StringOnlyConsistsOf("0123456789.Ee+-", P->buffer, P->bufferSize) )
				{
					// integer
					if( !StringContainsOneOf(".Ee+-", P->buffer, P->bufferSize) )
						type = JSON_T_INTEGER;
					
					// float
					else
						type = JSON_T_FLOAT;
				}
				else if(P->strict)
				{
					RewindBuffer(P);
					ERROR(0, JSON_E_SYNTAX) // Unquoted Strings sind bei striktem Syntax nicht erlaubt!
				}
				
				// else
					// type = JSON_T_STRING; // siehe Initialisierung von 'type'
			
			}
			// else
				// type = JSON_T_STRING; // siehe Initialisierung von 'type'
	}
	
	tJsonToken data;

	switch(type)
	{
		case JSON_T_KEY:
		case JSON_T_STRING:
			data.str.v = P->buffer;
			data.str.l = P->bufferSize;
			CALLBACK(type, &data)
			break;

		case JSON_T_INTEGER:
			data.int_v = strtol(P->buffer, 0, 10);
			CALLBACK(type, &data)
			break;
		
		case JSON_T_FLOAT:
			data.float_v = strtod(P->buffer, 0);
			CALLBACK(type, &data)
			break;

		// case JSON_T_TRUE:
		// case JSON_T_FALSE:
		// case JSON_T_NULL:
		default:
			CALLBACK(type, 0)
	}
	
	RewindBuffer(P);

	return JSON_E_END;
}

int ReadString( char Ch, tJsonReader* P )
{
	// Quoted string:
	if(P->isQuotedString)
	{
		if(Ch == '"') // Quote => Abbruch
		{
			if((!P->strict) && (P->bufferSize) && (P->buffer[P->bufferSize-1] == '\n')) // \n" => "
				--P->bufferSize;
		
			int r = ProcessString(P);

			if(r == JSON_E_END)  // Weil das Quote jetzt übersprungen werden muss.
			{
				P->inString = 0; // Leicht hackig. :X
				return JSON_E_NONE;
			}
			else
				return r;
		}

		if(Ch == '\n')
		{
			ERROR((!P->strict), JSON_E_SYNTAX) // Newline im strikten Modus nicht erlaubt. '\n' => "\n"
			
			if(!P->strict && (P->bufferSize == 0)) // "\n => "
				return JSON_E_NONE; // Ignorieren.
		}
		
		if((P->bufferSize) && (P->buffer[P->bufferSize-1] == '\\')) // Als letztes wurde ein Escape-Zeichen eingelesen?
		{
			switch( Ch )
			{
				case '"':
					P->buffer[P->bufferSize] = '"';
					return JSON_E_NONE;
				
				case '\\':
					// P->buffer[P->bufferSize-1] = '\\';
					return JSON_E_NONE;
				
				case '/':
					P->buffer[P->bufferSize-1] = '/'; // Dunno wozu das wichtig ist.
					return JSON_E_NONE;
				
				case 't':
					P->buffer[P->bufferSize-1] = '\t';
					return JSON_E_NONE;
				
				case 'n':
					P->buffer[P->bufferSize-1] = '\n';
					return JSON_E_NONE;
				
				case 'b':
					P->buffer[P->bufferSize-1] = '\b';
					return JSON_E_NONE;
				
				case 'f':
					P->buffer[P->bufferSize-1] = '\f';
					return JSON_E_NONE;
				
				case 'r':
					P->buffer[P->bufferSize-1] = '\r';
					return JSON_E_NONE;
				
				case 'u':
					AppendBuffer(P, &Ch, 1); // \uXXXX wird atm noch nicht unterstützt!
					return JSON_E_NONE;
				
				default:
					ERROR(0, JSON_E_SYNTAX)
			}
		}
	}
	else
	{
		if(IsWhiteSpace(Ch)) // WhiteSpace => Abbruch
			return ProcessString(P);
		
		switch(Ch) // Command char || LineFeed => Abbruch
		{
			case '[':
			case ']':
			case '{':
			case '}':
			case ',':
			case ':':
			case '\n': // Siehe oben!
				return ProcessString(P);
		}
	}
	
	AppendBuffer(P, &Ch, 1);

	return JSON_E_NONE;
}

void JsonCreateReader( tJsonReader* Reader )
{
	Reader->buffer = (char*)malloc(JSON_BUFFER_STEP_SIZE);
	Reader->bufferCapacity = JSON_BUFFER_STEP_SIZE;
	Reader->bufferSize = 0;
	
	Reader->callback = 0;
	Reader->context = 0;
	Reader->strict = 0;

	Reader->pos = 0;
	Reader->lineCount = 1;
	Reader->lastLinePos = 0;

	Reader->expected = JSON_C_VALUE;
	Reader->inString = 0;
	Reader->isQuotedString = 0;

	Reader->stackPos = -1;
}

void JsonDeleteReader( tJsonReader* Reader )
{
	if(Reader && Reader->buffer)
		free(Reader->buffer);
}

int JsonRead( tJsonReader* P, char Ch )
{
	++(P->pos);

	switch(Ch) // Allgemeines Zeug
	{
		case '\r': // Wird böse ignoriert, weil auch Mac ab OSX nur noch LF nutzt.
			// break;
			return JSON_E_NONE;

		case '\n':
			++(P->lineCount);
			P->lastLinePos = P->pos;
			break;
			
		case '\t': // \t ist okay. (Obwohl es ein Steuerzeichen ist. ;P)
			break;
		
		case '\0':
			ERROR((P->stackPos == -1), JSON_E_SYNTAX) // Stack muss leer sein, sonst stimmt was nicht.

			if(P->inString) // Wird atm ein String eingelesen?
			{
				ERROR(!P->isQuotedString, JSON_E_SYNTAX)
				return ProcessString(P);
			}
			return JSON_E_END;
			
		default:
			ERROR(!iscntrl(Ch), JSON_E_SYNTAX) // Don't try to cheat me! Damit könnte man sicher iwas böses anstellen.
	}

	if(P->inString)
	{
		int r = ReadString(Ch, P);
		if( r != JSON_E_END ) // Spezialwert: Bei JSON_E_END wird normal weitergearbeitet. (String ist zuende.)
			return r;
		P->inString = 0;
	}

	switch(Ch) // Command Chars durchgehen
	{
		case '[': // Array begin
			CHECK_EXPECTED(JSON_C_ARRAY)
			PUSH(JSON_C_ARRAY)
			CALLBACK(JSON_T_ARRAY_BEGIN, 0)
			P->expected = JSON_C_VALUE;
			break;

		case ']': // Array end
			POP(JSON_C_ARRAY)
			CALLBACK(JSON_T_ARRAY_END, 0)
			if(P->stackPos == -1)
				return JSON_E_END;
			break;

		case '{': // Object begin
			CHECK_EXPECTED(JSON_C_OBJECT)
			PUSH(JSON_C_OBJECT)
			CALLBACK(JSON_T_OBJECT_BEGIN, 0)
			P->expected = JSON_C_KEY;
			break;

		case '}': // Object end
			POP(JSON_C_OBJECT)
			CALLBACK(JSON_T_OBJECT_END, 0)
			if(P->stackPos == -1)
				return JSON_E_END;
			break;

		case ':': // <Key> : <Value>
			ERROR((P->stack[P->stackPos] == JSON_C_OBJECT), JSON_E_SYNTAX)
			P->expected = JSON_C_VALUE;
			break;

		case ',': // Value separator ( [<Value>,<Value>] || {<Key>:<Value>,<Key>:<Value>} )
			ERROR(( (P->stack[P->stackPos] == JSON_C_ARRAY) || (P->stack[P->stackPos] == JSON_C_OBJECT) ), JSON_E_SYNTAX)
			P->expected = (P->stack[P->stackPos] == JSON_C_ARRAY) ? (JSON_C_VALUE) : (JSON_C_KEY);
			break;

		default:
			if(!IsWhiteSpace(Ch))
			{
				// String-Einlese-Modus aktivieren!
				P->inString = 1;
				
				if(Ch == '"') // Es fängt mit einem Quote an? Gleich verwerten!
				{
					P->isQuotedString = 1;
					return JSON_E_NONE;
				}
				
				P->isQuotedString = 0;
				int r = ReadString(Ch, P);
				if(r != JSON_E_END)
					return r;
				P->inString = 0;
			}
	}

	return JSON_E_NONE;
}

void JsonGetPos( tJsonReader* P, int* ELine, int* EColumn )
{
	*ELine = P->lineCount;
	*EColumn = P->pos - P->lastLinePos;
}


// ----------------------------

#undef PUSH
#undef POP
// #undef CALLBACK
#undef CHECK_EXPECTED

#define PUSH(C) ERROR((++(P->stackPos) < JSON_PARSER_STACK_SIZE), JSON_E_STACK) P->stack[P->stackPos] = C ; P->stackComma[P->stackPos] = 0;
#define POP(C) ERROR((P->stack[P->stackPos] == C ), JSON_E_SYNTAX) --(P->stackPos);
// #define CALLBACK(...) if(!P->callback(P->context, __VA_ARGS__)) return JSON_E_NONE;
#define CHECK_EXPECTED(C) ERROR(IsExpected( C , P->expected), JSON_E_SYNTAX)
#define CALL(F, ...) if(( r = ( F ( __VA_ARGS__ )) ) != JSON_E_NONE) return r;


int WriteChar( tJsonWriter* P, char Ch )
{
	P->lastCh = Ch;
	return (P->callback(P->context, Ch)) ? (JSON_E_NONE) : (JSON_E_END);
}

int WriteRawString( tJsonWriter* P, const char* String, int MaxLength )
{
	int r;
	int i = 0;
	for(; *String; ++String)
	{
		if(MaxLength && (i++ >= MaxLength))
			break;
		CALL(WriteChar, P, *String)
	}
	return JSON_E_NONE;
}

int WriteString( tJsonWriter* P, const char* String, int MaxLength, int NoMultiLine )
{
	int r;
	int quoted = ( P->strict || StringContainsFn(IsWhiteSpace, String, MaxLength) );
	int multiline = ( !P->strict && !P->compact && StringContains('\n', String, MaxLength) );
	
	ERROR(!(multiline && NoMultiLine), JSON_E_SYNTAX)

	if(multiline && (P->lastCh != '\n'))
		CALL(WriteChar, P, '\n')
	if(quoted || multiline)
		CALL(WriteChar, P, '"')
	if(multiline)
		CALL(WriteChar, P, '\n')
	
	// CALL(WriteRawString, P, String, MaxLength)
	
	int i = 0;
	for(; *String; ++String)
	{
		if(MaxLength && (i++ >= MaxLength))
			break;
		
		switch(*String)
		{
			case '"':
				CALL(WriteChar, P, '\\')
				CALL(WriteChar, P, '"')
				break;	

			case '\\':
				CALL(WriteChar, P, '\\')
				CALL(WriteChar, P, '\\')
				break;

			case '\n':
				if(multiline)
				{
					CALL(WriteChar, P, *String);
					break;
				}
				CALL(WriteChar, P, '\\')
				CALL(WriteChar, P, 'n')
				break;
				
			case '\t':
				if(!P->strict && !P->compact)
				{
					CALL(WriteChar, P, *String);
					break;
				}
				CALL(WriteChar, P, '\\')
				CALL(WriteChar, P, 't')
				break;
				
			case '\b':
				CALL(WriteChar, P, '\\')
				CALL(WriteChar, P, 'b')
				break;
				
			case '\f':
				CALL(WriteChar, P, '\\')
				CALL(WriteChar, P, 'f')
				break;
				
			case '\r':
				CALL(WriteChar, P, '\\')
				CALL(WriteChar, P, 'r')
				break;
				
			/*
			case '\u':
				CALL(WriteChar, P, '\\')
				CALL(WriteChar, P, 'u')
				break;
			*/

			default:
				CALL(WriteChar, P, *String)
		}
	}

	if(multiline)
		CALL(WriteChar, P, '\n')
	if(quoted || multiline)
		CALL(WriteChar, P, '"')
	if(multiline)
		CALL(WriteChar, P, '\n')
	
	return JSON_E_NONE;
}

int WriteIndent( tJsonWriter* P )
{
	if(P->compact)
		return JSON_E_NONE;
	int r;
	int d = P->stackPos;
	while(d-- > 0)
		CALL(WriteRawString, P, P->tab, 0);
		// CALL(WriteChar, P, '\t')
	return JSON_E_NONE;
}

int WriteLinefeed( tJsonWriter* P )
{
	if(P->compact || (P->lastCh == '\0'))
		return JSON_E_NONE;
	int r;
	CALL(WriteChar, P, '\n')
	CALL(WriteIndent, P)
	return JSON_E_NONE;
}

int WriteComma( tJsonWriter* P )
{
	int r;
	if(P->stackComma[P->stackPos] != 0)
	{
		CALL(WriteChar, P, ',')
		if(!P->compact)
			CALL(WriteChar, P, ' ')
	}
	else
		P->stackComma[P->stackPos] = 1;
	return JSON_E_NONE;
}

void JsonCreateWriter( tJsonWriter* Writer )
{
	Writer->callback = 0;
	Writer->context = 0;

	Writer->compact = 0;
	Writer->strict = 0;

	Writer->stackPos = 0;
	Writer->stack[0] = -1;
	Writer->expected = JSON_C_VALUE;
	Writer->lastCh = '\0';
}

void JsonDeleteWriter( tJsonWriter* Writer )
{
	// Does nothing at the moment,
	// since we have no dynamically allocated data in this structure.
}

int JsonWrite( tJsonWriter* P, int Type, const tJsonToken* Data )
{
	int r;

	switch(Type)
	{
		case JSON_T_ARRAY_BEGIN:
			CHECK_EXPECTED(JSON_C_ARRAY)
			if(P->stack[P->stackPos] == JSON_C_ARRAY)
				CALL(WriteComma, P)
			CALL(WriteLinefeed, P)
			CALL(WriteChar, P, '[')
			PUSH(JSON_C_ARRAY)
			P->expected = JSON_C_VALUE;
			break;

		case JSON_T_ARRAY_END:
			POP(JSON_C_ARRAY)
			CALL(WriteLinefeed, P)
			CALL(WriteChar, P, ']')
			if(P->stackPos == -1)
				return JSON_E_END;
			if(P->stack[P->stackPos] == JSON_C_OBJECT)
				P->expected = JSON_C_KEY;
			else
				P->expected = JSON_C_VALUE;
			break;

		case JSON_T_OBJECT_BEGIN:
			CHECK_EXPECTED(JSON_C_OBJECT)
			if(P->stack[P->stackPos] == JSON_C_ARRAY)
				CALL(WriteComma, P)
			CALL(WriteLinefeed, P)
			CALL(WriteChar, P, '{')
			PUSH(JSON_C_OBJECT)
			P->expected = JSON_C_KEY;
			break;

		case JSON_T_OBJECT_END:
			POP(JSON_C_OBJECT)
			CALL(WriteLinefeed, P)
			CALL(WriteChar, P, '}')
			if(P->stackPos == -1)
				return JSON_E_END;
			if(P->stack[P->stackPos] == JSON_C_OBJECT)
				P->expected = JSON_C_KEY;
			else
				P->expected = JSON_C_VALUE;
			break;

		case JSON_T_KEY:
			CHECK_EXPECTED(JSON_C_KEY)
			ERROR((P->stack[P->stackPos] == JSON_C_OBJECT), JSON_E_SYNTAX)
			CALL(WriteComma, P)
			CALL(WriteLinefeed, P)
			CALL(WriteString, P, Data->str.v, Data->str.l, 1)
			CALL(WriteChar, P, ':')
			if(!P->compact)
				CALL(WriteChar, P, ' ')
			P->expected = JSON_C_VALUE;
			break;
			
		default:
			CHECK_EXPECTED(JSON_C_DATA)
			if(P->stack[P->stackPos] == JSON_C_ARRAY)
			{
				CALL(WriteComma, P)
				CALL(WriteLinefeed, P)
			}
			
			switch(Type)
			{
				case JSON_T_STRING:
					CALL(WriteString, P, Data->str.v, Data->str.l, 0);
					break;
				
				case JSON_T_INTEGER:
				{
					char buf[64];
					sprintf(buf, "%d", (int)Data->int_v);
					CALL(WriteRawString, P, buf, 0);
				}	break;

				case JSON_T_FLOAT:
				{
					char buf[64];
					sprintf(buf, "%g", Data->float_v);
					CALL(WriteRawString, P, buf, 0);
				}	break;

				case JSON_T_TRUE:
					CALL(WriteRawString, P, "true", 0);
					break;

				case JSON_T_FALSE:
					CALL(WriteRawString, P, "false", 0);
					break;

				case JSON_T_NULL:
					CALL(WriteRawString, P, "null", 0)
					break;

				default:
					return JSON_E_TOKEN;
			}

			if(P->stack[P->stackPos] == JSON_C_OBJECT)
				P->expected = JSON_C_KEY;
			break;
	}
	
	return JSON_E_NONE;
}