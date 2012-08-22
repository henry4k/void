#include <void/Common.h>
#include <void/Singleton.h>

class Foo : public SingletonClass<Foo>
{
	public:
		Foo()
		{
			Log("Foo constructed");
		}
		
		virtual ~Foo()
		{
			Log("Foo destructed");
		}
		
		virtual void foo()
		{
			Log("Foo!");
		}
};

class SuperFoo : public Foo, public SingletonClass<SuperFoo>
{
	public:
		SuperFoo()
		{
			Log("SuperFoo constructed");
		}
		
		~SuperFoo()
		{
			Log("SuperFoo destructed");
		}
		
		virtual void foo()
		{
			Log("Super Foo!!");
		}
};

int main()
{
	SuperFoo hehe;
	Singleton<SuperFoo>()->foo();
	Singleton<Foo>()->foo();
	return 0;
}