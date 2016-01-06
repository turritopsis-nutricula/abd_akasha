

#ifndef BRO_IRRDEBUGDRAW__H
#define BRO_IRRDEBUGDRAW__H

#ifdef MSVC
#pragma unmanaged
#endif



//wisp
#include <wisp_v3/irrExt.hpp>
//irrlicht
#include <irrlicht.h>
//bullet
#include "btBulletDynamicsCommon.h"

#include <boost/shared_ptr.hpp>

namespace akasha {
  class btIrrDebugDraw : public btIDebugDraw {
  public:
    btIrrDebugDraw() = delete;
	 btIrrDebugDraw(irrExt::IrrExtention&);

    virtual ~btIrrDebugDraw() { }

    virtual void drawLine(const btVector3& from,const btVector3& to,const btVector3& color);

    virtual void drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color);

    virtual void reportErrorWarning(const char* warningString) { };

    virtual void draw3dText(const btVector3& location,const char* textString) {};

    virtual void setDebugMode(int debugMode);

    virtual int	getDebugMode() const;

  private:

    btIDebugDraw::DebugDrawModes m_debugMode;
    irrExt::IrrExtention& m_client;

  };
}

#endif
