#ifndef BULLET_EXTENTION__H
#define BULLET_EXTENTION__H

//bullet
#include "btBulletDynamicsCommon.h"

//bboost
#include <boost/unordered_map.hpp>
#include <boost/circular_buffer.hpp>

namespace akasha{
/*  namespace bullet{
    //TODO:�K�v��?
    //! �ڐG�_���
    struct contactInfo{
      btCollisionObject& object0;
      btCollisionObject& object1;
      btVector3 pointInWorld;
      btVector3 normalInWorld;
      btScalar appliedImpulse;

      //! �m�[�}���R���X�g���N�^
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

      //! btManifoldPoint���g�����R���X�g���N�^
      contactInfo(
        btCollisionObject& obj0,
        btCollisionObject& obj1,
        const btManifoldPoint& pt) :
      object0( obj0 ),
      object1( obj1 ),
      pointInWorld( pt.getPositionWorldOnB() ),
      normalInWorld( pt.m_normalWorldOnB ),
      appliedImpulse(pt.getAppliedImpulse()){ }

      //! �R�s�[�R���X�g���N�^
      contactInfo(const contactInfo& i) :
      object0( i.object0 ),
      object1( i.object1 ),
      pointInWorld( i.pointInWorld ),
      normalInWorld( i.normalInWorld ),
      appliedImpulse( i.appliedImpulse ){ };

      //! ������Z�q
      contactInfo& operator=(const contactInfo& i){
        this->object0  = i.object0;
        this->object1 = i.object1;
        this->pointInWorld = i.pointInWorld;
        this->normalInWorld =  i.normalInWorld;
        this->appliedImpulse = i.appliedImpulse;
        return (*this);
      };
    };

    //! �}�j�t�H�[���h�̃n�b�V���֐�
//     static std::size_t hash_value( btPersistentManifold* const m){
//       size_t h = 0;
//       boost::hash_combine(h,m->getBody0());
//       boost::hash_combine(h,m->getBody1());
//       return h;
//     };

    //! �ڐG�_���R���e�i
    typedef boost::circular_buffer<contactInfo> contactInfoArray;
    typedef boost::unordered_map< btPersistentManifold*, contactInfoArray> manifoldMap;

    //�ڐG�R�[���o�b�N
    struct contactTickCallback{
      //! ���W�σo�b�t�@
      manifoldMap& buffer;

      contactTickCallback(manifoldMap& buf) :
      buffer( buf ){ };

      //�R�[���o�b�N
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
