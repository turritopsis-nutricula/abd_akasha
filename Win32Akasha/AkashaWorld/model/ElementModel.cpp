/*
 * ElementModel.cpp
 *
 *  Created on: 2012/02/24
 *      Author: ely
 */

#include "energy/EretnicManager.hpp"
#include "ModelLuaEnvironment.hpp"
#include "bridgeLink/BridgeLink.hpp"
//#include "../entity/EntitySkeleton.hpp"

//std
#include <algorithm>



#include <wisp_v3/wisp.hpp>

#include "../../akashaApplication/lua/LuaEnvironment.hpp"
#include "../../akashaApplication/lua/luabindExConverter.hpp"
#include "luabind/luabind.hpp"
//#include "../entity/EntityJoint.hpp"

#include "ModelConnectGraph.hpp"
#include "EnergyLinkGraph.hpp"

#include "ModelValue.hpp"
#include "ModelControlKey.hpp"

#include "element_v2/ElementBuffer.hpp"
#include "builder/ElementChunkBuilder.hpp"
#include "builder/ControlKeyBuilder.hpp"
#include "builder/UserValueBuilder.hpp"

//TODO : element/以下は整理予定
#include "element/energy/GearAssemblyBuilder.hpp"

#include "element/energy/Harnnes.hpp"
#include "element/energy/TractionLink.hpp"

#include "ValueManifold.hpp"



#include "ModelLuaConstract.hpp"

#include "ElementModel.hpp"

#include "ModelObserver.hpp"

#include "energy/EnergyManager.hpp"

#include "../../AkashaApplication/SoundManager.hpp"

#include <wisp_v3/irrExt.hpp>

//boost
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/if.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/exception/all.hpp>

#include <boost/fusion/include/at_c.hpp>
#include<boost/range/numeric.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <boost/functional/hash.hpp>

#include <boost/graph/undirected_dfs.hpp>




namespace akasha
{
namespace exception
{
struct LuaException : boost::exception, public std::exception {};
}
}
using namespace akasha;
using namespace akasha::model;

using akasha::model::ElementModel;


struct ElementModel::Impl
{
public:
	using KeyControlType = key::ModelControlKey<float, std::string>;
	using KeyControlBuffer = boost::ptr_vector<KeyControlType>;

private:
	irrExt::IrrExtention& m_irrExtention;
	//entity::EntityService& m_entityService;
	wisp::v3::WispHandle& m_wispClient;
	boost::shared_ptr<akasha::NodeUpdateTask> m_irrNodeUpdate;

	ElementModel::ModelType m_type;

	ModelConnectGraph m_connectGraph;

	value::ModelValueBuffer m_modelValues;
	KeyControlBuffer m_controlKeys;

	element_v2::ElementBuffer m_elementBuffer1;
	std::vector<boost::shared_ptr<element_v2::ElementParameter>> m_elementParamList;
	boost::shared_ptr<ModelObserver> m_modelObserver;

	std::vector<ValueManifoldConcept> m_valueManifold1;

	std::vector<bridgeLink::BridgeLink> m_bridgeLinks;

	energy::EnergyManager m_energyManager;

	energy::GearAssimbly m_gearAssimblys;
	std::vector<energy::GearAssimbly> m_gearAssimbly;

	energy::EretnicManager m_eretnicManager;

	// btPhyciscBody vs ElementId map
	boost::unordered_map<void const*, std::size_t> m_btObjectToIdMap;

	// Model Life time
	std::size_t m_ticks;

	// Model Hash Number
	std::size_t m_modelHash;

	lua::ModelLuaEnvironment m_modelLua;  //lua実行環境
	std::wstring m_luaErrorMessage;  // luaのエラー文

