#ifndef AKASHA_WORLD_BULLET_CUSTOM_CALLBACK_HOLDER_HPP_
#define AKASHA_WORLD_BULLET_CUSTOM_CALLBACK_HOLDER_HPP_

#include <Riceball/SingletonHolder.hpp>
#include <boost/function.hpp>

#include <BulletCollision/CollisionDispatch/btManifoldResult.h>

//カスタムコールバック extern
extern ContactAddedCallback     gContactAddedCallback;
extern ContactProcessedCallback gContactProcessedCallback;
extern ContactDestroyedCallback gContactDestroyedCallback;
namespace akasha
{
namespace bullet
{

using AddedCallbackFunctionType =
	boost::function<bool (
			btManifoldPoint&,
			btCollisionObjectWrapper const*, // object0
			int, // partID0
			int, //index0
			btCollisionObjectWrapper const*, //object1
			int, //partID1
			int //index1
			)>;
using ProcessCallbackFunctionType =
	boost::function<bool (
			btManifoldPoint&,
			void*, //body0
			void* //body1
			)>;
using DestroyedCallbackFunctionType =
	boost::function<bool (void*)>;

class CustomCallbackHolder
{

private:
	static AddedCallbackFunctionType addedCallback;
	static ProcessCallbackFunctionType processCallback;
	static DestroyedCallbackFunctionType destroyedCallback;

public:

	static bool AddedCallbackFunc(
			btManifoldPoint& cp,
			btCollisionObjectWrapper const* o0,
			int partId0,
			int index0,
			btCollisionObjectWrapper const* o1,
			int partId1,
			int index1)
	{
		assert(addedCallback);
		return addedCallback(cp,o0, partId0, index0, o1, partId1, index1);
	}

	static bool ProcessCallbackFunc( btManifoldPoint& cp, void* body0, void* body1)
	{
		assert(processCallback);
		return processCallback(cp, body0, body1);
	}

	static bool DestroyedCallbackFunc(void* userPersistenData)
	{
		assert(destroyedCallback);
		return destroyedCallback(userPersistenData);
	}
	// Accesser
	template<typename FuncType>
		static void setAddedCallback(FuncType f)
		{
			addedCallback = f;
			gContactAddedCallback = AddedCallbackFunc;
		}
	static void removeAddedCallback()
	{
		gContactAddedCallback = nullptr;
	}

	template<typename FuncType>
		static void setProcessCallback(FuncType f)
		{
			processCallback = f;
			gContactProcessedCallback = ProcessCallbackFunc;
		}
	static void removeProcessCallback()
	{
		gContactProcessedCallback = nullptr;
	}
	template<typename FuncType>
		static void setDestroyedCallback(FuncType f)
		{
			destroyedCallback = f;
			gContactDestroyedCallback = DestroyedCallbackFunc;
		}
	static void removeDestroyedCallback()
	{
		gContactDestroyedCallback = nullptr;
	}

	// ex. {{{
	// 					static bool
	// 					addContactCallback(btManifoldPoint,& cp,
	// 							btCollisionObjectWrapper const* ow0,
	// 							int partId0,
	// 							int index0,
	// 							btCollisionObjectWrapper const* ow1,
	// 							int partId1,
	// 							int index1)
	// 					{
	// 						auto const* o0 = ow0->m_collisionObject;
	// 						auto const* o1 = ow1->m_collisionObject;
	// 						return ((o0->getCollisionFlags() &
	// 									btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK) ?
	// 								static_cast<entity::EntityReceptor*>(o0->getUserPointer())->addContact0(cp, partId0, index0, o1, partId1, index1) : false) ||
	// 							((o1->getCollisionFlags() & btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK) ?
	// 							 static_cast<entity::EntityReceptor*>(o1->getUserPointer())->addContact1(cp, partId1, index1, o0, partId0, index0) : false);
	// 					}
	//
	//! processContactCallback
	// 				static bool
	// 					processContactCallback(btManifoldPoint& cp, void* body0,void* body1)
	// 					{
	// 						btCollisionObject* b0 = static_cast<btCollisionObject*>(body0);
	// 						btCollisionObject* b1 = static_cast<btCollisionObject*>(body1);
	// 						return
	// 							((b0->getCollisionFlags() & btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK) ?
	// 							 static_cast<entity::EntityReceptor*>(b0->getUserPointer())->processContact0(cp,b1) : false) ||
	// 							((b1->getCollisionFlags() & btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK) ?
	// 							 static_cast<entity::EntityReceptor*>(b1->getUserPointer())->processContact1(cp,b0) : false);
	// 					}

	// }}}
};


AddedCallbackFunctionType
CustomCallbackHolder::addedCallback = AddedCallbackFunctionType();
ProcessCallbackFunctionType
CustomCallbackHolder::processCallback = ProcessCallbackFunctionType();
DestroyedCallbackFunctionType
CustomCallbackHolder::destroyedCallback = DestroyedCallbackFunctionType();

} // namespace bullet
} // namespace akasha

// Initiraize
//

#endif /* end of include guard */
