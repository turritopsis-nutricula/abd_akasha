/*
 * LandReceptor.hpp
 *
 *  Created on: 2012/10/11
 *      Author: ely
 */

#ifndef LANDRECEPTOR_HPP_
#define LANDRECEPTOR_HPP_

#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/include/make_vector.hpp>
namespace akasha
{
namespace land
{
template<typename EffectManagerType, typename MeshHolderType,typename SoundManagerType>
	struct LandformReceptor : entity::DefaultReceptorImp
{
	// SE_ptr impactSound_;
	//  SE_ptr contactSound_;
	EffectManagerType& form_;
	btRigidBody* landBody_;
	MeshHolderType landMesh_;
	SoundManagerType& sound_;
	boost::unordered_set<btRigidBody*> m_contactcache;

	LandformReceptor(EffectManagerType& f, SoundManagerType& s) :
		form_(f),
		landBody_(0),
		landMesh_({0}),
		sound_(s)
		{ }

	bool
		normalTransform(
				btManifoldPoint& mani,
				int partID,
				int index,
				const btCollisionObject* other,
				int otherPartID,
				int otherIndex
				)
		{
			assert(landBody_);
			assert(landBody_->getCollisionShape()->getShapeType() == TRIANGLE_SHAPE_PROXYTYPE);
			typedef btBvhTriangleMeshShape MeshShapeType;

			mani.m_normalWorldOnB = landMesh_.getTriangleNormal(partID, index);
			mani.m_normalWorldOnB.normalize();


			return true;
		}
	bool
		addContact0(
				btManifoldPoint& m,
				int partID,
				int index,
				const btCollisionObject* other,
				int otherPartID, int otherIndex)
		{
			//TODO : 呼ばれる?
			std::cout << "call. land Reseptor 0" << std::endl;
			// 		            	  return normalTransform(m,partID, index, other, otherPartID, otherIndex);
			return true;
		}
	bool
		addContact1(
				btManifoldPoint& m,
				int partID,
				int index,
				const btCollisionObject* other,
				int otherPartID, int otherIndex
				){
			// 		            	  return normalTransform(m,partID, index, other, otherPartID, otherIndex);
			return true;
		}


	void subStepContact0(btCollisionObject const*,entity::EntityReceptor&,const btPersistentManifold&){ }
	void stepContact0(btCollisionObject const* o,entity::EntityReceptor& e,const btPersistentManifold& m)
	{
		makeDust<0>(o,e,m);
	}
	void subStepContact1(btCollisionObject const*,entity::EntityReceptor&,const btPersistentManifold&){ }
	void stepContact1(btCollisionObject const* o,entity::EntityReceptor& e,const btPersistentManifold& m)
	{
		makeDust<1>(o,e,m);
	}
	//弾着
	float shotImpact(const btVector3& pos,const btVector3& dir, float density)
	{


		//エフェクト TODO : 再実装
		/*
			irr::IrrlichtDevice& irr = irrlicht::IrrlichtManager::Instance().getIrrlichtDevice();
			irr::f32 size = dir.length() * 0.0002f;
			irr::scene::IBillboardSceneNode* smorkNode =
			irr.getSceneManager()->addBillboardSceneNode(
			0,
			irr::core::dimension2d<irr::f32>(size, size),
			irr::core::vector3df(pos.getX(), pos.getY(), pos.getZ()));
			smorkNode->setMaterialTexture(
			0,
			irr.getVideoDriver()->getTexture("Resources/dust.png"));

		//smorkNode->setMaterialType(irr::video::EMT_TRANSPARENT_ADD_COLOR);
		smorkNode->setMaterialType(irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL);
		smorkNode->getMaterial(0).Lighting = false;


		irr::scene::ISceneNodeAnimator* a =
		irr.getSceneManager()->createDeleteAnimator(2000);
		smorkNode->addAnimator(a);
		a->drop();
		*/
		//サウンド
		/*    if (impactSound_)
				{
				impactSound_->apply(
				boost::fusion::make_map<audio::tag::position,audio::tag::playing>(pos,true)
				);
				}*/
		return 0.0f;
	}

	////////////////////////////////////////////////////////////////
	template<int I>
		void makeDust(btCollisionObject const* o,entity::EntityReceptor& e,const btPersistentManifold& m)
		{
			btRigidBody const* b;
			if ( !(b = btRigidBody::upcast(o)))
				return;

			if (
					!(b->getLinearVelocity().length2() > btPow(1.f,2.f) ||
						b->getAngularVelocity().length2() > btPow(3.f,2.f))
				) return;

			unsigned int size = m.getNumContacts();
			for (unsigned int i=0; i < size; i++)
			{

				//if (m.getContactPoint(i).getDistance() <= btScalar(1.0e-3))
				if (m.getContactPoint(i).getDistance() <= btScalar(1.0e-2))
				{
					const auto& p = m.getContactPoint(i);

					/*btVector3 anglerFactor =  b->getAngularVelocity().cross(I == 0 ? p.m_localPointB : p.m_localPointAw);
					  anglerFactor.setY(btScalar(0)); // なんか落下方向に異様な値が入る
					  const btVector3 lVel = b->getLinearVelocity() + anglerFactor;*/
					const btVector3 landNormal = I==0 ? -p.m_normalWorldOnB : p.m_normalWorldOnB;
					const btVector3 lVel = b->getVelocityInLocalPoint(
							I==0 ?
							landNormal * (b->getWorldTransform().getBasis() * p.m_localPointB).dot(landNormal) :
							landNormal * (b->getWorldTransform().getBasis() * p.m_localPointA).dot(landNormal)
							);


					if (landNormal.dot(lVel) < -0.8f)
						sound_.touch(b, e, p, lVel, landNormal);

					if (lVel.length2() > 1.f)
					{
						const btVector3 dir = p.m_normalWorldOnB.cross(lVel.cross( p.m_normalWorldOnB ))/50.f;
						const auto& pos = p.getPositionWorldOnA();
						form_.add(
								boost::fusion::make_vector(
									irr::core::vector3df(pos.getX(),pos.getY(), pos.getZ() ),
									irr::core::vector3df(-(dir.getX()), -(dir.getY()), -(dir.getZ()) ),
									//convertType<irr::core::vector3df>(p.getPositionWorldOnA()),
									//convertType<irr::core::vector3df>(-dir),
									lVel.length() / 3.f
									));
					}
				}
			}
		}
};

}
}

#endif /* LANDRECEPTOR_HPP_ */
