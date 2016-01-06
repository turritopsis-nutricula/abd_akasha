/*
 * BeamFiguer.cpp
 *
 *  Created on: 2013/03/15
 *      Author: ely
 */


#include "BeamFiguer.hpp"
#include "../../WispTasks.hpp"

using namespace akasha::detection;



class BeamFiguer::BeamFiguerImpl
{
	irrExt::IrrExtention& m_client;
	boost::shared_ptr<NodeUpdateTask> m_wispTask;
	wisp::ObjectID m_nodeId;
public:
	BeamFiguerImpl(
			std::string const& texPath,
			irrExt::IrrExtention& c,
			boost::shared_ptr<NodeUpdateTask> const& wispTask) :
		m_client(c),
		m_wispTask(wispTask)
	{

		m_nodeId = m_client.createObject(
				wisp::CreateFuncType([&texPath](boost::uuids::uuid id,
						irr::IrrlichtDevice* irr,
						wisp::IrrObjectStore& store){

					auto* mgr = irr->getSceneManager();
					//TODO : LineBillboardˆÚA‚Ü‚¾
					/*
						auto* node = mgr->addBillboardSceneNode(0,irr::core::dimension2df(1.0f,1.0f));
						store.store(id, node, 0);

						node->setScale(irr::core::vector3df(3.f,3.f,3.f));
						irr::video::SMaterial m = node->getMaterial(0);

					//ƒ}ƒeƒŠƒAƒ‹Ý’è
					m.TextureLayer[0].Texture = irr->getVideoDriver()->getTexture(texPath.c_str());
					m.MaterialType = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL;
					m.Lighting = false;*/
					}));


	}


	void setTransform(btTransform const& t)
	{
		assert(m_nodeId);
		m_wispTask->add(*m_nodeId, t);

	}

};


BeamFiguer::BeamFiguer(std::string const& tex,
		irrExt::IrrExtention& c,
		boost::shared_ptr<NodeUpdateTask> const& task) :
	m_impl(new BeamFiguerImpl(tex,c, task))
{}

BeamFiguer::~BeamFiguer(){ }

void BeamFiguer::getWorldTransform(btTransform& t) const
{
	t = btTransform::getIdentity();
}
void BeamFiguer::setWorldTransform(btTransform const& t)
{
	m_impl->setTransform(t);
}
