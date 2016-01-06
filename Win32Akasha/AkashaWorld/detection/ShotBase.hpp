#ifndef SHOT_BASE__HPP
#define SHOT_BASE__HPP


#include "figuer/BeamFiguer.hpp"

#include <wisp_v3/wisp.hpp>


#include "../Receptor/Receptors.hpp"
#include "../BulletWorldManager.h"
#include "../akashaWorldCommon.h"

#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>

#include <boost/ptr_container/ptr_vector.hpp>
namespace akasha
{
namespace detection
{
template<class ShotType>
	class ShotBase
{

	struct bufferItem
	{
		ShotType shot_;
		bool isUpdate_;
		bufferItem( btCollisionObject* o, wisp::v3::WispHandle& c) :
			shot_( o, c)
		{
		}
	};

	typedef typename ShotType::argType argType;
	typedef bufferItem bufferType;
	boost::ptr_vector<bufferType> m_shotBuffer;

	btCollisionObject* m_exceptObject;

	int m_cout;

	wisp::v3::WispHandle* m_wispClient;
public:
	/*ShotBase(wisp::v3::WispHandle& c) :
	  m_exceptObject(0), m_cout(0), m_wispClient(c)
	  {

	  }*/
	ShotBase(): m_exceptObject(nullptr), m_cout(0), m_wispClient(nullptr)
	{

	}



	void setWispClient( wisp::v3::WispHandle& c)
	{
		m_wispClient = &c;
	}

	void
		setExeptObject(btCollisionObject* o)
		{
			m_exceptObject = o;
		}
	void
		update(const float time)
		{
			/*if (m_cout++ > 10)
			  {
			  m_cout = 0;
			  }
			  else
			  {
			  return;
			  }*/

			BOOST_FOREACH(bufferType & item, m_shotBuffer)
			{
				ShotType& b = item.shot_;

				if (item.isUpdate_)
				{
					if (b.isActive())
					{
						b.update(time);
					}
					else
					{
						item.isUpdate_ = false;
						b.unmaterialise();
					}
				}
				/*
					if (b.isActive())
					b.update(time, m_service);
					else
					{
					if (item.isUpdate_ || true)
					{
					item.isUpdate_ = false;
					b.unmaterialise(m_service);
					}
					}*/
			}
		}

	//追加する

	bool
		add(const argType&& a)
		{

			if (auto s = findIsFinished())
			{
				(*s).shot_.materialise(std::move(a));
				(*s).isUpdate_ = true;
				return true;
			}
			else
			{
				if (m_shotBuffer.size() > 10)
				{
					return false;
				}

				bufferType* b = new bufferType( m_exceptObject, *m_wispClient);
				b->isUpdate_ = true;
				m_shotBuffer.push_back(b);

				b->shot_.materialise(std::move(a));

			}

			return true;
		}

private:
	//! 終了した弾丸を検索して返す
	boost::optional<bufferType&> findIsFinished()
	{
		BOOST_FOREACH(bufferType& b, m_shotBuffer)
		{
			//if (! (b.shot_.isActive()))
			if (! (b.isUpdate_))
				return b;
		}
		return boost::none;
	}

};

////////////////////////////////////////////////////////////////
struct ShotMuzzle
{
	btVector3 position_;
	btVector3 direct_;
	btVector3 velocity_;
	btScalar energy_;
	ShotMuzzle(const btVector3& p, const btVector3& d, const btVector3& v,
			const btScalar e) :
		position_(p), direct_(d), velocity_(v), energy_(e)
	{
	}
};
////////////////////////////////////////////////////////////////
struct ImpactObject
{
	btCollisionObject* obj_;
	//btVector3 contactPoint_;
	btScalar contactPoint_[6];
	btScalar baseFromDistance_;
	ImpactObject(const btScalar distance, const btVector3& p,
			const btVector3& dir, btCollisionObject* o) :
		obj_(o)
	{
		contactPoint_[0] = p.getX();
		contactPoint_[1] = p.getY();
		contactPoint_[2] = p.getZ();
		contactPoint_[3] = dir.getX();
		contactPoint_[4] = dir.getY();
		contactPoint_[5] = dir.getZ();
		baseFromDistance_ = distance;
	}

	void
		getContactPoint(btVector3& vec) const
		{
			vec.setValue(contactPoint_[0], contactPoint_[1], contactPoint_[2]);
		}
	void
		getContactDirect(btVector3& vec) const
		{
			vec.setValue(contactPoint_[3], contactPoint_[4], contactPoint_[5]);
		}
};

static
	bool
operator<(const ImpactObject& l, const ImpactObject& r)
{
	return l.baseFromDistance_ < r.baseFromDistance_;
}

///////////////////////////////////////////////////////////////
struct DetectObjectDeletor
{
	void operator()(btGhostObject* o) const
	{
		//    		std::cout << "delete beam by deletor." << std::endl;
		BulletManager::Instance().removeCollisionObject(o);
		delete o;
	}
};
////////////////////////////////////////////////////////////////
template<template<class > class R>
class LowVelocityBeam
{

	R<LowVelocityBeam> m_receptor;

	boost::shared_ptr<btGhostObject> m_collision;
	boost::shared_ptr<btBoxShape> m_shape;

	entity::EntityReceptor m_receptorWrapper;
	BeamFiguer m_figuer;

	float m_time;
	bool m_isActive;
	btVector3 m_velocity;
	btVector3 m_pos;
	btScalar m_energy;
	std::vector<ImpactObject> m_impactBuffer;

	btCollisionObject* m_exceptObject;


public:
	typedef ShotMuzzle argType;