	// 		boost::shared_ptr<cricket::AudioVoice> m_crashSound;

public:
	Impl(
			irrExt::IrrExtention& irrEx,
			wisp::v3::WispHandle& c,
			boost::shared_ptr<akasha::NodeUpdateTask> task,
			boost::shared_ptr<land::EnvironmentLand> land ) :
		m_irrExtention( irrEx ),
		m_wispClient( c ),
		m_type(ElementModel::ModelType::undefine),
		m_modelHash( 0 ),
		m_modelLua( irrEx,c, land )
	{

		{
			// 			auto& soundManager = cricket::CricketManager::Instance();
			//auto crashSound = soundManager.createSinWave(440.f, 1);
			boost::filesystem::path p( system::AkashaSystem::Instance().getConstant().getCurrentDirectoryPath() / "resources/sound/break.wav" );
			// 			auto crashSound = soundManager.createWave( p );
			// 			m_crashSound = soundManager.createVoice( crashSound );
			m_irrNodeUpdate = task;
			m_elementBuffer1.setIrrNodeTask(task);
		}
	}

	~Impl()
	{
		for(auto& e : m_elementBuffer1.getList())
			e.constraintRelease();
	}

	ElementModel::ModelType getType() const
	{
		return m_type;
	}

	//! ModelConstract by file
	bool constract(
			const loader::ModelDefine<std::string>& data,
			ElementModel::ModelType type,
			boost::function<void (bool)>&& simulationHandle)
	{
		m_type = type;
		m_ticks  = 0;
		bool result = false;

		using string_type = std::string;
		using ValueType = loader::ModelValueDefine<string_type>;

		//ModelValue
		const std::vector<ValueType>& values = data.values_;
		builder::UserValueBuilder<string_type> vb;
		result = vb.build( values, m_modelValues );

		//ContralKey
		builder::ControlKeyBuilder<float, string_type> kb( boost::ref( m_modelValues ) );
		using KeyType = loader::ModelKeyDefine< string_type >;
		const std::vector<KeyType>& keyes = data.keys_;
		result = kb.build( keyes, m_controlKeys );

		//Element
		using ElementType = loader::ModelElementDefine<string_type>;
		const std::vector<ElementType>& elements = data.elements_;
		{
			builder::BtShapeBuilder shapes;

			builder::ElementChunkBuilder eb(
					m_irrExtention,m_wispClient , shapes, getType());
			eb.build(m_elementBuffer1, m_valueManifold1, m_modelValues, elements);

			if ( !result ) return false;

			m_modelObserver = m_elementBuffer1.makeStatusBuffer();
		}

		//ValueManifold Sync
		for (ValueManifoldConcept& m : m_valueManifold1) {
			m.sync(m_elementBuffer1.getList());
		}


		//lua constract
		const string_type& luaCode = data.luaCode_;
		try
		{
			if ( luaCode.size() > 0)
			{
				bool const load_success = m_modelLua.setup(
						luaCode,
						m_modelObserver,
						data.modelFileFullPath_,
						std::move(simulationHandle));

				if (!load_success)
				{
					const char* mes =
						lua_tostring( m_modelLua.getLuaEnviroment().getState() , -1 );
					assert( mes );
					m_luaErrorMessage = convertType<std::wstring>( std::string( mes ) );
					akasha::system::AkashaSystem::Instance().getLogger()( akasha::system::log::ModelInfo {"Lua error"} );
				}
			}

		}
		catch( std::runtime_error& )
		{
			BOOST_THROW_EXCEPTION( exception::LuaException() );
		}

		// ElementParameterList
		m_elementParamList.clear();
		for (auto const& p : m_elementBuffer1.getList())
			m_elementParamList.push_back(p.getParameterPtr());

		//Element connection graph
		{
			std::vector<std::pair<int, int>> buf;
			buf.reserve( m_elementBuffer1.getList().size() );
			for (auto const& p : m_elementParamList )
				buf.emplace_back( p->id(), p->parentId() );

			m_connectGraph.constract( buf );
		}
		//Init ConceptList
		std::vector<element_v2::ElementInitConcept> initList =
			m_elementBuffer1.getInitConceptList();

		{
			energy::ConductorBuilder condBuilder;
			// EnergyPort Init
			for(auto& e : initList)
				e.initEnergyPhase1(condBuilder);

			condBuilder.dispatchRequest( m_connectGraph );

			for(auto& e : initList)
				e.initEnergyPhase2(condBuilder);

			for(auto& e : initList)
				e.initEnergyPhase3(condBuilder);

			condBuilder.setupEnergyManager(m_energyManager);
		}

		// element connection
		std::vector<int> toRootBuf;
		std::vector<boost::shared_ptr<element_v2::ElementParameter>> paramsToNonAdapter;
		for(auto& e : initList)
		{
			paramsToNonAdapter.clear();
			toRootBuf.clear();
			m_connectGraph.getParentsIds(e.getId(), toRootBuf);

			auto nonAdapterIter = boost::find_if(toRootBuf,
					[&initList,&paramsToNonAdapter,this](int n){
					return !(initList.at(n).isAdapter());
					});
			if ( nonAdapterIter != toRootBuf.end())
			{
				std::for_each(toRootBuf.begin(),nonAdapterIter+1,
						[&paramsToNonAdapter,this](int n) {
						paramsToNonAdapter.push_back(
								m_elementBuffer1.getList().at(n).getParameterPtr());
						});
				auto b =  initList.at(*nonAdapterIter).getPhysicsBody();
				assert(b && "ElementMode#constract physicsBody null");
				e.initConnect(b, paramsToNonAdapter);
			}
		}

		//alengment
		for (auto& e : initList) {
			toRootBuf.clear();
			m_connectGraph.getParentsIds(e.getId(), toRootBuf);

			auto i = boost::find_if(toRootBuf,
					[&initList](int n){ return !(initList.at(n).isAdapter()); });
			if ( i != toRootBuf.end() )
				e.initAlignment(
						getElement( *i ).getParameterPtr()->getWorldTransform());
		}

		// create m_btObjectToIdMap
		for (auto& e : initList)
			m_btObjectToIdMap[ e.getPhysicsBody().get() ] = e.getId();

		//TODO : welding timming

		// BridgeLink
		m_bridgeLinks.clear();
		for (auto& e : m_elementBuffer1.getList())
		{
			auto const& att = e.getParameterPtr()->getAttribute();
			int const l = att.getValue(boost::type<element::parameter::link>());
			if (l>=0 && l < (int)m_elementBuffer1.getList().size() && l != e.getId())
			{
				m_bridgeLinks.emplace_back();
				auto& link = m_bridgeLinks.back();
				link.init(
						e.getPhysicsBody(),
						getElement(l).getPhysicsBody(),
						m_irrExtention,
						m_irrNodeUpdate
						);
			}
		}

		//Eretnic
		{
			std::vector<int> buf;
			m_eretnicManager.clear();
			for (auto const& e : m_elementBuffer1.getList())
			{
				auto const& param = e.getParameterPtr();
				if (param->getTypeId() !=
						element_v2::TypeToId<element_v2::BatteryElement>::value)
					continue;

				buf.clear();
				m_connectGraph.getAllChildrenIds(param->id(), buf);
				m_eretnicManager.add(buf);
			}
		}
		//Energy link graph TODO

		// model hash
		calcModelHash();

		// update observer
		m_modelObserver->update(
				m_ticks, m_modelValues );


		return result;
	}

