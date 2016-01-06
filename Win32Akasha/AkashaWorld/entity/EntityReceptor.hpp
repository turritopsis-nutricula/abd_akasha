#ifndef ENTITY_RECEPTOR__HPP
#define ENTITY_RECEPTOR__HPP

#include "btBulletDynamicsCommon.h"
#include <Riceball/VTBL_Load.hpp>
namespace akasha
{
namespace entity
{

TEMPLATE_VTBL(
		EntityReceptor,
		(void, init,  (0),  1)
		(void, subStepContact0,
		 ((btCollisionObject const*) (::akasha::entity::EntityReceptor&) (const btPersistentManifold&)), 1)
		(void, stepContact0,
		 ((btCollisionObject const*) (::akasha::entity::EntityReceptor&) (const btPersistentManifold&)), 1)
		(void, subStepContact1,
		 ((btCollisionObject const*) (::akasha::entity::EntityReceptor&) (const btPersistentManifold&)), 1)
		(void, stepContact1,
		 ((btCollisionObject const*) (::akasha::entity::EntityReceptor&) (const btPersistentManifold&)), 1)
		(bool, addContact0,
		 ((btManifoldPoint&) (int) (int) (const btCollisionObject*) (int) (int)), 1)
		(bool,addContact1,
		 ((btManifoldPoint&) (int) (int) (const btCollisionObject*) (int) (int)), 1)
		(bool, processContact0,  ((btManifoldPoint&) (btCollisionObject*)),      1)
		(bool, processContact1,  ((btManifoldPoint&) (btCollisionObject*)),      1)


		(float, shotImpact, ((const btVector3&) (const btVector3&) (float)), 1)

		)

		////////////////////////////////////////////////////////////////
		//! 標準の実装レセプター
		struct DefaultReceptorImp
	{
		void init(){ }
		void subStepContact0(btCollisionObject const*,EntityReceptor&,const btPersistentManifold&){ }
		void stepContact0(btCollisionObject const*,EntityReceptor&,const btPersistentManifold&){ }
		void subStepContact1(btCollisionObject const*,EntityReceptor&,const btPersistentManifold&){ }
		void stepContact1(btCollisionObject const*,EntityReceptor&,const btPersistentManifold&){ }
		bool
			addContact0(btManifoldPoint&, int partID,int index,const btCollisionObject* other, int otherPartID, int otherIndex){return false; }
		bool
			addContact1(btManifoldPoint&, int partID,int index,const btCollisionObject* other, int otherPartID, int otherIndex){return false; }
		bool
			processContact0(btManifoldPoint&, btCollisionObject* other){ return false; }
		bool
			processContact1(btManifoldPoint&, btCollisionObject* other){ return false; }




		float shotImpact(const btVector3&,const btVector3&, float){ return 1.f; }
	};
static DefaultReceptorImp g_DefaultReceptorImpl;
///グローバルな空レセプタ
static EntityReceptor  gDefaultReceptor(g_DefaultReceptorImpl);

}
}

/*
#define CONCEPT_ARRAY (5,( \
(1, subStepContact0, void, (3, (btCollisionObject* ,::akasha::entity::EntityReceptor&, const btPersistentManifold&))), \
(1, stepContact0,  void,  (3, (btCollisionObject* ,::akasha::entity::EntityReceptor&, const btPersistentManifold&))), \
(1, subStepContact1, void, (3, (btCollisionObject* ,::akasha::entity::EntityReceptor&, const btPersistentManifold&))), \
(1, stepContact1,  void,  (3, (btCollisionObject* ,::akasha::entity::EntityReceptor&, const btPersistentManifold&))), \
(1, shotImpact, float,   (3, (const btVector3&, const btVector3&, float))) \
))

#include "TemplateVTBL_Ex_MacroLoad.hpp"

//#include "TemplateVTBL_MacroLoad.hpp"

namespace akasha
{
namespace entity
{

//! イベントレセプター
struct EntityReceptor
{
//! VTBLテーブル定義
VTBL_STRUCT_AND_INIALIZER( VTBL, VTBLInitalizer, T, vtbl_);

//! コンストラクタ
template<typename T>
EntityReceptor(T& impl)
: this_(&impl),
vptr_(&VTBLInitalizer<T>::vtbl_)
{}

//!リセット
template<typename T>
void reset(T& impl)
{
this_ = &impl;
vptr_ = &VTBLInitalizer<T>::vtbl_;
}

void subStepContact0(btCollisionObject* other, EntityReceptor& r, const btPersistentManifold& p)
{
vptr_->subStepContact0_nc(this_, other, r, p);
}

void stepContact0(btCollisionObject* other, EntityReceptor& r, const btPersistentManifold& p)
{
vptr_->stepContact0_nc(this_, other, r, p);
}

void subStepContact1(btCollisionObject* other, EntityReceptor& r, const btPersistentManifold& p)
{
vptr_->subStepContact1_nc(this_, other, r, p);
}

void stepContact1(btCollisionObject* other, EntityReceptor& r, const btPersistentManifold& p)
{
vptr_->stepContact1_nc(this_, other, r, p);
}

float shotImpact(const btVector3& position, const btVector3& direct, float density)
{
return vptr_->shotImpact_nc(this_, position, direct, density);
}

void* this_;
VTBL* vptr_;
};

//VTBL初期化
VTBL_INIT_DEFINE( EntityReceptor, VTBL, VTBLInitalizer, vtbl_);

////////////////////////////////////////////////////////////////
//! 標準の実装レセプター
struct DefaultReceptorImp
{
	void subStepContact0(btCollisionObject*,EntityReceptor&,const btPersistentManifold&){ }
	void stepContact0(btCollisionObject*,EntityReceptor&,const btPersistentManifold&){ }
	void subStepContact1(btCollisionObject*,EntityReceptor&,const btPersistentManifold&){ }
	void stepContact1(btCollisionObject*,EntityReceptor&,const btPersistentManifold&){ }
	float shotImpact(const btVector3&,const btVector3&, float){ return 1.f; }
};

static DefaultReceptorImp gDefaultReceptor;

}
}

#include "TemplateVTBL_Ex_MacroClear.hpp"
#undef CONCEPT_ARRAY
*
* */
#endif
