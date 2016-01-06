#include "SmorkEffect.hpp"
#include <irrlicht/IDummyTransformationSceneNode.h>

#include <algorithm>

#if 0
#include <irrlicht/IGUIEnvironment.h>
#include <irrlicht/IGUIFont.h>
#endif //0

using akasha::effect::SmorkEffectManager;


//fwd
void SmorkParticleSetup(
		boost::uuids::uuid id,
		irr::IrrlichtDevice* irr,
		wisp::IrrObjectStore& store,
		boost::filesystem::path texPath,
		std::size_t count
		);
void SmorkEffectManager::setup(
		irrExt::IrrExtention& irrEx)
{
	auto const& sysConst = akasha::system::AkashaSystem::Instance().getConstant();
	boost::filesystem::path texPath(
			sysConst.getCurrentDirectoryPath() / "Resources/effect/dust.png" );

	m_paricleSystem = irrEx.createObject(
			boost::bind(SmorkParticleSetup, _1,_2,_3, texPath, 16));

	m_particleTask = irrEx.createTask(
			boost::bind(SmorkEffectManager::WorkFunc, _1,_2, *m_paricleSystem),
			boost::type<workArg_t>());

}

void SmorkEffectManager::update( const float step)
{

	// Empty command post need

	for (auto& c : m_commands)
		c.step_ = step;
	m_particleTask->swapPost(m_commands);
	m_commands.clear();

}

void SmorkEffectManager::WorkFunc(
		workArg_t const& arg, irrExt::TaskArg const& exArg, boost::uuids::uuid const id)
{
	auto* rootNode = exArg.store_.find( id );
	assert(rootNode);

	auto& childNode =  rootNode->getChildren();
	auto endIter = childNode.end();
	auto const argEnd = arg.cend();
	auto argIter = arg.cbegin();

	irr::scene::IParticleSystemSceneNode* sys;
	for (auto i=childNode.begin(); i != endIter; ++i)
	{
		sys = static_cast<irr::scene::IParticleSystemSceneNode*>(*i);
		auto* em = sys->getEmitter();
		if (argIter == argEnd)
		{
			em->setMaxParticlesPerSecond( 0 );
			em->setMinParticlesPerSecond( 0 );
		}
		else
		{
			auto const& p = *(argIter++);
			// 			float const step = p.step_;
			sys->setPosition(p.pos_);

			// 			em->setDirection( p.dir_ * step * 0.01f);
			std::size_t w( 30*p.weight_);
			em->setMaxLifeTime( w + 20);
			em->setMinLifeTime( w - 20 );

			float const size(  p.weight_ * 0.1f);
			irr::core::dimension2df const sized(size,size);
			em->setMaxStartSize( sized );
			em->setMinStartSize( sized );

			//TODO : special renderer need.
			// 			int cbase( (p.weight_-1.f) * 0.2f * 255.f);
			// 			irr::video::SColor const c( 128,std::min(cbase, 255), 255,255);
			// 			em->setMaxStartColor(c);
			// 			em->setMinStartColor(c);

			em->setMaxParticlesPerSecond( 10 );
			em->setMinParticlesPerSecond( 6 );
		}
	}

#if 0
	{//Debug
		auto& irr = systemArg.irr_;
		irr.getGUIEnvironment()->getBuiltInFont()->draw(
				(std::wstring(L"order count: ") + std::to_wstring(arg.size())).c_str(),
				irr::core::recti(0,0,0,0),irr::video::SColor(255,255,255,255));    // 文字を表示
	}
#endif
}
//////////////////////////////////////////////////////////////
void SmorkParticleSetup(
		boost::uuids::uuid id,
		irr::IrrlichtDevice* irr,
		wisp::IrrObjectStore& store,
		boost::filesystem::path const texPath,
		std::size_t const count
		)
{
	wisp::WorkingDir workDir(irr);

	auto* smgr = irr->getSceneManager();
	auto* rootNode = smgr->addDummyTransformationSceneNode();
	rootNode->setVisible(true);
	store.store( id, rootNode, 0);

	namespace core = irr::core;
	namespace video = irr::video;

	for (std::size_t i=0; i<count; ++i)
	{
		auto* node = smgr->addParticleSystemSceneNode(false,rootNode);
		irr::video::IVideoDriver* d = node->getSceneManager()->getVideoDriver();
		auto& mat = node->getMaterial(0);
		mat.setTexture(0,d->getTexture(texPath.c_str()));
		mat.setFlag(video::E_MATERIAL_FLAG::EMF_LIGHTING, false);
		node->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);

		//emmiter
		{
			const irr::f32 halfSize =  0.2f;
			auto* em = node->createBoxEmitter(
					core::aabbox3d<irr::f32>(
						-halfSize, -halfSize, -halfSize, halfSize, halfSize, halfSize), // emitter size
					core::vector3df(0.0f,0.0f,0.0f),	// initial direction
					0,0,									  // emit rate
					video::SColor(0,255,255,255),		 // darkest color
					video::SColor(0,255,255,255),		 // brightest color
					2,100,5,								 // min and max age, angle
					core::dimension2df(0.1f,0.1f),			// min size
					core::dimension2df(0.5f,0.5f)	  // max size
					);
			node->setEmitter(em);
			em->drop();
		}

		//アフェクタ
		auto* g = node->createGravityAffector(
				core::vector3df(0.f, 0.01f, 0.f), 1000);
		auto* sa = node->createScaleParticleAffector(core::dimension2df(0.1f,0.1f));
		node->addAffector(g);
		node->addAffector(sa);
		sa->drop();
		g->drop();
	}
}
