#ifndef JSON_H
#define JSON_H

#define JSON_PARSER_STACK_SIZE 128

enum
{
	JSON_E_END = -1,
	JSON_E_NONE = 0,
	JSON_E_SYNTAX = 1,
	JSON_E_STACK = 2,
	JSON_E_TOKEN = 3,
};

enum
{
	JSON_T_NONE = 0,
	JSON_T_ARRAY_BEGIN,
	JSON_T_ARRAY_END,
	JSON_T_OBJECT_BEGIN,
	JSON_T_OBJECT_END,
	JSON_T_KEY,
	JSON_T_STRING,
	JSON_T_INTEGER,
	JSON_T_FLOAT,
	JSON_T_TRUE,
	JSON_T_FALSE,
	JSON_T_NULL
};

typedef union tagJsonToken
{
	long int int_v;
	double float_v;
	struct
	{
		const char* v;
		unsigned int l;
	} str;
} tJsonToken;

typedef int (*fnJsonReadCb)( void* Context, int Type, const tJsonToken* Data );
typedef int (*fnJsonWriteCb)( void* Context, char Ch );


typedef struct tagJsonReader
{
	char* buffer;
	int bufferCapacity;
	int bufferSize;
	
	fnJsonReadCb callback;
	void* context;
	int strict;
	
	int pos;
	int lineCount;
	int lastLinePos;

	int expected;
	int inString;
	int isQuotedString;
	
	char stack[JSON_PARSER_STACK_SIZE];
	int stackPos;
} tJsonReader;

void JsonCreateReader( tJsonReader* Reader );
void JsonDeleteReader( tJsonReader* Reader );
int JsonRead( tJsonReader* P, char Ch );
void JsonGetPos( tJsonReader* P, int* ELine, int* EColumn );


typedef struct tagJsonWriter
{
	fnJsonWriteCb callback;
	void* context;

	int compact;
	int strict;
	const char* tab;
	
	char stack[JSON_PARSER_STACK_SIZE];
	char stackComma[JSON_PARSER_STACK_SIZE];
	int stackPos;
	int expected;
	char lastCh;
} tJsonWriter;

void JsonCreateWriter( tJsonWriter* Writer );
void JsonDeleteWriter( tJsonWriter* Writer );
int JsonWrite( tJsonWriter* P, int Type, const tJsonToken* Data );

#endif