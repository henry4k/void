#include <kj/Common.h>
#include <kj/Squirrel.h>
#include <kj/Object.h>




/// ----- Object ------

Object::Object( ObjectId id, ObjectTypeId typeId, HSQOBJECT scriptObject, Squirrel* squirrel ) :
	m_Id(id),
	m_TypeId(typeId),
	m_ScriptObject(scriptObject),
	m_Squirrel(squirrel)
{
	sq_addref(m_Squirrel->vm(), &m_ScriptObject);
}

Object::~Object()
{
	sq_release(m_Squirrel->vm(), &m_ScriptObject);
}

ObjectId Object::id() const
{
	return m_Id;
}

ObjectTypeId Object::typeId() const
{
	return m_TypeId;
}

HSQOBJECT Object::scriptObject() const
{
	return m_ScriptObject;
}



/// ----- ObjectManager -----

SQInteger fn_ObjectGetId( HSQUIRRELVM vm );
SQInteger fn_GetObjectById( HSQUIRRELVM vm );

ObjectManager::ObjectManager( Squirrel* squirrel ) :
	m_Squirrel(squirrel)
{
	m_Squirrel->registerFunction("ObjectGetId", fn_ObjectGetId, 2, ".p");
	m_Squirrel->registerFunction("GetObjectById", fn_GetObjectById, 2, ".i");
}

ObjectManager::~ObjectManager()
{
	std::map<ObjectId,Object*>::iterator i = m_Objects.begin();
	for(; i != m_Objects.end(); i++)
		delete i->second;
	
	m_Objects.clear();
}

bool ObjectManager::registerObject( Object* object )
{
	if(m_Objects.count(object->id()))
	{
		delete object;
		return false;
	}
	
	m_Objects[object->id()] = object; // löl
	return true;
}

void ObjectManager::removeObject( ObjectId id )
{
	std::map<ObjectId,Object*>::iterator i = m_Objects.find(id);
	if(i == m_Objects.end())
		return;
	
	delete i->second;
	m_Objects.erase(i);
}

Object* ObjectManager::getObject( ObjectId id )
{
	std::map<ObjectId,Object*>::const_iterator i = m_Objects.find(id);
	return (i != m_Objects.end()) ? (i->second) : (NULL);
}

const Object* ObjectManager::getObject( ObjectId id ) const
{
	std::map<ObjectId,Object*>::const_iterator i = m_Objects.find(id);
	return (i != m_Objects.end()) ? (i->second) : (NULL);
}


/// -------------- Squirrel Bindings ------------------ ///

SQInteger fn_ObjectGetId( HSQUIRRELVM vm ) // handle
{
	Object* p = NULL;
	sq_getuserpointer(vm, 2, (SQUserPointer*)&p);
	sq_pushinteger(vm, p->id());
	return 1;
}

SQInteger fn_GetObjectById( HSQUIRRELVM vm ) // id
{
// 	SQInteger id;
// 	sq_getinteger(vm, 2, &id);
// 	
// 	Object* p = (Object*)GetObject(id);
// 	if(!p)
// 		return 0;
// 	
// 	sq_pushobject(vm, p->scriptObject);
	return 1;
}