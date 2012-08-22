#ifndef __SINGLETON_H__
#define __SINGLETON_H__

#include <void/Common.h>


/**
 * Helper function that stores a singleton.
 * Do not use directly!
 */
template<class T>
T*& Singleton_()
{
	static T* s = NULL;
	return s;
}

/**
 * Just derive your class from this one
 * and it will automatically create a singleton once its constructed.
 * Its assured that only one instance exists at a time.
 */
template<class T>
class SingletonClass
{
	protected:
		SingletonClass()
		{
			assert( Singleton_<T>() == NULL ); // Only one instance at a time!
			Singleton_<T>() = static_cast<T*>(this);
		}
		
		virtual ~SingletonClass()
		{
			Singleton_<T>() = NULL;
		}
};

/**
 * Lets you access a singleton.
 * @return Pointer to the singleton.
 */
template<class T>
T* Singleton()
{
	return Singleton_<T>();
}

#endif
