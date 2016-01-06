#ifndef ELEMENT_MODEL__HPP
#define ELEMENT_MODEL__HPP

#include "ModelObserver.hpp"
#include "../environment/EnvironmentLand.hpp"
#include "../../AkashaApplication/network/WorldData.hpp"

#include "../contactEvent/EventGroup.hpp"
#include <wisp_v3/wisp.hpp>
//#include "../entity/EntityService.hpp"

#include "loader/define/ModelDefineStruct.hpp"




namespace akasha
{
//fwd
class NodeUpdateTask;
namespace model
{
//! エレメントモデル
class ElementModel
{

	struct Impl; //先行宣言
	boost::scoped_ptr<Impl> m_impl;
public:
	enum class ModelType : unsigned int
	{
		undefine,
		main,
		sub,
	};

	using contactEventMapType =
		std::vector<
		std::pair<contactEvent::EventGroup,
		boost::shared_ptr<btFractureBody>>>;
public:
	ElementModel(
			irrExt::IrrExtention&,
			wisp::v3::WispHandle&,
			boost::shared_ptr<akasha::NodeUpdateTask>,
			boost::shared_ptr<land::EnvironmentLand> land);

	~ElementModel();

	//! データから構築
	bool constract(
			const loader::ModelDefine<std::string>& data,
			ModelType type,
			boost::function<void (bool)>&& simulationHandle);


	//! 初期化完了ハンドラ
	void onInit();

	btTransform const&
		getElementTransform(const int) const;

	//! エレメント重量を返す
	btScalar
		getElementMass(const int n) const;

	//! 重心位置
	boost::optional<btVector3>
		gravityCenter(unsigned int id = 0) const;
	boost::optional<btVector3>
		getGravityCenter(unsigned int id = 0) const;

	//! 接続の親を返す。-1で切断
	int getParentElementID(const unsigned int n) const ;

	//! 接続の根を返す。
	int getTopElementID(const unsigned int n) const ;

	//! ContactEventGroup Map
	contactEventMapType
		createContactGroupMap() const;

	boost::shared_ptr<ModelObserver> const&
		getObserver() const;

	// ContactEvent
	void contactToLandForm(btManifoldPoint const& pt, void const* btObject);

	////////////////
	//! モデルを移動
	void modelWrap(btTransform const& t);

	void drawExternForce() const;

	void changeEnvironmentLand( boost::shared_ptr<land::EnvironmentLand> );

	//! Model frame count
	std::size_t getLifeTicks() const;

	////////////////
	//! 更新
	void update(const btScalar fps, const btScalar sub, bool const isSimulationRunning);

	void stepEndUpdate(btScalar const fps, btScalar const sub, bool const isSimulationRunning);

	////////////////
	//内部ステップ
	void subStep(const btScalar step);
	void subStepEnd(const btScalar step);

	const std::wstring&
		getLuaErrorMesage() const;


	///////////////
	//ネットワーク
	void getNetworkBase(network::OtherModelBase<std::string>&) const;

	void getNetworkUpdata(network::OtherModelUpdataer&) const;

};

}
}
#endif
