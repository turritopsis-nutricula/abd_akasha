#ifndef AKASHA_ELEMENT_TYPE_FWD_HPP_INCLUDED
#define AKASHA_ELEMENT_TYPE_FWD_HPP_INCLUDED

#include "CreateInfo.hpp"
#include "../../WispTasks.hpp"
#include <riceBall/mpl/EasyTuple.hpp>
#include <boost/type.hpp>
#include <boost/static_assert.hpp>

namespace akasha
{
namespace model
{
namespace element_v2
{
template<typename ElementType>
	struct TypeToId
{
	constexpr static int value = -1;
};
template<int ID>
struct IdToType
{
	typedef void type;
	constexpr static bool value = false;
};

template<typename ElementType>
ElementType* MakeElement(
		CreateInfo const&,
		boost::shared_ptr<akasha::NodeUpdateTask> const& task,
		boost::type<ElementType> const&);

template<int ID>
constexpr char const* IdToString();

template<int ID>
constexpr bool IsAdapterFromId();

template<int ID>
constexpr int InsertElementID(){ return -1; }

template<int ID>
constexpr int TailElementID(){ return -1; }

template<int ID>
constexpr bool IsCreateble(){ return true; }

#define ELEMENT_FWD(ID, name, str, isAdapter) class name ; \
	template<> \
name * MakeElement(CreateInfo const&, \
		boost::shared_ptr<akasha::NodeUpdateTask> const&, \
		boost::type< name > const &); \
template<> \
constexpr char const* IdToString< ID >(){ return str;} \
template<> \
struct TypeToId< name > { constexpr static  int value = ID; }; \
template<> \
struct IdToType< ID > { typedef name type; constexpr static bool value=true; }; \
template<> \
constexpr bool IsAdapterFromId< ID >() { return isAdapter; }

#define SET_INSERT_ELEMENT(Id, insert_id) \
	template<> constexpr int InsertElementID< Id >() {return insert_id; }

#define SET_PUSH_ELEMENT(Id, tail_id) \
	template<> constexpr int TailElementID< Id >() {return tail_id; }

#define DIS_CREATEBLE( id ) \
	template<> constexpr bool IsCreateble< id >(){ return false; }

//コア Core
ELEMENT_FWD(0, CoreElement , "CORE", false);
//チップ Chip
ELEMENT_FWD(1, ChipElement , "CHIP", false);
//ラダー Rudder
ELEMENT_FWD(2, RudderElement ,"RUDDER", false);
//トリム Trim
ELEMENT_FWD(6, TrimElement , "TRIM" , false);
//リム Rim
ELEMENT_FWD(3, RimElement,   "RIM", false);
DIS_CREATEBLE(3);
//ホイール Wheel
ELEMENT_FWD(4, WheelElement , "WHEEL", false);
SET_INSERT_ELEMENT(4, 3);
//ウェイト Weight
//TODO : option値での接続強度変更未実装
ELEMENT_FWD(8, WeightElement, "WEIGHT", false);
//カウル Cowl
ELEMENT_FWD(9, CowlElement, "COWL" , false);
//アーム Arm
ELEMENT_FWD(10, ArmElement, "ARM" , false);
//ジェット Jet
ELEMENT_FWD(7, JetElement, "JET" , false);
//フレーム Frame
ELEMENT_FWD(33, FrameElement, "FRAME" , false);
//ラダーフレーム RudderF
ELEMENT_FWD(34, RudderFrameElement, "RUDDERF" , false);
//トリムフレーム TrimF
ELEMENT_FWD(35, TrimFrameElement, "TRIMF" , false);
//Battery
ELEMENT_FWD(201, BatteryElement, "BATTERY", false);
// Empty Adapter Void
ELEMENT_FWD(1000, VoidAdapterElement, "VOID", true);
// Air TAnk Tank
ELEMENT_FWD(202, AirTankElement, "TANK", false);
// Housing Housing
ELEMENT_FWD(500, HousingElement, "HOUSING", false);
SET_PUSH_ELEMENT(500, 600);
// OutputShaft
ELEMENT_FWD( 600, OutputShaftElement, "OUTPUTSHAFT", false);
DIS_CREATEBLE( 600 );
// 			//パネル Panel
// 			ELEMENT_FWD(100, PanelElement, "PANEL" , false);
// 			//ユニット Unit
// 			ELEMENT_FWD(200, BatteryElement, "BATTERY" , false);
// 			//出力軸 SHAFT
// 			ELEMENT_FWD(210, ShaftElement, "SHAFT" , false);
// 			//出力輪 DWheel
// 			ELEMENT_FWD(211, DrivenWheelElement, "DRIVENWHEEL" , false);
// 			//デバック PULSE
// 			ELEMENT_FWD(500, PulseEngineElement, "PULSEENGINE" , false);
// 			/// パイプ PIPE
// 			ELEMENT_FWD(300, PipeElement, "PIPE" , false);
ELEMENT_FWD( -1, UndefineElement, "UNDEFINE", false);
DIS_CREATEBLE(-1);


#undef SET_INSERT_ELEMENT
#undef SET_PUSH_ELEMENT
#undef ELEMENT_FWD

namespace detail
{
typedef rice::mpl::TypeTuple<
	// Regacy
	CoreElement
	,ChipElement
	,RudderElement
	,TrimElement
	,RimElement
	,WheelElement
	,JetElement
	//Frame family
	,FrameElement
	,RudderFrameElement
	,TrimFrameElement

