#ifndef AKASHA_MODEL_ENERGYMANAGER_HPP_
#define AKASHA_MODEL_ENERGYMANAGER_HPP_

#include "../ModelConnectGraph.hpp"
#include "../element_v2/direct/ConnectType.hpp"

#include "EnergyPortMap.hpp"
#include "EnergyType.hpp"

#include <boost/range/algorithm/copy.hpp>
#include <boost/range/algorithm/find_if.hpp>

#include <boost/multi_index/member.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index_container.hpp>

namespace akasha
{
namespace model
{
namespace energy
{


class EnergyManager
{
// 	typedef boost::multi_index bmi;
	using portIDType = RawNodeID;

	// NodeID -> EnergyPort
	EnergyPortMap m_port_map;

public:
	void addPort(portIDType id)
	{
		m_port_map.regist( id.type_, id.raw_value_);
	}

	bool connectPorts(portIDType _0, portIDType _1)
	{
		return m_port_map.setPair( _0.raw_value_, _1.raw_value_ );
	}

	bool disconnectPorts(portIDType _0, portIDType _1)
	{
		return m_port_map.removePair(_0.raw_value_, _1.raw_value_);
	}

	template<EnergyType T,typename F>
		void portAccess(NodeID<T> const id, F&& f)
		{
			f(m_port_map.ref<T>(id.raw_value()));
		}
	template<EnergyType T,typename F>
		void portAccess(NodeID<T> const id, F&& f) const
		{
			f(m_port_map.const_ref<T>(id.raw_value()));
		}

	template<EnergyType T>
		typename Pair<T>::type& refNode(
				NodeID<T> const id)
		{
			return m_port_map.ref_node<T>(id.raw_value());
		}

	template<EnergyType T>
		typename Pair<T>::type const& refNode(
				NodeID<T> const id) const
		{
			return m_port_map.ref_node<T>(id.raw_value());
		}

	template<EnergyType T>
		typename Link<T>::type const& refLink(
				NodeID<T> const id) const
		{
			return m_port_map.ref_link<T>(id.raw_value());
		}

	void synchronizeUpdate()
	{
		m_port_map.update();
	}

	void disconnect(/* portIDType, portIDTYpe*/ )
	{
		std::cout << "EnergyManager disconnect() not working\n";
	}
};


////////////////////////////////////////////////
// Builder

enum class ConductDirect
{
	None,
	Parent,
	Children
};

class ConductorBuilder : boost::noncopyable
{
public:
	// Setup Dispatchers
	class RegistDispatcher : boost::noncopyable
	{
	public:
		struct registRequestType
		{
			int registID_; // elementID
			RawNodeID nodeID_;
			ConductDirect direct_;
			element_v2::ConnectType connectType_;

			registRequestType(
					int registID,
					RawNodeID id,
					ConductDirect d,
					element_v2::ConnectType c) :
				registID_(registID),
				nodeID_(id),
				direct_(d),
			connectType_(c)
			{}
		};

	private:
		ConductorBuilder& manager_;
		int curretIdNumber_;
		std::vector< registRequestType > regist_;
		int registID_;
		element_v2::ConnectType connectType_;

	public:
		RegistDispatcher() = delete;

		RegistDispatcher(
				ConductorBuilder& m,
				int portIDBase,
				int registID,
				element_v2::ConnectType connectType) :
			manager_(m),
			curretIdNumber_( portIDBase ),
			registID_(registID),
			connectType_(connectType){
			}

		//move constractor
		RegistDispatcher(RegistDispatcher&& o) :
			manager_(o.manager_),
			curretIdNumber_(o.curretIdNumber_),
			regist_(std::move(o.regist_)),
			registID_( o.registID_),
			connectType_( o.connectType_)
		{}

		~RegistDispatcher()
		{
			manager_.regist(regist_, curretIdNumber_);
		}

		// return RawNodeID
		template<EnergyType N>
		NodeID<N> regist(ConductDirect direct)
		{
			int raw_id = curretIdNumber_++;
			regist_.emplace_back(
					registID_, RawNodeID(raw_id, N), direct, connectType_);
			NodeID<N> id;
			id.raw_value_ = raw_id;
			return std::move(id);
		}
	};

