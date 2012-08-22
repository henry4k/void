#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <map>

#include <tools4k/Vector.h>

#include <void/Squirrel.h>
#include <void/Singleton.h>


typedef unsigned int ObjectId;
typedef unsigned int ObjectTypeId;
static const ObjectId InvalidObjectId = 0;
static const ObjectId FirstObjectId = 1;


class Object
{
	public:
		Object( ObjectId id, ObjectTypeId typeId, HSQOBJECT scriptObject );
		virtual ~Object();
		
		/**
		 * Unique object id needed to identify an object over the network.
		 */
		ObjectId id() const;
		
		/**
		 * Encodes the object class.
		 */
		ObjectTypeId typeId() const;
		
		/**
		 * Handle to the script instance.
		 */
		HSQOBJECT scriptObject() const;
		
		/**
		 * Pushes this as user pointer in the script vm.
		 */
		virtual void pushHandle() = 0;
		
		/**
		 * Retrieves the user poiter at the specified position from the vm.
		 * Note that this is no strong reference!
		 */
		static Object* GetHandle( int index );
		
	private:
		ObjectId m_Id;
		ObjectTypeId m_TypeId;
		HSQOBJECT m_ScriptObject;
};

class ObjectManager : public SingletonClass<ObjectManager>
{
	public:
		ObjectManager();
		virtual ~ObjectManager();
		
		/**
		 * Must be called regulary. Uhm.. dunno what it does. :(
		 */
		virtual void update() = 0;
		
		/**
		 * Removes an object.
		 */
		void removeObject( ObjectId id );

		/**
		 * Returns an object pointer
		 * or NULL if the id is invalid.
		 */
		Object* getObject( ObjectId id );
		const Object* getObject( ObjectId id ) const;
		
	protected:
		bool registerObject( Object* object );
		Squirrel* m_Squirrel;
		
		friend class Object;
		
		std::map<ObjectId,Object*> m_Objects;
};

#endif
