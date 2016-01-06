#ifndef DETECTIONWORLD_H
#define DETECTIONWORLD_H

#include "btBulletCollisionCommon.h"

#include <boost/scoped_ptr.hpp>
class DetectionWorld
{
    public:
        DetectionWorld() {}

        void init()
        {
            {//broadphase


             const btScalar worldBottom = btScalar(-30 * btPow(10,3));
                const btScalar worldTop = btScalar(30 * btPow(10,3));
                const btScalar worldSize = btScalar(100 * btPow(10,3));
                btVector3 worldAabbMin(-worldSize,worldBottom,-worldSize);
                btVector3 worldAabbMax(worldSize,worldTop,worldSize);
                m_broadphase.reset( new btAxisSweep3(worldAabbMin, worldAabbMax) );
            }
            {//dispater


                m_cofig.reset( new btDefaultCollisionConfiguration ());
                m_dispatcher.reset( new btCollisionDispatcher( m_cofig.get()) );
            }

                m_btWorld.reset(
                            new btCollisionWorld(m_dispatcher.get(), m_broadphase.get(), m_cofig.get()));

        }

    private:
        boost::scoped_ptr< btBroadphaseInterface > m_broadphase;
        boost::scoped_ptr< btCollisionConfiguration > m_cofig;
        boost::scoped_ptr< btDispatcher > m_dispatcher;


        boost::scoped_ptr< btCollisionWorld > m_btWorld;

};

#endif // DETECTIONWORLD_H
