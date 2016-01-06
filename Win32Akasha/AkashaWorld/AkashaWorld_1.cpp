


#include "AkashaWorld.h"
#include "../AkashaSystem/AkashaSystem.hpp"

#include "environment/EnvironmentLand.hpp" // 環境

#include "model/loader/RCFileLoader.hpp" // Rcdファイルローダー
#include "model/ElementModel.hpp" // モデル

#include "PlayerView.hpp"       // 視点
#include "Toy/Toy.hpp"

#include "detection/ObjectCapture.hpp" //オブジェクトキャプチャー

//#include "WorldCommand.hpp"     //ワールドコマンド

#include "WispTasks.hpp"
#include "IrrExtention.h"
#include "RenderType.hpp"




#include "bullet/btProfileDump.hpp" //ダンプキャプチャ
//wisp
#include <wisp_v3/wisp.hpp>

//irr
#include "COpenGLMaterialRenderer.h"
#include "COpenGLSLMaterialRenderer.h"
//bullet
#include "btIrrDebugDraw.h"


//boost
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/make_shared.hpp>
#include <boost/format.hpp>




//#include "SpeakerUnit.hpp"
//#include "WaveBuffer.hpp"
//#include "Speaker.hpp"

#include "convert/ConvertFunction.hpp" // データ形式コンバート


//#include "avatar/AvatarBase.hpp" // プレイヤーアバター




using namespace akasha;
using namespace irr;
using namespace core;
using namespace video;
using namespace scene;

//Land読み込み################################################################
	bool
AkashaWorld::loadLand( const irr::io::path& landFilePath )
{
	{
		auto newLand =
			boost::make_shared<akasha::land::EnvironmentLand>(
					m_irrEx, m_wisp  );
		if ( !newLand->loadFromFile( landFilePath ) )
			return false;

		m_land.swap( newLand );


	}

	//モデルに更新を通知
	if ( m_model )
		m_model->changeEnvironmentLand( m_land );
	if (m_subModel)
		m_subModel->changeEnvironmentLand( m_land );

	// ContactEvnetClear
	m_land->registContactGroup(m_colEvent);
	m_colEvent.clear();

	//モデルを初期位置へ
	initModelPosition();
	initSubModelPosition();

	akasha::system::AkashaSystem::Instance().getLogger()(
			akasha::system::log::ApplicationInfo {"Land Load Compleate."} );

	return true;
}

//モデル読み込み################################################################


//fwd
bool loadElementModel(
		const irr::io::path& modelFilePath,
		irrExt::IrrExtention& irrEx,
		wisp::v3::WispHandle& wisp,
		boost::shared_ptr<akasha::NodeUpdateTask> const&,
		boost::shared_ptr<land::EnvironmentLand> const& land,
		AkashaWorld& world,
		contactEvent::ContactEventManager& colEvent,
		boost::scoped_ptr<model::ElementModel>& resultModel,
		model::ElementModel::ModelType modelType );

// Main Model Load //////////////////////////////////////////
bool AkashaWorld::loadModel( const irr::io::path& modelFilePath )
{

	bool result = loadElementModel(
			modelFilePath,
			m_irrEx,
			m_wisp,
			m_irrNodeUpdate,
			m_land,
			*this,
			m_colEvent,
			m_model,
			model::ElementModel::ModelType::main
			);
	if ( !result ) return false;

	//モデルファイル名を保存
	m_modelFilePath = modelFilePath;

	//モデルを初期位置へ
	initModelPosition();

	akasha::system::AkashaSystem::Instance().getLogger()(
			akasha::system::log::ApplicationInfo {"Model Load Compleate."} );

	// Simulation Forsed start.
	setSimulationRunning(true);

	//テストコード
	if( 0 )
	{
		network::OtherModelBase<std::string> base;
		m_model->getNetworkBase( base );

		m_otherModel.clear();
		m_otherModel.emplace_back();
		m_otherModel.back().constract( base );
	}
	return true;
}


//SubModel Load ##################################################################################
bool AkashaWorld::loadSubModel(irr::io::path const& filePath)
{

	bool result = loadElementModel(
			filePath,
			m_irrEx,
			m_wisp,
			m_irrNodeUpdate,
			m_land,
			*this,
			m_colEvent,
			m_subModel,
			model::ElementModel::ModelType::sub
			);
	if ( !result ) return false;

	//モデルを初期位置へ
	initSubModelPosition();

	akasha::system::AkashaSystem::Instance().getLogger()(
			akasha::system::log::ApplicationInfo {"SubModel Load Compleate."} );

	return true;

}

bool loadElementModel(
		const irr::io::path& modelFilePath,
		irrExt::IrrExtention& irrEx,
		wisp::v3::WispHandle& wisp,
		boost::shared_ptr<akasha::NodeUpdateTask> const& nodeTask,
		boost::shared_ptr<land::EnvironmentLand> const& land,
		AkashaWorld& world,
		contactEvent::ContactEventManager& colEvent,
		boost::scoped_ptr<model::ElementModel>& resultModel,
		model::ElementModel::ModelType modelType )
{

	//現状RCDのみ
	model::loader::ModelDefine<std::string> data;
	{
		model::loader::RCFileLoader loader;
		bool result = false;
		wisp.synchronaizedOperate(
				wisp::SynchronizeFuncType(
					[&loader, &modelFilePath, &data, &result]( irr::IrrlichtDevice * irr )
					{
// 					std::cout << irr->getFileSystem()->getWorkingDirectory().c_str() << std::endl;

					wisp::WorkingDir workDir(irr);
					result = loader.loadFromFile( *irr->getFileSystem(), modelFilePath, data );
					}));

		if ( !result ) //失敗
			return false;
		//TODO : エラーメッセージを考える
	}

	//ファイルパスを記録
	data.modelFileFullPath_ = modelFilePath.c_str();

	//TODO : デバックに再読み込み
	system::AkashaSystem::Instance().getConstant().load();

	{
		//コンストラクト
		boost::scoped_ptr<model::ElementModel> newModel(
				new model::ElementModel( irrEx,boost::ref( wisp ), nodeTask, land ) );

		auto sumifunc = boost::bind(&AkashaWorld::setSimulationRunning, boost::ref(world), _1);
		if ( !newModel->constract( data, modelType, std::move(sumifunc) ) )
			return false;

		resultModel.swap( newModel );
	}

	//ContactEvent Regist
	{
		for(auto const& i : resultModel->createContactGroupMap())
			colEvent.registBody(i.first, i.second);
		colEvent.clear();
	}
	//初期化完了ハンドラ
	resultModel->onInit();

	return true;
}
