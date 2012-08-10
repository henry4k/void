#ifndef RESOURCE_HPP
#define RESOURCE_HPP

#include <kj/Common.h>

enum
{
	RES_TEXTURE = 0
};

class Resource
{
	public:
		virtual ~Resource() {};
		
		/**
			Loads (or reloads) a resource from a file
			Return false if something went wrong
			e.g. bool LoadImage( void* data, const char* filePath ) { ...; return true; }
		**/
		virtual bool load( const char* path ) = 0;
};

/**
	Shall allocate memory for a new resource
	e.g. void* AllocImage() { return new Image; }
**/
typedef Resource* (*ResourceAllocFn)();


/**
	Call this before using the resource manager
**/
void InitializeResourceManager();

/**
	Call this when the resource manager is no longer needed (i.e. when the application is shut down)
**/
void TerminateResourceManager();

/**
	typeId can be an arbitrary number
**/
template<class ResourceT>
void RegisterResourceType( Handle typeId );

/**
	Loads a resource if its not already been loaded
	and increments the reference counter
	Returns NULL if something went wrong
**/
Resource* LoadResource( Handle typeId, const char* filePath );

/**
	Releases a resource
	i.e. decrements the reference counter and deletes it if there are no references left
**/
void ReleaseResource( Resource* data );







/// ---- impl ----

void RegisterResourceType_( Handle typeId, ResourceAllocFn allocFn );

template<class ResourceT>
Resource* AllocResource()
{
	return new ResourceT();
}

template<class ResourceT>
void RegisterResourceType( Handle typeId )
{
	RegisterResourceType_(typeId, AllocResource<ResourceT>);
}

#endif