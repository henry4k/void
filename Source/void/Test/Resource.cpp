#include <void/Common.h>
#include <void/Resource.h>

struct Dummy : Resource
{
	int foo;
	int bar;
	
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
	RegisterResourceType<Dummy>(1);
	
	Dummy* dummy = (Dummy*)LoadResource(1, "FooBar.dummy");
	ReleaseResource(dummy);
	
	TerminateResourceManager();
}