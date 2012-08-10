#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <map>
#include <tools4k/Vector.h>
#include <kj/Squirrel.h>

typedef unsigned int ObjectId;
typedef unsigned int ObjectTypeId;
static const ObjectId InvalidObjectId = 0;
static const ObjectId FirstObjectId = 1;


class Object
{
	public:
		Object( ObjectId id, ObjectTypeId typeId, HSQOBJECT scriptObject, Squirrel* squirrel );
		~Object();
		
		//! Unique object id needed to identify an object over the network
		ObjectId id() const;
		
		//! Encodes the object class
		ObjectTypeId typeId() const;
		
		//! Handle to the script instance
		HSQOBJECT scriptObject() const;
		
	private:
		ObjectId m_Id;
		ObjectTypeId m_TypeId;
		HSQOBJECT m_ScriptObject;
		Squirrel* m_Squirrel;
};

class ObjectManager
{
	public:
		ObjectManager( Squirrel* squirrel );
		virtual ~ObjectManager();
		
		virtual void update() = 0;
		
		void removeObject( ObjectId id );
		Object* getObject( ObjectId id );
		const Object* getObject( ObjectId id ) const;
		
	protected:
		bool registerObject( Object* object );
		Squirrel* m_Squirrel;
		
	private:
		std::map<ObjectId,Object*> m_Objects;
};

#endif