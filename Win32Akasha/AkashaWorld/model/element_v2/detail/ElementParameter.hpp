#ifndef AKASHA_MODEL_ELEMENT_ELEMENTPARAMETOR_HPP_INCLUDED
#define AKASHA_MODEL_ELEMENT_ELEMENTPARAMETOR_HPP_INCLUDED

#include "../../energy/EretnicSocket.hpp"
#include "../CreateInfo.hpp"
#include "../../element/ElementData.hpp"
#include "../../../../AkashaSystem/AkashaSystem.hpp"
namespace akasha
{
namespace model
{
namespace element_v2
{
//fwd
class AdapterParameter;

struct SocketInfo
{
	bool energy_ = true;
	bool electronic_ = false;
	bool traction_ = false;
	bool air_ = false;
};
class ElementParameter : private boost::noncopyable
{
	int m_typeId;
	int m_id;
	int m_parentId;
	std::string m_nameString;
	float m_damage;
	float m_energyLevel = 0.f;

	element::parameter::elementParameter m_parametors;
	element_v2::ConnectType m_connectDirect;
	bool m_connectStatus;

	btTransform m_transform;
	btVector3 m_velocity;
	btVector3 m_anglerVelocity;
	float m_invMass;

	boost::shared_ptr<AdapterParameter> m_adapterParam;
	std::vector<boost::weak_ptr<energy::EretnicSocket>> m_electricSockets;
	SocketInfo m_upperConnection;
	SocketInfo m_lowerConnection;
	SocketInfo m_innerConnection;

public:
	void init(int typeID,bool isAdapter, CreateInfo const&);

	int id() const { return m_id; }
	void id(int i){ m_id = i;}
	int parentId() const{return m_parentId; }
	void parentId(int id){ m_parentId = id; }
	int getTypeId() const{ return m_typeId; }
	void setConnectStatus(bool f){ m_connectStatus = f;}
	bool isConnect() const{ return m_connectStatus; }

	element_v2::ConnectType const&
		getDirectType() const {return m_connectDirect;}

	boost::shared_ptr<AdapterParameter> const&
		getAdapterParameter() const
		{
			return m_adapterParam;
		}
	bool isAdapter() const
	{
		return m_adapterParam.get();
	}

	float damage() const{return m_damage; }
	void damage(float d) { m_damage = d; }

	//SokectInfo
	SocketInfo const&
		getUpperConnectionInfo() const
		{
			return m_upperConnection;
		}
	SocketInfo const&
		getLowerConnectionInfo() const
		{
			return m_lowerConnection;
		}
	SocketInfo const&
		getInnerConnectionInfo() const
		{
			return m_innerConnection;
		}
	SocketInfo&
		getUpperConnectionInfo()
		{
			return m_upperConnection;
		}
	SocketInfo&
		getLowerConnectionInfo()
		{
			return m_lowerConnection;
		}
	SocketInfo&
		getInnerConnectionInfo()
		{
			return m_innerConnection;
		}

	float energyLevel() const{return m_energyLevel;}
	void energyLevel(float e){ m_energyLevel = e;}
	std::string const& getNameString() const{ return m_nameString; }

	btTransform const& getWorldTransform() const
	{
		return m_transform;
	}
	void setWorldTransform(btTransform const& s)
	{
		m_transform = s;
	}
	btVector3 const& getVelocity() const
	{
		return m_velocity;
	}
	void setVelocity(btVector3 const& v)
	{
		m_velocity = v;
	}
	btVector3 const& getAngularVelocity() const
	{
		return m_anglerVelocity;
	}
	void setAngularVelocity(btVector3 const& a)
	{
		m_anglerVelocity = a;
	}
	void setInvMass(btScalar const m)
	{
		m_invMass = m;
	}
	float getInvMass() const
	{
		return m_invMass;
	}

	element::parameter::elementParameter const&
		getAttribute() const
		{
			return m_parametors;
		}
	element::parameter::elementParameter&
		getAttribute()
		{
			return m_parametors;
		}
	//TODO : depc
	element::parameter::elementParameter const&
		getParameter() const
		{
			return m_parametors;
		}
	//TODO : depc
	element::parameter::elementParameter&
		getParameter()
		{
			return m_parametors;
		}

	std::vector<boost::weak_ptr<energy::ElectricSocket>> const&
		getEretnicSockets() const
		{
			return m_electricSockets;
		}
	boost::shared_ptr<energy::ElectricSocket>
		addElectricSocket()
		{
			auto s = boost::make_shared<energy::ElectricSocket>();
			m_electricSockets.emplace_back( s );
			return s;
		}
	template<typename T>
		boost::optional<T>
		getProperty( const std::string& name, const boost::type<T>& ) const
		{
			//property_treeの取得
			auto pathRoot = system::GetSettingTree("MIZUNUKI.Physics.Model.Elements");
			assert(pathRoot);
			for (auto const& d : *pathRoot)
			{
				if ( d.first == "element" )
					if ( auto t = d.second.get_optional<int>( "<xmlattr>.type" ) )
						if ( ( *t ) == m_typeId)
							return d.second.get_optional<T>( name );
			}

			return boost::none;
		}
};

template<typename PrameType>
void get_Parameter(ElementParameter const&,boost::type<PrameType> const&){}
template<typename PrameType>
void set_Parameter(ElementParameter&, typename PrameType::holderType,boost::type<PrameType> const&){ }
}
}
}

#endif // AKASHA_MODEL_ELEMENT_ELEMENTPARAMETOR_HPP_INCLUDED
