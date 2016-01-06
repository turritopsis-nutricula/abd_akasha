#ifndef BULLET_HFFLUIDDYNAMICS_HPP
#define BULLET_HFFLUIDDYNAMICS_HPP


#include "fluidDynamics/fluid/btHfFluid.h"
// #include "../BulletHfFluid/btHfFluidBuoyantConvexShape.h"
//#include "BulletHfFluid/btHfFluidRigidDynamicsWorld.h"

#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <LinearMath/btIDebugDraw.h>
#include <LinearMath/btQuickprof.h>
#include <boost/mpl/arg.hpp>

#define DRAWMODE_NORMAL 0
#define DRAWMODE_VELOCITY 1
#define DRAWMODE_MAX 2

#define BODY_DRAWMODE_NORMAL 0
#define BODY_DRAWMODE_VOXEL 1
#define BODY_DRAWMODE_MAX 2


typedef	btAlignedObjectArray<btHfFluid*> btHfFluidArray;

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
namespace akasha
{
namespace bullet
{
template<typename Base, typename Info>
	class HfFluidDynamicsImpl : public Base
{
	btHfFluidArray	m_hfFluids;
	int m_drawMode;
	int m_bodyDrawMode;


public:

	virtual ~HfFluidDynamicsImpl(){ }

	template<typename T>
		HfFluidDynamicsImpl(const T& t) : Base(t)
	{
		m_drawMode = DRAWMODE_NORMAL;
		m_bodyDrawMode = BODY_DRAWMODE_NORMAL;
	}

	HfFluidDynamicsImpl(btDispatcher* dispatcher,btBroadphaseInterface* pairCache,btConstraintSolver* constraintSolver,btCollisionConfiguration* collisionConfiguration) :
		Base(dispatcher, pairCache, constraintSolver,  collisionConfiguration)
	{
		m_drawMode = DRAWMODE_NORMAL;
		m_bodyDrawMode = BODY_DRAWMODE_NORMAL;
	}

protected:
	virtual void predictUnconstraintMotion(btScalar timeStep)
	{
		Base::predictUnconstraintMotion( timeStep);
		/*
			for ( int i=0;i<m_hfFluids.size();++i)
			{
			btHfFluid* phff = m_hfFluids[i];

		// XXX: phff->predictMotion(timeStep);
		}*/
	}
	virtual void	internalSingleStepSimulation( btScalar timeStep)
	{
		Base::internalSingleStepSimulation( timeStep );
		// 				updateFluids (timeStep);
		// 				solveFluidConstraints (timeStep);
	}

	void	updateFluids(btScalar timeStep)
	{
		BT_PROFILE("updateFluids");
		for ( int i=0;i<m_hfFluids.size();i++)
		{
			btHfFluid*	phff=(btHfFluid*)m_hfFluids[i];
			phff->predictMotion (timeStep);
		}
	}

	void	solveFluidConstraints(btScalar timeStep)
	{
		BT_PROFILE("solve Fluid Contacts");

#if 0 ////////////////
		if(m_hfFluids.size())
		{
			btHfFluid::solveClusters(m_hfFluids);
		}

		for(int i=0;i<m_hfFluids.size();++i)
		{
			btHfFluid*	psb=(btHfFluid*)m_hfFluids[i];
			psb->solveConstraints();
		}
#endif ////////////////
	}
public:
	void	addHfFluid(
			btHfFluid* fluid,
			short group = btBroadphaseProxy::DefaultFilter,
			short mask = btBroadphaseProxy::AllFilter)
	{
		m_hfFluids.push_back(fluid);
		Base::addCollisionObject(
				fluid,
				group,
				mask
				);
	}

	void	removeHfFluid(btHfFluid* fluid)
	{
		m_hfFluids.remove(fluid);
		Base::removeCollisionObject(fluid);
	}

	void setDrawMode (int drawMode)
	{
		m_drawMode = drawMode;
	}

	void setBodyDrawMode (int bodyDrawMode)
	{
		m_bodyDrawMode = bodyDrawMode;
	}

	btHfFluidArray& getHfFluidArray()
	{
		return m_hfFluids;
	}

	const btHfFluidArray& getHfFluidArray() const
	{
		return m_hfFluids;
	}