	class AcceptDispatcher : boost::noncopyable
	{
	public:
		struct Request
		{
			RawNodeID nodeID_;
			RawNodeID fromPortID_;
			ConductDirect requestDirect_;
			element_v2::ConnectType fromConnectType_;
			int fromID_;
			int toID_;

			Request(
					int from_id,
					int to_id,
					RawNodeID id,
					RawNodeID fromPortID,
					ConductDirect d,
					element_v2::ConnectType c) :
				nodeID_(id),
				fromPortID_(fromPortID),
				requestDirect_(d),
				fromConnectType_(c),
				fromID_( from_id),
				toID_(from_id)
			{}
		};

	private:
		ConductorBuilder& manager_;
		int registedID_;
		std::vector< Request> requests_;
		std::vector<RawNodeID> accepts_;

	public:
		AcceptDispatcher() = delete;
		AcceptDispatcher(
				ConductorBuilder& m,int registID, std::vector<Request>&& v) :
			manager_(m),
			registedID_(registID),
			requests_( std::move(v))
		{}

		AcceptDispatcher(AcceptDispatcher&& o) :
			manager_(o.manager_),
			requests_(std::move(o.requests_)),
			accepts_(std::move(o.accepts_)){}

		~AcceptDispatcher()
		{ manager_.accept( requests_, registedID_, accepts_); }

		std::vector<Request> const& getRequests() const
		{ return requests_; }

		void accept( Request const& r)
		{ accepts_.push_back(r.fromPortID_); }
	}; // struct AcceptDispatcher

	struct ConductResult
	{
		RawNodeID portID0_;
		RawNodeID portID1_;
		int fromID_;
		int toID_;

		ConductResult(
				RawNodeID _0,
				RawNodeID _1,
				int from_id,
				int to_id) :
			portID0_(_0), portID1_(_1), fromID_(from_id), toID_(to_id){}
	};
public:
	RegistDispatcher createRegister(int registID)
	{
		return createRegister(
				registID, element_v2::ConnectType::Undefine);
	}
	RegistDispatcher createRegister(
			int registID, element_v2::ConnectType const);

	void dispatchRequest( akasha::model::ModelConnectGraph const&);

	AcceptDispatcher conductorReport(int registedID);

	template<EnergyType T>
	bool isAccept(NodeID<T> const id) const
	{
		return isAccept(id.raw_value());
	}
	bool isAccept(RawNodeID const& id) const
	{
		return isAccept(id.raw_value_);
	}
	bool isAccept(int const& idNumber) const
	{
		auto i = boost::find_if(
				acceptedBuffer_, [idNumber](RawNodeID ap){
				return ap.raw_value_==idNumber;});

		return acceptedBuffer_.end() != i;
	}

	std::vector<RawNodeID> generateConnectPortIDs(int registID) const;

	void setupEnergyManager( EnergyManager& m) const
	{
		auto const& map = acceptedList_.get<0>();
		for (auto const& p : map)
		{
			m.addPort(p.portID0_);
			m.addPort(p.portID1_);
			m.connectPorts(p.portID0_, p.portID1_);
		}
	}
private:
	// To Inner Class
	void regist(
			std::vector<RegistDispatcher::registRequestType> const&, int );
	void accept(
			std::vector< AcceptDispatcher::Request> const& requests,
			int registID,
			std::vector<RawNodeID> const& accepts);

private:
	int lastIdNumber_ = 0;
	using requestBufferType = std::vector<RegistDispatcher::registRequestType>;
	requestBufferType requestBuffer_;

	// toID -> request_iterator
	using requestMapType =
		boost::unordered_multimap<int, requestBufferType::const_iterator >;
	requestMapType requestMap_;