	//! Init Handle
	void onInit()
	{
		m_modelLua.onInit( m_modelValues );
	}

	//! Model hash
	std::size_t getModelHash() const
	{
		return m_modelHash;
	}

	//! All element count
	unsigned int
		getElementCount() const
		{
			return m_elementBuffer1.getList().size();
		}
	//! ElementConcept
	element_v2::ElementConcept&
		getElement( unsigned int id )
		{
			assert( id < getElementCount() );
			return m_elementBuffer1.getList()[id];
		}
	const element_v2::ElementConcept&
		getElement( unsigned int id ) const
		{
			assert( id < getElementCount());
			return m_elementBuffer1.getList()[id];
		}



	btTransform const&
		getElementTransform( const int n ) const
		{
			return getElement(n).getParameterPtr()->getWorldTransform();
		}

	//! GravityCenter
	boost::optional<btVector3> gravityCenter( std::size_t id ) const
	{
		return getGravityCenter( id );
	}
	// DEPLOY
	boost::optional<btVector3>
		getGravityCenter( int id = 0 ) const
		{
			std::vector<int> childBuffer;
			m_connectGraph.getAllChildrenIds(
					m_connectGraph.getRootID( id ),
					childBuffer );

			float const massSum =
				boost::accumulate( childBuffer,
						0.f,
						[this](float sum, int const id){
						return sum +
						(1.f/getElement(id).getParameterPtr()->getInvMass());
						});
			btVector3 const momentSum =
				boost::accumulate( childBuffer,
						btVector3(btScalar(0),btScalar(0),btScalar(0)),
						[this](btVector3 const& s,int const id){
						auto const& p = getElement(id).getParameterPtr();
						return s + (
								(1.f/p->getInvMass()) * p->getWorldTransform().getOrigin());
						});

			if ( massSum )
				return momentSum / massSum;
			else
				return boost::none;
		}