	LowVelocityBeam(btCollisionObject* exceptObj, wisp::v3::WispHandle& c) :
		m_receptorWrapper(m_receptor),
		m_figuer("Resources/effect/bullet.png", c),
		m_isActive(false), m_exceptObject(exceptObj)
	{

		//レセプタセット
		m_receptor.s_ = this;


		m_shape.reset(new btBoxShape(btVector3(1.f,1.f,1.f)));
		m_collision.reset(new btGhostObject(), DetectObjectDeletor());
		m_collision->setCollisionShape(m_shape.get());
		m_collision->setCollisionFlags(m_collision->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

		//           m_collision->setUserPointer(&m_receptorWrapper);
	}

	//! 顕現化
	void
		materialise(const argType&& a)
		{
			assert(!m_isActive);
			//ステータス初期化
			m_isActive = true;
			m_time = 0.f;

			m_pos = a.position_;
			m_velocity = a.velocity_ + a.direct_ * btScalar(600.0f);
			m_energy = a.energy_;
			m_impactBuffer.clear();

			//IrrNodeセット
			//irr::f32 size = 1.0f ; //(m_energy - 5000.f) * 0.00002f + 0.01f;

			m_collision->setWorldTransform(btTransform::getIdentity());
			assert(m_collision.get());
			BulletManager::Instance().addCollisionObject(m_collision.get(), COL_TYPE_BOM, COL_TYPE_MODEL | COL_TYPE_LAND);

			//std::cout << "materialise beam." << std::endl;
		}

	//! 消滅
	void unmaterialise()
	{

		//        	std::cout << "unmaterialise beam." << std::endl;
		BulletManager::Instance().removeCollisionObject(m_collision.get());
		m_isActive = false;
	}

	//! 生存フラグ
	bool isActive() const
	{
		return m_isActive;
	}

	//! 更新
	template<typename T>
		void update(const T time)
		{
			m_time += time;

			//TODO : 先に地形に当たるとモデルに当たらない
			// 地形のみRayによる判定を行なっているがモデルの当たり判定は一度stepSumilationを通す必要があるので
			// 1フレーム目の範囲に地形があるとそこで消失する
			checkLandContact(time);

			//衝突イベント処理
			bool nonImpact = checkImpact();

			if (nonImpact)
			{
				//位置更新
				const btVector3 to = m_pos + m_velocity * time;
				calcForm( m_pos, to);

				//表示位置反映
				m_figuer.setWorldTransform(m_collision->getWorldTransform());

				//次フレーム用更新
				m_pos = to;
			}

			m_isActive = m_time < 5.f && nonImpact;
			//m_isActive = m_time < 0.5f && nonImpact;

			//m_velocity.setY( m_velocity.getY() - 9.81 * time );

			m_impactBuffer.clear();

		}

	//! 接触コールバック
	void contact(
			btCollisionObject* o,
			entity::EntityReceptor& r,
			const btPersistentManifold& m
			)
	{
		if (o == m_exceptObject) return;

		const int num = m.getNumContacts();
		for (int i=0; i<num; i++)
		{
			const btManifoldPoint& p = m.getContactPoint(i);

			//std::cout << i << ": distance:" << p.getDistance() <<std::endl;

			if(p.getDistance() <= 0.f + SIMD_EPSILON)
			{
				const btVector3& pos = p.getPositionWorldOnB();
				const btVector3 sub = pos-m_pos;

				//std::cout << "contact hit." <<std::endl;

				m_impactBuffer.push_back(
						ImpactObject(
							sub.length(),
							pos,
							sub.normalized(),
							o
							));
			}
		}
	}

private:
	//地形との当たり判定を追加処理
	template<typename T>
		void
		checkLandContact(T time)
		{
			const btVector3 to = m_pos + m_velocity * time;
			btCollisionWorld::ClosestRayResultCallback callback(m_pos, to);
			callback.m_collisionFilterGroup = COL_TYPE_BOM;
			callback.m_collisionFilterMask = COL_TYPE_LAND;

			BulletManager::Instance().rayTest(m_pos, to, callback);

			if (callback.hasHit())
			{
				//	std::cout << "hit by ray." << callback.m_hitPointWorld << std::endl;
				const btVector3 sub = m_pos-callback.m_hitPointWorld;
				m_impactBuffer.push_back(
						ImpactObject(
							sub.length(),
							callback.m_hitPointWorld,
							sub.normalized(),
							callback.m_collisionObject
							));
			}
		}

	//姿勢計算
	void calcForm(const btVector3& from,const btVector3& to)
	{
		const btVector3 halfDir = (to-from)*0.5f;
		btTransform& t = m_collision->getWorldTransform();

		t.getBasis().setRotation(
				btQuaternion(
					halfDir.cross(btVector3(0.f,1.f,0.f)),
					-halfDir.angle(btVector3(0.f,1.f,0.f))
					));
		t.setOrigin(from+halfDir);

		m_collision->getCollisionShape()->setLocalScaling(
				btVector3(0.1f, halfDir.length(), 0.1f));
	}
	//! 衝突処理
	// falseで弾判定消失
	bool checkImpact()
	{

		float ratio = 1.0;
		//ソート
		std::sort(m_impactBuffer.begin(), m_impactBuffer.end());

		BOOST_FOREACH(const ImpactObject& o, m_impactBuffer )
		{

			btCollisionObject* obj = o.obj_;
			if (obj && obj != m_exceptObject)
			{
				//                     entity::EntityReceptor& r = *(static_cast<entity::EntityReceptor*>(obj->getUserPointer()));
				btVector3 pos;
				btVector3 dir;
				o.getContactPoint( pos );
				o.getContactDirect( dir );

				//レセプタ経由でコールバック
				//                     ratio = r.shotImpact(pos, dir * m_energy * ratio, 1.f);
				//
				//                     if (ratio <= 0.f)
				//                     	return false;
			}
		}
		return true;
	}

};
}
}

#endif