	//debug Draw////////////////////////////////////////////////////////////////
	virtual void	debugDrawWorld()
	{
		/*
			if ( Base::getDebugDrawer())
			{
			int i;
			for (  i=0;i<this->m_hfFluids.size();i++)
			{
			btHfFluid*	phh=(btHfFluid*)this->m_hfFluids[i];
			switch (m_drawMode)
			{
			case DRAWMODE_NORMAL:
			drawHfFluidGround (Base::m_debugDrawer, phh);
		//drawHfFluidNormal (m_debugDrawer, phh);
		break;
		case DRAWMODE_VELOCITY:
		drawHfFluidGround (Base::m_debugDrawer, phh);
		//drawHfFluidNormal (m_debugDrawer, phh);
		drawHfFluidVelocity (Base::m_debugDrawer, phh);
		break;
		default:
		btAssert (0);
		break;
		}
		}
		for (i = 0; i < this->m_collisionObjects.size(); i++)
		{
		btCollisionShape* shape = Base::m_collisionObjects[i]->getCollisionShape();
		if (shape->getShapeType() == HFFLUID_BUOYANT_CONVEX_SHAPE_PROXYTYPE)
		{
		btHfFluidBuoyantConvexShape* buoyantShape = (btHfFluidBuoyantConvexShape*)shape;
		drawHfFluidBuoyantConvexShape (
		Base::m_debugDrawer,
		Base::m_collisionObjects[i], buoyantShape, m_bodyDrawMode);
		}
		}
		}*/
		Base::debugDrawWorld();
	}

protected:
	void drawHfFluidGround (btIDebugDraw* debugDraw, btHfFluid* fluid)
	{
		const btScalar* ground = fluid->getGroundArray ();
		btVector3 com = fluid->getWorldTransform().getOrigin();
		btVector3 color = btVector3(btScalar(0.13f), btScalar(0.13f), btScalar(0.0));
		for (int i = 1; i < fluid->getNumNodesWidth()-1; i++)
		{
			for (int j = 1; j < fluid->getNumNodesLength()-1; j++)
			{
				int sw = fluid->arrayIndex (i, j);
				int se = fluid->arrayIndex (i+1, j);
				int nw = fluid->arrayIndex (i, j+1);
				int ne = fluid->arrayIndex (i+1, j+1);
				btVector3 swV = btVector3 (fluid->widthPos (i), ground[sw], fluid->lengthPos (j));
				btVector3 seV = btVector3 (fluid->widthPos (i+1), ground[se], fluid->lengthPos (j));
				btVector3 nwV = btVector3 (fluid->widthPos (i), ground[nw], fluid->lengthPos (j+1));
				btVector3 neV = btVector3 (fluid->widthPos (i+1), ground[ne], fluid->lengthPos (j+1));
				debugDraw->drawTriangle (swV+com, seV+com, nwV+com, color, btScalar(1.0f));
				debugDraw->drawTriangle (seV+com, neV+com, nwV+com, color, btScalar(1.0f));
			}
		}
	}

	void drawHfFluidVelocity (btIDebugDraw* debugDraw, btHfFluid* fluid)
	{
		btScalar alpha(0.7f);
		const btScalar* height = fluid->getHeightArray ();
		btVector3 com = fluid->getWorldTransform().getOrigin();
		btVector3 red = btVector3(btScalar(1.0f), btScalar(0.0f), btScalar(0.0));
		btVector3 green = btVector3(btScalar(0.0f), btScalar(1.0f), btScalar(0.0));
		const bool* flags = fluid->getFlagsArray ();
		for (int i = 1; i < fluid->getNumNodesWidth()-1; i++)
		{
			for (int j = 1; j < fluid->getNumNodesLength()-1; j++)
			{
				int index = fluid->arrayIndex (i, j);
				if (!flags[index])
					continue;
				btVector3 from = btVector3 (fluid->widthPos (i), height[index]+btScalar(0.1f), fluid->lengthPos (j));
				btVector3 velocity;
				velocity.setX (fluid->getVelocityUArray()[index]);
				velocity.setY (btScalar(0.0f));
				velocity.setZ (fluid->getVelocityVArray()[index]);
				velocity.normalize();
				btVector3 to = from + velocity;

				debugDraw->drawLine (from+com, to+com, red, green);
			}
		}
	}
	void drawHfFluidNormal (btIDebugDraw* debugDraw, btHfFluid* fluid)
	{
		int colIndex = 0;
		btVector3 col[2];
		col[0] = btVector3(btScalar(0.0f), btScalar(0.0f), btScalar(1.0));
		col[1] = btVector3(btScalar(0.0f), btScalar(0.5f), btScalar(0.5));
		btScalar alpha(0.7f);
		const btScalar* height = fluid->getHeightArray ();
		const btScalar* eta = fluid->getEtaArray ();
		const btScalar* ground = fluid->getGroundArray ();
		btVector3 com = fluid->getWorldTransform().getOrigin();
		const bool* flags = fluid->getFlagsArray ();
		for (int i = 0; i < fluid->getNumNodesWidth()-1; i++)
		{
			for (int j = 0; j < fluid->getNumNodesLength()-1; j++)
			{
				int sw = fluid->arrayIndex (i, j);
				int se = fluid->arrayIndex (i+1, j);
				int nw = fluid->arrayIndex (i, j+1);
				int ne = fluid->arrayIndex (i+1, j+1);

				btScalar h = eta[sw];
				btScalar g = ground[sw];

				if (h < btScalar(0.05f))
					continue;

				if (h <= btScalar(0.01f))
					continue;

				btVector3 boxMin = btVector3(fluid->widthPos (i), g, fluid->lengthPos(j));
				btVector3 boxMax = btVector3(fluid->widthPos(i+1), g+h, fluid->lengthPos(j+1));
				boxMin += com;
				boxMax += com;

				debugDraw->drawBox (boxMin, boxMax, btVector3(btScalar(0.0f), btScalar(0.0f), btScalar(1.0f)));
			}
		}
	}
};

typedef HfFluidDynamicsImpl<boost::mpl::_1,boost::mpl::_2> HfFluidDynamics;

}
}


#undef DRAWMODE_NORMAL
#undef DRAWMODE_VELOCITY
#undef DRAWMODE_MAX

#undef BODY_DRAWMODE_NORMAL
#undef BODY_DRAWMODE_VOXEL
#undef BODY_DRAWMODE_MAX

#endif
