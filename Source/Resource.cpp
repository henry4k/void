#include <string.h>
#include <string>
#include <map>

#include "Common.hpp"
#include "Resource.hpp"


struct ResourceType
{
	Handle typeId;
	ResourceAllocFn allocFn;
	ResourceFreeFn freeFn;
	ResourceLoadFn loadFn;
};

struct Resource
{
	void* data;
	char filePath[MaxPathLength];
	int references;
	Handle typeId;
};


std::map<Handle,ResourceType*>  g_ResourceTypes;
std::map<std::string,Resource*> g_ResourcesByPath;
std::map<void*,Resource*>       g_ResourcesByData;

std::string ResourceName( Handle typeId, const char* filePath )
{
	return filePath; // FIXME
}

void RegisterResourceType( Handle typeId, ResourceAllocFn allocFn, ResourceFreeFn freeFn, ResourceLoadFn loadFn )
{
	if(!allocFn || !freeFn || !loadFn)
		Error("Invalid resource structure (id = %d)", typeId);
	
	ResourceType* type = new ResourceType;
	type->typeId = typeId;
	type->allocFn = allocFn;
	type->freeFn = freeFn;
	type->loadFn = loadFn;
	
	g_ResourceTypes[typeId] = type;
}

ResourceType* GetResourceType( Handle typeId )
{
	std::map<Handle,ResourceType*>::const_iterator i = g_ResourceTypes.find(typeId);
	if(i == g_ResourceTypes.end())
		return 0;
	else
		return i->second;
}

Resource* GetResourceByPath( Handle typeId, const char* filePath )
{
	std::map<std::string,Resource*>::const_iterator i = g_ResourcesByPath.find(ResourceName(typeId, filePath));
	if(i == g_ResourcesByPath.end())
		return 0;
	else
		return i->second;
}

Resource* GetResourceByData( void* data )
{
	std::map<void*,Resource*>::const_iterator i = g_ResourcesByData.find(data);
	if(i == g_ResourcesByData.end())
		return 0;
	else
		return i->second;
}

Resource* CreateResource( Handle typeId, const char* filePath, void* data )
{
	Resource* resource = new Resource;
	
	ResourceType type = GetResourceType(typeId);
	resource->data = type->allocFn();
	
	if(!type->loadFn(resource->data, filePath))
	{
		Log("Can't load resource '%s'", filePath);
		type->freeFn(resource->data);
		delete resource;
		return 0;
	}
	
	strncpy(resource->filePath, filePath, MaxPathLength);
	resource->references = 1;
	resource->typeId = typeId;
	
	return resource;
}

void FreeResource( Resource* resource )
{
	ResourceType* type = GetResourceType(resource->typeId);
	type->freeFn(resource->data);
	delete resource;
}

void* LoadResource( Handle typeId, const char* filePath )
{
	Resource* resource = GetResourceByPath(typeId, filePath);
	if(resource)
	{
		resource->references++;
		return resource;
	}
	
	resource = CreateResource(typeId, filePath);
	if(!resource)
		return 0;
	
	g_ResourcesByPath[ResourceName(typeId, filePath)] = resource;
	g_ResourcesByData[resource->data]                 = resource;
	
	return resource->data;
}

void ReleaseResource( void* data )
{
	Resource* resource = GetResourceByData(data);
	if(!resource)
	{
		Log("Resource %p does not exist", data);
		return;
	}
	
	FreeResource(resource);
	
	g_ResourcesByData.erase(data);
	g_ResourcesByPath.erase(ResourceName(resource->typeId, resource->filePath));
}

void InitializeResourceManager()
{
	// ...
}

void TerminateResourceManager()
{
	{
		std::map<void*,Resource*>::const_iterator i = g_ResourcesByData.begin();
		for(; i != g_ResourcesByData.end(); i++)
		{
			FreeResource(i->second);
		}
		
		g_ResourcesByData.clear();
		g_ResourcesByPath.clear();
	}
	
	{
		std::map<Handle,ResourceType*>::const_iterator i = g_ResourceTypes.begin();
		for(; i != g_ResourceTypes.end(); i++)
		{
			delete i->second;
		}
		
		g_ResourceTypes.clear();
	}
}