	std::vector< RawNodeID > acceptedBuffer_; //<RawNodeID>
#define bmi boost::multi_index
	using acceptResultType =
		bmi::multi_index_container<
		ConductResult,
		bmi::indexed_by<
			bmi::random_access<>,
			bmi::hashed_non_unique<
			bmi::member<ConductResult,int, &ConductResult::toID_>>,
			bmi::hashed_non_unique<
			bmi::member<ConductResult,int, &ConductResult::fromID_>>
			>>;
#undef bmi

	acceptResultType acceptedList_;
};

//////////////////////////////////////////////
// ConductorBuilder Impliments
inline ConductorBuilder::RegistDispatcher
ConductorBuilder::createRegister(
		int registID, element_v2::ConnectType const connectType)
{
	return std::move(
			ConductorBuilder::RegistDispatcher(
				*this,
				lastIdNumber_,
				registID,
				connectType));
}
inline void ConductorBuilder::regist(
		std::vector<RegistDispatcher::registRequestType> const& r,
		int lastNodeNumber)
{
	lastIdNumber_ = lastNodeNumber;
	boost::copy(r, std::back_inserter(requestBuffer_));
}

inline void ConductorBuilder::dispatchRequest(
			akasha::model::ModelConnectGraph const& graph)
{
	auto const req_end = requestBuffer_.cend();
	for (
			auto req_iter = requestBuffer_.cbegin();
			req_iter != req_end;
			++req_iter
			)
	{
		auto const& r = *req_iter;
		switch (r.direct_)
		{
		case ConductDirect::Parent:
			{
				int parentID = graph.getParent(r.registID_);
				if (parentID >= 0)
					requestMap_.insert(
							std::make_pair(parentID, req_iter));
				break;
			} // Parent
		default:
			assert(false && "ConductorBuilder::dispatchRequest"); return;
		}
	}
}

inline ConductorBuilder::AcceptDispatcher
ConductorBuilder::conductorReport(int registedID)
{
	using value_type = AcceptDispatcher::Request;
	using requestBufValueType =
		std::pair<int, requestBufferType::const_iterator>;
	std::vector< value_type > reqList;

	auto rangeIter = requestMap_.equal_range( registedID );
	for (auto const& pairIter:
			boost::make_iterator_range(rangeIter))
	{
		auto const& reqIter = pairIter.second;
		reqList.emplace_back(
				reqIter->registID_, //from
				registedID, // to
				RawNodeID(lastIdNumber_++, reqIter->nodeID_.type_),
				reqIter->nodeID_,
				reqIter->direct_,
				reqIter->connectType_);
	}

	return std::move(
			AcceptDispatcher(*this, registedID, std::move(reqList)));
}

inline void ConductorBuilder::accept(
		std::vector< AcceptDispatcher::Request> const& requests,
		int,
		std::vector<RawNodeID> const& accepts)
{
	boost::copy(accepts, std::back_inserter(acceptedBuffer_));

	for (auto const& a : accepts)
	{
		auto req_iter = boost::find_if(
				requests,
				[&a](AcceptDispatcher::Request const& r){
				return r.fromPortID_.raw_value_ == a.raw_value_; });

		assert(req_iter != requests.end() &&
				"ConductorBuilder::accept()");

		acceptedList_.emplace_back(
				req_iter->fromPortID_,
				req_iter->nodeID_,
				req_iter->fromID_,
				req_iter->toID_
				);
	}
}

inline std::vector<RawNodeID>
ConductorBuilder::generateConnectPortIDs(int registID) const
{
	std::vector<RawNodeID> connectIDs;

	{ // to map
		auto const& regiMap = acceptedList_.get<1>();
		auto range = regiMap.equal_range(registID);

		for (auto const& r : boost::make_iterator_range(range))
				connectIDs.push_back(r.portID1_);
	}
	{// from map
		auto const& regiMap = acceptedList_.get<2>();
		auto range = regiMap.equal_range(registID);

		for (auto const& r : boost::make_iterator_range(range))
				connectIDs.push_back(r.portID0_);
	}

	return std::move(connectIDs);
}

} // namespace energy
} // namespace model
} // namespace akasha

#endif /* end of include guard */
