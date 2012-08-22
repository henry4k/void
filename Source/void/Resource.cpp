#include <string.h>
#include <string>
#include <list>
#include <map>

#include <void/Common.h>
#include <void/Resource.h>



/// ---- Resource ----

void Resource::release()
{
	// ... ?!!
}

std::string Resource::filePath() const
{
	return m_FilePath;
}

std::string Resource::name() const
{
	return Resource::Name(m_TypeId, m_FilePath.c_str());
}

std::string Resource::Name( int typeId, const char* filePath )
{
	return filePath;
}



/// ---- ResourceManager ----

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
	std::map<std::string, Resource*>::const_iterator i = m_Resources.begin();
	for(; i != m_Resources.end(); ++i)
	{
		delete i->second;
	}
}

void ResourceManager::registerResourceType_( int typeId, ResourceAllocFn allocFn )
{
	assert(allocFn);
	
	ResourceType& rt = m_ResourceTypes[typeId];
	rt.typeId = typeId;
	rt.allocFn = allocFn;
}

const ResourceType* ResourceManager::getResourceType( int typeId ) const
{
	std::map<int, ResourceType>::const_iterator i = m_ResourceTypes.find(typeId);
	if(i == m_ResourceTypes.end())
		return NULL;
	else
		return &i->second;
}

Resource* ResourceManager::loadResource( int typeId, const char* filePath )
{
	std::map<std::string, Resource*>::iterator i = m_Resources.find(Resource::Name(typeId, filePath));
	if(i != m_Resources.end())
	{
		++i->second->m_RefCount;
		return i->second;
	}
	else
	{
		const ResourceType* type = getResourceType(typeId);
		Resource* res = type->allocFn();
		res->m_RefCount = 0;
		res->m_TypeId = typeId;
		res->m_FilePath = filePath;

		if(!res->onLoad())
		{
			Error("Can't load resource '%s'", filePath);
			delete res;
			return NULL;
		}

		m_Resources[res->name()] = res;

		return res;
	}
}

void ResourceManager::releaseResource( Resource* res )
{
	--res->m_RefCount;
	if(res->m_RefCount > 0)
		return;
	
	m_Resources.erase(res->name());
	delete res;
}