	// New
	//,BatteryElement

	// Adapter
	,VoidAdapterElement //TODO: under constract

	// Tank
	,AirTankElement

	// Housing
	,HousingElement

	// Output
	, OutputShaftElement
	, UndefineElement


	//,WeightElement
	//,CowlElement
	//,ArmElement
	//,PanelElement
	> ElementFWDTypeTuple;
}
typedef typename rice::mpl::ReMap<
boost::type,
	detail::ElementFWDTypeTuple>::type ElementTypeTuple;
typedef detail::ElementFWDTypeTuple ElementRawTypeTuple;


namespace mpl {


template<int N,
	bool ExprResult,
	template <int> class HolderType,
	typename NextTuple
		>
		struct IF_ExprType;

template<int N,
	template <int> class HolderType,
	typename NextTuple>
		struct IF_ExprType<N, true, HolderType, NextTuple>
		{
			typedef  typename rice::mpl::PushBack<
				HolderType<N>, NextTuple
				>::type type;
		};
template<int N,
	template <int> class HolderType,
	typename NextTuple>
		struct IF_ExprType<N, false, HolderType, NextTuple>
		{
			typedef NextTuple type;
		};
template<
	int N, int End,
		 template <int> class ExprType,
		 template <int> class HolderType,
		 typename RootType>
			 struct TupleMaker
			 {
				 static constexpr bool enable = ExprType<N>::value;
				 typedef typename IF_ExprType<
					 N, enable, HolderType,
					 typename TupleMaker<N+1,End,ExprType,HolderType, RootType>::type>::type
						 type;
			 };
template<int End, template <int> class ExprType,template <int> class HolderType, typename RootType>
	struct TupleMaker<End,End, ExprType,HolderType, RootType>
	{
		typedef rice::mpl::TypeTuple< RootType > type;
	};
/////////////////////////////////
// usage:
// template<int ID> struct ValidExpr{
// 	constexpr static bool value = IdToType<N>::value;
// };
// template<int ID> struct Unit{
//  typdef typename IdToType< ID >::type elemType;
//  typdef boost::shared ptr<elemType> type;
//  };
//  struct Root{
//  typedef boost::shared_ptr<CoreElement> type;
//  };
//  typdef typename TupleMaker<1,10, ValidExpr,Unit,Root>::type
//  tuple;
//  typedef rice::mpl::TypeTupleFolder<tuple> buffer_type;
} // namespace mpl
} // element_v2
} // model
}

#endif // AKASHA_ELEMENT_TYPE_FWD_HPP_INCLUDED
