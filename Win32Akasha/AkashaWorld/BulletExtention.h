#ifndef BULLET_EXTENTION__H
#define BULLET_EXTENTION__H

//bullet
#include "btBulletDynamicsCommon.h"

//bboost
#include <boost/unordered_map.hpp>
#include <boost/circular_buffer.hpp>

namespace akasha{
/*  namespace bullet{
    //TODO:必要か?
    //! 接触点情報
    struct contactInfo{
      btCollisionObject& object0;
      btCollisionObject& object1;
      btVector3 pointInWorld;
      btVector3 normalInWorld;
      btScalar appliedImpulse;

      //! ノーマルコンストラクタ
      contactInfo(
        btCollisionObject& obj0,
        btCollisionObject& obj1,
        const btVector3& point,
        const btVector3& nomral,
        btScalar impulse
        ) :
      object0( obj0 ),
      object1( obj1 ),
      pointInWorld( point ),
      normalInWorld( nomral ),
      appliedImpulse( impulse )
        { };

      //! btManifoldPointを使ったコンストラクタ
      contactInfo(
        btCollisionObject& obj0,
        btCollisionObject& obj1,
        const btManifoldPoint& pt) :
      object0( obj0 ),
      object1( obj1 ),
      pointInWorld( pt.getPositionWorldOnB() ),
      normalInWorld( pt.m_normalWorldOnB ),
      appliedImpulse(pt.getAppliedImpulse()){ }

      //! コピーコンストラクタ
      contactInfo(const contactInfo& i) :
      object0( i.object0 ),
      object1( i.object1 ),
      pointInWorld( i.pointInWorld ),
      normalInWorld( i.normalInWorld ),
      appliedImpulse( i.appliedImpulse ){ };

      //! 代入演算子
      contactInfo& operator=(const contactInfo& i){
        this->object0  = i.object0;
        this->object1 = i.object1;
        this->pointInWorld = i.pointInWorld;
        this->normalInWorld =  i.normalInWorld;
        this->appliedImpulse = i.appliedImpulse;
        return (*this);
      };
    };

    //! マニフォールドのハッシュ関数
//     static std::size_t hash_value( btPersistentManifold* const m){
//       size_t h = 0;
//       boost::hash_combine(h,m->getBody0());
//       boost::hash_combine(h,m->getBody1());
//       return h;
//     };

    //! 接触点情報コンテナ
    typedef boost::circular_buffer<contactInfo> contactInfoArray;
    typedef boost::unordered_map< btPersistentManifold*, contactInfoArray> manifoldMap;

    //接触コールバック
    struct contactTickCallback{
      //! 情報集積バッファ
      manifoldMap& buffer;

      contactTickCallback(manifoldMap& buf) :
      buffer( buf ){ };

      //コールバック
      void operator()(btDynamicsWorld* world, btScalar step){
        const int numManifolds = world->getDispatcher()->getNumManifolds();
        for (int i=0;i<numManifolds;i++)
          {
            btPersistentManifold* contactManifold =  world->getDispatcher()->getManifoldByIndexInternal(i);
            btCollisionObject const* obA =
					static_cast<btCollisionObject const*>( contactManifold->getBody0());
            btCollisionObject const* obB =
					static_cast<btCollisionObject const*>(contactManifold->getBody1());

            const int numContacts = contactManifold->getNumContacts();
            for (int j=0;j<numContacts;j++)
              {
                const btManifoldPoint pt = contactManifold->getContactPoint(j);
                if (pt.getDistance()<0.f)
                  {
                    buffer[ contactManifold ].push_back(
                      contactInfo(*obA, *obB,  pt)
                      );
                  }} //for numContacts

          } //for numManifolds
      };
    };


  };*/
};

#endif //BULLET_EXTENTION__H
