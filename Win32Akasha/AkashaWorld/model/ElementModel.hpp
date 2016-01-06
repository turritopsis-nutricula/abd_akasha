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
//! �G�������g���f��
class ElementModel
{

	struct Impl; //��s�錾
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

	//! �f�[�^����\�z
	bool constract(
			const loader::ModelDefine<std::string>& data,
			ModelType type,
			boost::function<void (bool)>&& simulationHandle);


	//! �����������n���h��
	void onInit();

	btTransform const&
		getElementTransform(const int) const;

	//! �G�������g�d�ʂ�Ԃ�
	btScalar
		getElementMass(const int n) const;

	//! �d�S�ʒu
	boost::optional<btVector3>
		gravityCenter(unsigned int id = 0) const;
	boost::optional<btVector3>
		getGravityCenter(unsigned int id = 0) const;

	//! �ڑ��̐e��Ԃ��B-1�Őؒf
	int getParentElementID(const unsigned int n) const ;

	//! �ڑ��̍���Ԃ��B
	int getTopElementID(const unsigned int n) const ;

	//! ContactEventGroup Map
	contactEventMapType
		createContactGroupMap() const;

	boost::shared_ptr<ModelObserver> const&
		getObserver() const;

	// ContactEvent
	void contactToLandForm(btManifoldPoint const& pt, void const* btObject);

	////////////////
	//! ���f�����ړ�
	void modelWrap(btTransform const& t);

	void drawExternForce() const;

	void changeEnvironmentLand( boost::shared_ptr<land::EnvironmentLand> );

	//! Model frame count
	std::size_t getLifeTicks() const;

	////////////////
	//! �X�V
	void update(const btScalar fps, const btScalar sub, bool const isSimulationRunning);

	void stepEndUpdate(btScalar const fps, btScalar const sub, bool const isSimulationRunning);

	////////////////
	//�����X�e�b�v
	void subStep(const btScalar step);
	void subStepEnd(const btScalar step);

	const std::wstring&
		getLuaErrorMesage() const;


	///////////////
	//�l�b�g���[�N
	void getNetworkBase(network::OtherModelBase<std::string>&) const;

	void getNetworkUpdata(network::OtherModelUpdataer&) const;

};

}
}
#endif