	//! Parent element ID by Nth element
	// Root element return -1
	int getParentElementID( const unsigned int n ) const
	{
		return getElement( n).getParameterPtr()->isConnect() ?
			(int)getElement( n ).getParentId() : -1;
	}

	//! Top element ID by Nth element
	int getTopElementID( const unsigned int n ) const
	{
		return m_connectGraph.getRootID( ( unsigned int )n );
	}



	//! Contact event map
	ElementModel::contactEventMapType contactGroupMap() const
	{
		ElementModel::contactEventMapType buf;
		buf.reserve( m_elementBuffer1.getCount() );

		for (auto const& e : m_elementBuffer1.getList())
		{
			if (auto t = e.getContactEventType())
				buf.push_back(std::make_pair( *t, e.getPhysicsBody() ));
		}

		return std::move(buf);
	}

	//Contact Landform
	void contactToLandForm(btManifoldPoint const& pt, void const* btObjectPtr)
	{
		// 				std::cout << "contact:" << getLifeTicks() << " id:" << m_btObjectToIdMap[btObjectPtr] <<
		// 					" f=" << pt.getAppliedImpulse() <<
		// 					" d=" << pt.getDistance() << std::endl;
	}

	//! Model Warp
	void modelWrap( btTransform const& t )
	{
		//一緒に移動してしまう系もあるので一度bufる
		btTransform const centerInv =
			getElement(0).getParameterPtr()->getWorldTransform().inverse();
		std::vector<btTransform> buf;
		buf.reserve( m_elementBuffer1.getList().size() );

		for(auto const& e : m_elementBuffer1.getList())
			buf.push_back( centerInv *
					e.getParameterPtr()->getWorldTransform());

		std::vector<element_v2::ElementManipulateConcept> manipuList =
			m_elementBuffer1.getManipulateConceptList();
		for (auto& e : manipuList | boost::adaptors::reversed)
		{
			e.setTransform( t* buf.back());
			buf.pop_back();
		}

		//bridge
		buf.clear();
		for (auto const& l : m_bridgeLinks)
			buf.push_back( centerInv * l.getBody()->getWorldTransform());
		for (auto& l : m_bridgeLinks | boost::adaptors::reversed)
		{
			l.getBody()->setWorldTransform( t*buf.back());
			buf.pop_back();
		}

	}
	void drawExternForce() const
	{

		//TODO : 要再実装

	}
	void changeEnvironmentLand(
			boost::shared_ptr<land::EnvironmentLand> l )
	{
		m_modelLua.changeEnvironmentLand( l );
	}
	/// life frameticks
	std::size_t getLifeTicks() const
	{
		return m_ticks;
	}
	/// Sumiration step update
	void update( const btScalar fps, const btScalar sub, bool const isSimulationRunning )
	{

		if(ElementModel::ModelType::main==getType())
		{
			//update key input
			KeyControlBuffer::iterator e = m_controlKeys.end();
			for ( KeyControlBuffer::iterator i = m_controlKeys.begin(); i != e;
					i++ )
				( *i ).update( true );
		}

		//Model value update
		value::ModelValueBuffer::iterator e = m_modelValues.end();
		for ( value::ModelValueBuffer::iterator i = m_modelValues.begin(); i != e;
				i++ )
			( *i )();


		//Value manifold update
		for (ValueManifoldConcept& m : m_valueManifold1)
			m.sync(m_elementBuffer1.getList());

		// Step notify
		for (auto& e : m_elementBuffer1.getList())
			e.stepStart( fps, sub, m_energyManager);

		// element connection info update
		std::vector<std::pair<int, bool>> connectBuf;
		connectBuf.reserve( m_elementParamList.size() );
		for (auto const& p : m_elementParamList )
			connectBuf.emplace_back(p-> id(), p->isConnect());

		std::vector<int> const disconnectBuffer =
			m_connectGraph.update( connectBuf );

		if ( disconnectBuffer.size() > 0 )
		{
			//TODO : under constraction
			m_energyManager.disconnect();
			//TODO : Break Sound
			// 				auto const& e = getElement( ( unsigned int )disconnectBuffer[0] );
		}
	}

