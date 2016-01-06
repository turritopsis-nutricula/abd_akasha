#ifndef PLAYER_VIEW__HPP
#define PLAYER_VIEW__HPP


#include "model/ModelObserver.hpp"
#include "../AkashaApplication/SoundManager.hpp"
#include <wisp_v3/wisp.hpp>

#include "ICameraSceneNode.h"
#include "os.h"

#include <LinearMath/btVector3.h>
#include <boost/array.hpp>

namespace akasha
{



//! Irrアニメーターラッパーテンプレート
template<typename T>
	class IrrAnimator : T, public irr::scene::ISceneNodeAnimator
{
public:
	//! 基底コンストラクタ
	IrrAnimator( const irr::u32 startTime ) :
		T( startTime )
	{

	}

	//! クローンコンストラクタ
	template<typename T_, typename N, typename S>
		IrrAnimator( const T_& t, N* n, S* smgr ) :
			T( t, n, smgr )
	{
	}

	//! クローン
	irr::scene::ISceneNodeAnimator*
		createClone( irr::scene::ISceneNode* i, irr::scene::ISceneManager* m )
		{
			return new IrrAnimator<T>( *this, i, m );
		}
	void
		animateNode( irr::scene::ISceneNode *node, irr::u32 timeMs )
		{
			T::operator()( node, timeMs );
		}
	bool
		hasFinished() const
		{
			return T::hasFinished();
		}
	bool
		isEventReceiverEnabled() const
		{
			return false;
		}
};


enum class EViewMode
{
	Celestial,
	Target,

};
//! プレイヤー視点
class PlayerView
{

	wisp::v3::WispHandle& m_wisp;
	wisp::ObjectID m_cameraID;

	std::pair<float, float> m_cameraOffset;
	float m_cameraFOV;
	float m_cameraAspect;

	boost::function<bool ( boost::array<btVector3, 3>&, std::pair<float, float> const&, model::ModelObserver const&, int )> m_viewFunction;
	btTransform m_viewTransform;
	int m_tick;

public:
	struct cameraUpdate
	{
		btVector3 pos_;
		btVector3 target_;
		btVector3 up_;
		float fov_;
		float aspect_;
	};
private:
	boost::shared_ptr< irrExt::Task< cameraUpdate > >
		m_cameraUpdateTask;

public:

	PlayerView( irrExt::IrrExtention&, wisp::v3::WispHandle& );

	//! カメラのオフセット量
	std::pair<float, float> const& getCameraOffset() const;
	void setCameraOffset(std::pair<float, float> const&);


	//! FOV
	float getFOV() const;
	void setFOV(float);

	//! ビューモード
	void setViewMode(EViewMode);

	//! 解像度変更
	void setCameraAspect( const int w, const int h );
	//! フレーム更新
	void update( model::ModelObserver const& model);


	boost::fusion::vector<btVector3, btVector3>
		getMouseCursorRayDirect() const;


};

}

#endif //PLAYER_VIEW__HPP
