#include <string.h>
#include <kj/Config.hpp>


void FreeNode( const ConfigNode* node )
{
	switch(node->nodeType)
	{
		case CNODE_VALUE:
		{
			ConfigValue* n = (ConfigValue*)node;
			if(n->valueType == CVALUE_STRING)
				delete[] n->s;
			delete n;
		} return;
		
		case CNODE_ARRAY:
		{
			ConfigArray* n = (ConfigArray*)node;
			// ...
			delete n;
		} return;
		
		case CNODE_TABLE:
		{
			ConfigTable* n = (ConfigTable*)node;
			// ...
			delete n;
		} return;
		
		defualt:
			;
	}
}


void ConfigBool( ConfigValue* node, bool value )
{
	node->node.nodeType = CNODE_VALUE;
	node->valueType = CVALUE_BOOL;
	node->b = value;
}

void ConfigInt( ConfigValue* node, int value )
{
	node->node.nodeType = CNODE_VALUE;
	node->valueType = CVALUE_INT;
	node->i = value;
}

void ConfigFloat( ConfigValue* node, double value )
{
	node->node.nodeType = CNODE_VALUE;
	node->valueType = CVALUE_FLOAT;
	node->f = value;
}

void ConfigString( ConfigValue* node, const char* value )
{
	node->node.nodeType = CNODE_VALUE;
	node->valueType = CVALUE_STRING;
	
	int length = strlen(value);
	char* v = new char[length];
	strcpy(v, value);
	
	node->b = v;
}


	int size;
	int capacity;
	ConfigNode* values;

void CreateConfigArray( ConfigArray* node, int capacity )
{
	node->node.nodeType = CNODE_ARRAY;
	node->capacity = capacity;
	node->size = 0;
	
	if(capacity)
	{
		node->values = new ConfigNode*[capacity];
	}
	else
	{
		node->values = 0;
	}
}

void* CppRealloc( void* data, const size_t oldSize, const size_t newSize )
{
	char* buf = new char[newSize];
	size_t copyAmount;
	if(newSize > oldSize)
		copyAmount = oldSize;
	else
		copyAmount = newSize;
	memcpy(buf, data, copyAmount);
	delete[] (char*)data;
	return buf;
}

void ConfigArrayResize( ConfigArray* node, int size )
{
	if(size > node->capacity)
	{
		node->values = CppRealloc(node->values, node->size*sizeof(ConfigNode*), size*sizeof(ConfigNode*));
		node->capacity = size;
	}
	
	node->size = size;
}

ConfigNode* ConfigArrayGetIndex( const ConfigArray* node, int index )
{
	if(index >= node->size)
		return 0;
	return node->values[index];
}

void ConfigArraySetIndex( ConfigArray* node, int index, ConfigNode* value )
{
	node->
}

void CreateConfigTable( ConfigTable* node );
ConfigNode* ConfigTableGetSlot( const ConfigTable* table, const char* slot );
void        ConfigTableSetSlot(       ConfigTable* table, const char* slot, ConfigNode* value );

ConfigNode* LoadJson( const char* fileName );
bool        SaveJson( const char* fileName, const ConfigNode* node );