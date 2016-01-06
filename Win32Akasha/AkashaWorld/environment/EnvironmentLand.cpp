#include "EnvironmentLand.hpp"

//irr
#include "vector3d.h"
#include "ILightSceneNode.h"

//boost
#include <boost/utility/in_place_factory.hpp>

using akasha::land::EnvironmentLand;

// Load Land
bool EnvironmentLand::loadFromFile(irr::io::path const& p)
{
	m_water.setup(m_irrEx, m_client);

	//form
	m_form = boost::in_place();
	//地形メッシュをロード
	if (!m_form->loadFromFile(p, m_irrEx, m_client))
		return false;



	irr::video::SLight lightData;
	lightData.Type = irr::video::E_LIGHT_TYPE::ELT_POINT;
	lightData.Radius = FLT_MAX;
	lightData.AmbientColor = irr::video::SColorf(0.21f,0.21f,0.21f);
	lightData.DiffuseColor = irr::video::SColorf(0.8f,0.8f,0.8f);


	m_lightID = m_irrEx.createObject(
			wisp::CreateFuncType([lightData](
					boost::uuids::uuid id,
					irr::IrrlichtDevice* irr,
					wisp::IrrObjectStore& store
					){
				auto* mgr = irr->getSceneManager();
				auto* l = mgr->addLightSceneNode(0,
					irr::core::vector3df(1.f, 10.f, 3.f).normalize() * 10e-8f
					);
				store.store(id, l, 0);
				l->setLightData(lightData);
				l->setPosition(irr::core::vector3df(0,10e+7F,0));
				l->setRotation( irr::core::vector3df(80.f,0.f,0.f));
				}));

	using boost::filesystem::path;
	path current(system::AkashaSystem::Instance().getConstant().getCurrentDirectoryPath() /"Resources/effect" );
	m_sky = m_irrEx.createObject(
			wisp::CreateFuncType([current](
					boost::uuids::uuid id,
					irr::IrrlichtDevice* irr,
					wisp::IrrObjectStore& store){

				boost::array<path,6> list = {
				{"skybox_top.bmp"   ,
				"skybox_bottom.bmp",
				"skybox_east.bmp"  ,
				"skybox_west.bmp"  ,
				"skybox_south.bmp" ,
				"skybox_north.bmp"}
				};

				auto* v = irr->getVideoDriver();
				boost::array<irr::video::ITexture*, 6> tex;

				size_t i=0;
				BOOST_FOREACH(auto& p, list){
				tex[i] = v->getTexture((current / p).c_str());
				++i;
				}

				store.store(id,
						irr->getSceneManager()->addSkyBoxSceneNode(
							tex[0],tex[1],tex[2],tex[3],tex[4],tex[5]
							),0);
			}));



	return true;
}

//Land MaxHeight by (X,Z)
boost::optional<btScalar> EnvironmentLand::getLandHeight(
		btScalar const x,
		btScalar const z
		) const
{
	const btScalar WORLD_SIZE = 30000;
	const btVector3 rayFrom(x,WORLD_SIZE,z);
	const btVector3 rayTo(x,-1,z);

	btCollisionWorld::ClosestRayResultCallback cb( rayFrom, rayTo );
	cb.m_collisionFilterGroup = COL_TYPE_TOY;
	cb.m_collisionFilterMask = COL_TYPE_LAND;

	BulletManager::Instance().rayTest(rayFrom,rayTo, cb);

	if (cb.hasHit())
		return  cb.m_hitPointWorld.getY();
	else
		return boost::none;
}

//Land Height by Point
boost::optional<btScalar> EnvironmentLand::getLandHeight(
		btVector3 const& from, btScalar depth
		) const
{
	//const btScalar WORLD_SIZE = 30000;
	const btVector3 rayTo(from.getX(), from.getY()-depth ,from.getZ());
	//const btVector3 rayTo(0.f, -depth ,0.f);

	btCollisionWorld::ClosestRayResultCallback cb( from, rayTo );
	cb.m_collisionFilterGroup = COL_TYPE_TOY;
	cb.m_collisionFilterMask = COL_TYPE_LAND;

	BulletManager::Instance().rayTest(from,rayTo, cb);

	if (cb.hasHit())
		return  cb.m_hitPointWorld.getY();
	else
		return boost::none;
}

void EnvironmentLand::registContactGroup(
		contactEvent::ContactEventManager& m
		) const
{
	for (auto const& i : m_form->getContactGroupMap())
		m.registBody(i.first, i.second);
}
