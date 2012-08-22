#ifndef __VOID_RESOURCE_H__
#define __VOID_RESOURCE_H__

#include <map>
#include <string>

#include <void/Common.h>
#include <void/Singleton.h>


enum
{
	RES_TEXTURE2D,
	RES_TEXTURECUBE,
	RES_MESH,
	RES_SOUND
};

class Resource
{
	public:
		virtual ~Resource() {};
				
		/**
		 * ...
		 */
		void release();
		
		/**
		 * ...
		 */
		std::string filePath() const;
		
		/**
		 * Uniqe resource name created from its type and file path.
		 */
		std::string name() const;

		/**
		 * Static version of name();
		 */
		static std::string Name( int typeId, const char* filePath );

	protected:
		/**
		 * Called when the resource needs to be (re)loaded.
		 */
		virtual bool onLoad() = 0;

	private:
		std::string m_FilePath;
		int m_RefCount;
		int m_TypeId;

		friend class ResourceManager;
};

/**
 * Shall allocate memory for a new resource
 * e.g. void* AllocImage() { return new Image; }
 */
typedef Resource* (*ResourceAllocFn)();



class ResourceType
{
	public:
		int typeId;
		ResourceAllocFn allocFn;
};



class ResourceManager : public SingletonClass<ResourceManager>
{
	public:
		ResourceManager();
		~ResourceManager();
		
		/**
		 * @param typeId Each type must be associated with an unique number.
		 */
		template<class ResourceT>
		void registerResourceType( int typeId );
		
		/**
		 * Loads a resource if its not already been loaded
		 * and increments the reference counter
		 * Returns NULL if something went wrong
		 */
		Resource* loadResource( int typeId, const char* filePath );

		/**
		 * Releases a resource
		 * i.e. decrements the reference counter and deletes it if there are no references left
		 */
		void releaseResource( Resource* res );

	private:
		const ResourceType* getResourceType( int typeId ) const;
		void registerResourceType_( int typeId, ResourceAllocFn allocFn );
		
		std::map<int, ResourceType> m_ResourceTypes;
		std::map<std::string, Resource*> m_Resources;
};
/// --- Impl ---

template<class ResourceT>
Resource* AllocResource_()
{
	return new ResourceT();
}

template<class ResourceT>
void ResourceManager::registerResourceType( int typeId )
{
	registerResourceType_(typeId, AllocResource_<ResourceT>);
}

#endif
