#ifndef EVENT_HPP_INCLUDED
#define EVENT_HPP_INCLUDED

#include "../CreateInfo.hpp"
#include "../../../WispTasks.hpp"
#include "../detail/ElementParameter.hpp"
#include "../../../bullet/btFractureBody.hpp"
#include "../../../contactEvent/EventGroup.hpp"

#include "../../energy/EnergyManager.hpp"

#include <type_traits>

namespace akasha
{
namespace model
{
namespace element_v2
{
namespace phase
{

// element init
struct Initialize
{
	CreateInfo const& info_;
	boost::shared_ptr<ElementParameter> paramPtr_;
	boost::shared_ptr<NodeUpdateTask> nodeUpdateTask_;
};

// Energy setting
struct EnergyPortRequest
{
	energy::ConductorBuilder::RegistDispatcher& register_;
};
struct EnergyPortAccept
{
	energy::ConductorBuilder::AcceptDispatcher& accepter_;
};
struct EnergyPortComplete
{
	energy::ConductorBuilder const& builder_;
	int elementID_;
};

// Joint connect
struct ElementConnect
{
	element_v2::DirectType direct1_;
	boost::shared_ptr<btFractureBody> parentBody_;
	boost::shared_ptr<btFractureBody> mineBody_;
	ElementParameter const& paramRef_;
	std::vector<boost::shared_ptr<ElementParameter>> parentsParams_;
	btMatrix3x3 rerativeTrans_;
};

//! Element Init Arrangement
struct ElementArrangement
{
	element_v2::DirectType direct_;
	btTransform parentTransform_;
	btTransform& resultTransform_;
};

// Welding
struct ElementWelding { };

} // namespace phase

namespace event
{
//! 強制エレメント接続解除
struct ForceDisconnect
{
};
//! btConstraint Release
struct ConstraintRelease
{
};
/////////////////////
//Physics Command
struct Warp{float x,y,z;};
struct ApplyForce{float x,y,z;};

/////////////////////
//Getter
struct GetPhysicsBody{
	boost::shared_ptr<btFractureBody>& body_;
};
struct GetContactEventGroup
{
	boost::optional<contactEvent::EventGroup>& group_;
};

////////////////////
//更新
//! サブステップ開始
struct PreTick
{
	float subStep_;
	energy::EnergyManager& energyManager_;
};
//! サブステップ終了
struct Tick
{
	float subStep_;
	energy::EnergyManager const& energyManager_;
};

//! ステップ開始
struct StepStart
{
	float fps_, subStep_;
	energy::EnergyManager& energyManager_;
};
//! ステップ終了
struct StepEnd
{
	energy::EnergyManager const& energyManager_;
};

////////////////////////
//告知
//! ParameterUpdate
template<typename ParameterType>
struct ParameterUpdate{};
//! 強制移動
struct ForceMove
{
	btTransform const& to_;
};

//! 外力表示
struct ExternForce
{
	bool enable_;
	// 		    typedef boost::fusion::vector<btVector3, btVector3, btVector3> lineType;
	// 		    std::vector<lineType>& buffer_;
};



}
}
}
}

//helper Define
namespace akasha{namespace model{ namespace element_v2{ namespace helper{
extern void * enabler;
}}}}
#define CreateSwitch(Nname, expr ) template<int Nname, \
	typename std::enable_if< expr >::type*& = ::akasha::model::element_v2::helper::enabler>

#endif // EVENT_HPP_INCLUDED
