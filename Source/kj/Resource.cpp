#include <string.h>
#include <string>
#include <list>
#include <map>

#include <kj/Common.h>
#include <kj/Resource.h>


/// ---- ResourceType ----

struct ResourceType
{
	Handle typeId;
	ResourceAllocFn allocFn;
};

std::map<Handle,ResourceType> g_ResourceTypes;


void RegisterResourceType( Handle typeId, ResourceAllocFn allocFn )
{
	if(!allocFn)
		Error("Invalid resource structure (id = %d)", typeId);
	
	ResourceType& rt = g_ResourceTypes[typeId];
	rt.typeId = typeId;
	rt.allocFn = allocFn;
}

const ResourceType* GetResourceType( Handle typeId )
{
	std::map<Handle,ResourceType>::const_iterator i = g_ResourceTypes.find(typeId);
	if(i == g_ResourceTypes.end())
		return NULL;
	else
		return &i->second;
}


/// ---- Resource ----

struct ResourceInfo
{
	Resource* data;
	std::string filePath;
	int references;
	Handle typeId;
};

std::map<std::string,ResourceInfo*> g_ResourcesByPath;
std::map<Resource*,  ResourceInfo*> g_ResourcesByData;


std::string ResourceName( Handle typeId, const char* filePath )
{
	return filePath; // FIXME
}

Resource* CreateResource( Handle typeId, const char* filePath )
{
	const ResourceType* type = GetResourceType(typeId);
	
	ResourceInfo* res = new ResourceInfo;
	res->data = type->allocFn();
	res->filePath = filePath;
	res->references = 1;
	res->typeId = type->typeId;
	
	if(!res->data->load(filePath))
	{
		Log("Can't load resource '%s'", filePath);
		delete res->data;
		delete res;
		return NULL;
	}
	
	g_ResourcesByData[res->data] = res;
	g_ResourcesByPath[filePath]  = res;
	
	return res->data;
}

void RemoveResource( Resource* data )
{
	std::map<Resource*,ResourceInfo*>::iterator i = g_ResourcesByData.find(data);
	if(i == g_ResourcesByData.end())
		return;
	
	ResourceInfo* ri = i->second;
	
	g_ResourcesByPath.erase(ResourceName(ri->typeId, ri->filePath.c_str()));
	g_ResourcesByData.erase(i);
	
	delete ri->data;
	delete ri;
}

Resource* LoadResource( Handle typeId, const char* filePath )
{
	std::map<std::string,ResourceInfo*>::const_iterator i = g_ResourcesByPath.find(ResourceName(typeId, filePath));
	if(i != g_ResourcesByPath.end())
	{
		i->second->references++;
		return i->second->data;
	}
	
	return CreateResource(typeId, filePath);
}

void ReleaseResource( Resource* data )
{
	std::map<Resource*,ResourceInfo*>::const_iterator i = g_ResourcesByData.find(data);
	if(i == g_ResourcesByData.end())
	{
		Log("Resource %p does not exist", data);
		return;
	}
	
	ResourceInfo* ri = i->second;
	ri->references--;
	if(ri->references <= 0)
		RemoveResource(data);
}



/// ---- ResourceManager ----

void InitializeResourceManager()
{
	// ...
}

void TerminateResourceManager()
{
	{
		std::map<Resource*,ResourceInfo*>::const_iterator i = g_ResourcesByData.begin();
		for(; i != g_ResourcesByData.end(); i++)
		{
			ResourceInfo* ri = i->second;
			delete ri->data;
			delete ri;
		}
		
		g_ResourcesByData.clear();
		g_ResourcesByPath.clear();
	}
	
	{
		g_ResourceTypes.clear();
	}
}