	/// Sumiration stepend
	void stepEndUpdate( btScalar const fps, btScalar const sub, bool const isSimulationRunning)
	{
		for (auto& e : m_elementBuffer1.getList())
			e.stepEnd(m_energyManager);

		// Gear assembry update
		for (auto& g : m_gearAssimbly)
			g.update( 1.0 /60.0);

		// observer update
		m_modelObserver->update(
				m_ticks, m_modelValues );

		// lua onframe
		m_modelLua.onFrame( m_modelValues );

		// frame ticks countup
		if (isSimulationRunning)
			++m_ticks;
	}

	/// SubStep
	void subStep( const btScalar step )
	{
		for (auto& e : m_elementBuffer1.getList())
			e.preTick( step, m_energyManager );
	}
	void subStepEnd( const btScalar step )
	{
		// Energy update
		m_energyManager.synchronizeUpdate();

		for (auto& e : m_elementBuffer1.getList())
			e.tick( step, m_energyManager );
	}
	/// ModelvalueBuffer
	const value::ModelValueBuffer&
		getValueBuffer() const
		{
			return m_modelValues;
		}

	const std::wstring&
		getLuaErrorMessage() const
		{
			return m_luaErrorMessage;
		}
	void getNetworkBase( network::OtherModelBase<std::string>& d ) const
	{
		d.valueNames_.clear();
		boost::for_each( m_modelValues, [&d]( value::ModelValueType const & v )
				{

				d.valueNames_.push_back( v.getName() );
				} );

		d.elements_.clear();
		for (auto const& e : m_elementBuffer1.getList())
		{
			typename network::OtherModelBase<std::string>::element
				ed {e.getId(), ( int )e.getParentId(), e.getTypeId()};
			d.elements_.push_back( ed );
		}
	}

	void getNetworkUpdata( network::OtherModelUpdataer& d ) const
	{
		d.modelHash_ = getModelHash();
		boost::for_each( m_modelValues, [&]( value::ModelValueType const & v )
				{
				d.values_.push_back( v.getVolum() );
				} );
		for (auto const& e : m_elementBuffer1.getList())
		{
			btQuaternion q;
			btTransform const& t = e.getParameterPtr()->getWorldTransform();
			t.getBasis().getRotation( q );
			d.transform_.emplace_back( t.getOrigin(), std::move( q ) );
		}
		//         boost::for_each( m_elementList, [&]( element::ElementWrapper const & e )
		//         {
		//             btQuaternion q;
		//             btTransform const& t = e.getTransform();
		//             t.getBasis().getRotation( q );
		//             d.transform_.emplace_back( t.getOrigin(), std::move( q ) );
		//
		//         } );

	}

	boost::shared_ptr<ModelObserver> const&
		getObserver() const
		{
			return m_modelObserver;
		}
private:

