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



//! Irr�A�j���[�^�[���b�p�[�e���v���[�g
template<typename T>
	class IrrAnimator : T, public irr::scene::ISceneNodeAnimator
{
public:
	//! ���R���X�g���N�^
	IrrAnimator( const irr::u32 startTime ) :
		T( startTime )
	{

	}

	//! �N���[���R���X�g���N�^
	template<typename T_, typename N, typename S>
		IrrAnimator( const T_& t, N* n, S* smgr ) :
			T( t, n, smgr )
	{
	}

	//! �N���[��
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
//! �v���C���[���_
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

	//! �J�����̃I�t�Z�b�g��
	std::pair<float, float> const& getCameraOffset() const;
	void setCameraOffset(std::pair<float, float> const&);


	//! FOV
	float getFOV() const;
	void setFOV(float);

	//! �r���[���[�h
	void setViewMode(EViewMode);

	//! �𑜓x�ύX
	void setCameraAspect( const int w, const int h );
	//! �t���[���X�V
	void update( model::ModelObserver const& model);


	boost::fusion::vector<btVector3, btVector3>
		getMouseCursorRayDirect() const;


};

}

#endif //PLAYER_VIEW__HPP
