#ifndef RESOURCE_HPP
#define RESOURCE_HPP

#include "Common.hpp"



/**
	Shall allocate memory for a new resource
	e.g. void* AllocImage() { return new Image; }
**/
typedef void* (*ResourceAllocFn)();

/**
	Frees the resources memory
	e.g. void FreeImage( void* data ) { delete (Image*)data; }
**/
typedef void (*ResourceFreeFn)( void* data );

/**
	Loads (or reloads) a resource from a file
	Return false if something went wrong
	e.g. bool LoadImage( void* data, const char* filePath ) { ...; return true; }
**/
typedef bool (*ResourceLoadFn)( void* data, const char* filePath );



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
void RegisterResourceType( Handle typeId, ResourceAllocFn allocFn, ResourceFreeFn freeFn, ResourceLoadFn loadFn );

/**
	Loads a resource if its not already been loaded
	and increments the reference counter
	Returns NULL if something went wrong
**/
void* LoadResource( Handle typeId, const char* filePath );

/**
	Releases a resource
	i.e. decrements the reference counter and deletes it if there are no references left
**/
void ReleaseResource( void* data );



#endif