	void calcModelHash()
	{
		//TODO: No Fix parameter calculate
		m_modelHash = 0;
		BOOST_FOREACH( value::ModelValueType const & v, m_modelValues )
		{
			boost::hash_combine( m_modelHash, v.getName() );
		}

		//TODO: underConstraction
		// 		for (auto const& e : m_elementBuffer1.getList())
		// 			boost::hash_combine( m_modelHash, e );

		// 				BOOST_FOREACH( ValueManifold const & m, m_valueManifold )
		// 				{
		// 					boost::hash_combine( m_modelHash, m );
		// 				}
	}

};


///////////////////////////////////////////////////////////////////
// ElementModel

ElementModel::ElementModel(
		irrExt::IrrExtention& irr,
		wisp::v3::WispHandle& c,
		boost::shared_ptr<akasha::NodeUpdateTask> task,
		boost::shared_ptr<land::EnvironmentLand> land ) :
	m_impl( new ElementModel::Impl(irr, c,task, land ) )
{

}
ElementModel::~ElementModel()
{
}

bool ElementModel::constract(
		const loader::ModelDefine<std::string>& data,
		ModelType type,
		boost::function<void (bool)>&& simulationHandle)
{
	return m_impl->constract( data, type, std::move(simulationHandle) );
}


void ElementModel::onInit()
{
	m_impl->onInit();
}



btTransform const&
ElementModel::getElementTransform( const int n ) const
{
	return m_impl->getElementTransform( n );
}

/// Element Mass by Nth element
btScalar ElementModel::getElementMass( const int n ) const
{
	float inv = m_impl->getElement( n ).getParameterPtr()->getInvMass();
	if (inv != 0.f)
		return 1.f/inv;
	else
		return 0.f;
}

int ElementModel::getParentElementID( const unsigned int n ) const
{
	return m_impl->getParentElementID( n );
}

///
int ElementModel::getTopElementID( const unsigned int n ) const
{
	return m_impl->getTopElementID( n );
}

boost::optional<btVector3>
ElementModel::gravityCenter( unsigned int id /*= 0*/ ) const
{
	return m_impl->gravityCenter( id );
}
boost::optional<btVector3>
ElementModel::getGravityCenter( unsigned int id /*= 0*/ ) const
{
	return m_impl->gravityCenter( id );
}

//! ContactEventMap
ElementModel::contactEventMapType
ElementModel::createContactGroupMap() const
{
	return m_impl->contactGroupMap();
}

// ContactModel
void ElementModel::contactToLandForm(btManifoldPoint const& pt, void const* btObject)
{
	return m_impl->contactToLandForm(pt, btObject);
}

////////////////
//! モデルを移動
void ElementModel::modelWrap( btTransform const& t )
{
	m_impl->modelWrap( t );
}
void ElementModel::drawExternForce() const
{
	m_impl->drawExternForce();
}

void ElementModel::changeEnvironmentLand(
		boost::shared_ptr<land::EnvironmentLand> l )
{
	m_impl->changeEnvironmentLand( l );
}

std::size_t ElementModel::getLifeTicks() const
{
	return m_impl->getLifeTicks();
}

boost::shared_ptr<ModelObserver> const&
ElementModel::getObserver() const
{
	return m_impl->getObserver();
}

////////////////
//! 更新
void ElementModel::update(
		const btScalar fps, const btScalar sub, bool const isSimulationRunning )
{

	m_impl->update( fps, sub, isSimulationRunning );
}

void ElementModel::stepEndUpdate( const btScalar fps, const btScalar sub, bool const isSimulationRunning )
{
	m_impl->stepEndUpdate( fps, sub, isSimulationRunning );
}
////////////////
//内部ステップ
void ElementModel::subStep( const btScalar step )
{
	m_impl->subStep( step );
}
void ElementModel::subStepEnd( const btScalar step )
{
	m_impl->subStepEnd( step );
}

const std::wstring&
ElementModel::getLuaErrorMesage() const
{
	return m_impl->getLuaErrorMessage();
}


// network
void ElementModel::getNetworkBase( network::OtherModelBase<std::string>& d ) const
{
	m_impl->getNetworkBase( d );
}
void ElementModel::getNetworkUpdata( network::OtherModelUpdataer& d ) const
{
	m_impl->getNetworkUpdata( d );
}
