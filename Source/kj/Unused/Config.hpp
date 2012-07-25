#ifndef CONFIG_HPP
#define CONFIG_HPP

enum ConfigNodeType
{
	CNODE_VALUE = 0,
	CNODE_ARRAY,
	CNODE_TABLE
};

struct ConfigNode
{
	int nodeType;
};


enum ConfigValueType
{
	CVALUE_BOOL = 0,
	CVALUE_INT,
	CVALUE_FLOAT,
	CVALUE_STRING
};

struct ConfigValue
{
	ConfigNode node;
	
	int valueType;
	union
	{
		bool b;
		int i;
		double f;
		const char* s;
	} value;
};

struct ConfigArray
{
	ConfigNode node;
	
	int capacity;
	int size;
	ConfigNode** values;
};


void FreeNode( const ConfigNode* node );

void ConfigBool( ConfigValue* node, bool value );
void ConfigInt( ConfigValue* node, int value );
void ConfigFloat( ConfigValue* node, double value );
void ConfigString( ConfigValue* node, const char* value );

void CreateConfigArray( ConfigArray* node, int capacity = 0 );
ConfigNode* ConfigArrayGetIndex( const ConfigArray* array, int index );
void        ConfigArraySetIndex(       ConfigArray* array, int index, ConfigNode* value );

void CreateConfigTable( ConfigTable* node );
ConfigNode* ConfigTableGetSlot( const ConfigTable* table, const char* slot );
void        ConfigTableSetSlot(       ConfigTable* table, const char* slot, ConfigNode* value );

ConfigNode* LoadJson( const char* fileName );
bool        SaveJson( const char* fileName, const ConfigNode* node );

#endif