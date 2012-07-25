#include <kj/Common.h>
#include <kj/Resource.h>

struct Dummy : Resource
{
	int foo;
	int bar;
	
	static Resource* Alloc()
	{
		return new Dummy;
	}
	
	bool load( const char* filePath )
	{
		foo = 1;
		bar = 2;
		return true;
	}
};

int main()
{
	InitializeResourceManager();
	RegisterResourceType(1, Dummy::Alloc);
	
	Dummy* dummy = (Dummy*)LoadResource(1, "FooBar.dummy");
	ReleaseResource(dummy);
	
	TerminateResourceManager();
}