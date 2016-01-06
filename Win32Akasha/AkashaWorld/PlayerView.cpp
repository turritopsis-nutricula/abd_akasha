#include "PlayerView.hpp"

struct TestAnimator
{

	irr::u32 startTime_;
	TestAnimator( const irr::u32 startTime ) : startTime_( startTime ) {  }

	template<typename T, typename N, typename S>
		TestAnimator( const T& t, N* n, S* mgr ) {  }

	bool operator()( irr::scene::ISceneNode* node, irr::u32 timeMs ) const
	{
		using irr::core::vector3df;
		vector3df center( 0.f, 45.f, 0.f );
		vector3df axis( 0.f, 1.f, 0.f );

		vector3df const& pos = node->getPosition();
		vector3df sub( pos - center );
		sub.Y = 0.f;
		vector3df dir( sub.crossProduct( axis ).normalize() );

		irr::u32 time;
		float Speed( 0.1 );
		// Check for the condition where the S tartTime is in the future.
		if( startTime_ > timeMs )
			time = ( ( irr::s32 )timeMs - ( irr::s32 )startTime_ ) * Speed;
		else
			time = ( timeMs - startTime_ ) * Speed;

		dir *= ( float )time * 0.00003f;
		node->setPosition( center + sub.normalize() * 10.f + dir );

		return true;
	}

	bool hasFinished() const
	{
		return false;
	}

};


//ターゲットに完全一致
struct TargetView
{
	void init(btVector3 const&)
	{

	}
	bool operator()(
			boost::array<btVector3, 3>& result,
			std::pair<float, float>,
			akasha::model::ModelObserver const& model, int )
	{
		btTransform const& t = model.getElementStatus(0).getWorldTransform();
		result[0] = t.getOrigin();
		btMatrix3x3 const& basis = t.getBasis();

		result[1] = basis[2];
		result[2] = basis[1];

		result[1].setX( result[1].getX() * -1.f );
		result[1] += result[0];
		result[2].setX( result[2].getX() * -1.f );

		return true;
	}
};


struct CelestialView
{
	btVector3 m_basePoint;
	btScalar m_distance;
	btScalar m_speed;

	btVector3 m_targetBuffer;

	btVector3 m_viewBuffer;

	CelestialView(btVector3 const& initPos) :
		m_basePoint( 0.f, 0.f, 0.f ),
		m_distance( 10.0 ),
		m_speed( 0.01f ),
		m_targetBuffer( initPos ),
		m_viewBuffer(0.f,0.f, -1.f)
	{
	}

	bool operator()(
			boost::array<btVector3, 3>& result,
			std::pair<float,float> const& offset,
			akasha::model::ModelObserver const& model,
			int
			)
	{
		if (auto targetOpt = model.getGravityCenter( 0 ))
		{

			btVector3 view(targetOpt - m_basePoint);
			view.setY(btScalar(0.0));
			if (view.length2() > SIMD_EPSILON)
				view.normalize();
			else
				view = btVector3(0.f, 0.f, -1.f);

			btVector3 mov( targetOpt - view - m_basePoint);
			btScalar const drift(mov.length());
			if (drift < SIMD_EPSILON )  return false;
			mov /= drift;
			m_basePoint += mov * btMin((drift*drift)*0.1f, drift);

			btVector3 const up(0.f,1.f,0.f);
			btVector3 correctView( targetOpt - m_basePoint );
			btScalar const distance( correctView.length() );

			if (distance < SIMD_EPSILON ) return false;
			correctView /= distance;

			btVector3 const viewTan( up.cross( correctView ) );
			btVector3 const viewNorm( correctView.cross(viewTan) );

			btMatrix3x3 const m( viewTan.getX(), viewTan.getY(), viewTan.getZ(),
					viewNorm.getX(), viewNorm.getY(), viewNorm.getZ(),
					-correctView.getX(), -correctView.getY(), -correctView.getZ());
			btQuaternion r(btVector3(0.f,1.f,0.f), offset.first * SIMD_PI);
			r *= btQuaternion(btVector3(1.f, 0.f,0.f), (offset.second-0.1f)*SIMD_PI);

			result[0] = m_basePoint + m * quatRotate(r, btVector3(0.f,0.f,5.f));
			result[1] = lerp(m_viewBuffer, correctView, 0.01f);
			m_viewBuffer = result[1];
			result[1] += m_basePoint;
			result[2] = viewNorm;


			return true;
		}

		return false;
	}


};

void CameraUpdateTask(
		akasha::PlayerView::cameraUpdate const&,
		boost::uuids::uuid const,
		irrExt::TaskArg const&
		);

namespace akasha
{

PlayerView::PlayerView( irrExt::IrrExtention& irrEx, wisp::v3::WispHandle& c) :
	m_wisp( c ),
	m_cameraOffset({0.f,0.f}),
	m_cameraFOV(50.f),
	m_viewTransform( btTransform::getIdentity() )
	{
		m_tick = 0;

		m_viewFunction = CelestialView(btVector3(0,0,0)); //TargetView();
		//m_viewFunction = TargetView();

		m_cameraID = irrEx.createObject( wisp::CreateFuncType(
					[]( boost::uuids::uuid id,
						irr::IrrlichtDevice * irr,
						wisp::IrrObjectStore & store )
					{
					auto* n = irr->getSceneManager()->addCameraSceneNode( 0,
							irr::core::vector3df( 0.f, 60.f, -10.f ),
							irr::core::vector3df( 0.f, 0.f, 0.f ), true );
					store.store( id, n, 0 );

					n->setName( "MainCamera" );
					}
					) );
		assert( m_cameraID && "Camera ID by PlayerView");

		m_cameraUpdateTask = irrEx.createTask(
				boost::bind(CameraUpdateTask, _1, *m_cameraID, _2),
				boost::type<cameraUpdate>());

	}

//! カメラのオフセット量
std::pair<float, float> const& PlayerView::getCameraOffset() const
{
	return m_cameraOffset;
}
void PlayerView::setCameraOffset(std::pair<float, float> const& o)
{
	m_cameraOffset = o;
}


//! FOV
float PlayerView::getFOV() const
{
	return m_cameraFOV;
}
void PlayerView::setFOV(float f)
{
	m_cameraFOV = btClamped(f, 1.f, 160.f);
}

//! ViewMode
void PlayerView::setViewMode(akasha::EViewMode m)
{
	using akasha::EViewMode;
	switch( m)
	{
	case EViewMode::Celestial:
		m_viewFunction = CelestialView(btVector3(0.,0.,0.));
		break;
	case EViewMode::Target:
		m_viewFunction = TargetView();
		break;
	default :
		assert(false);

	}
	return;
}

//! カメラアスペクト
void PlayerView::setCameraAspect( const int w, const int h )
{
	if ( h > 0 && w > 0 )
		m_cameraAspect = (float)w/(float)h;
}

//! update
void PlayerView::update( model::ModelObserver const& modelObj )
{

	boost::array<btVector3, 3> result;
	bool calcResult =
		m_viewFunction( result, m_cameraOffset, modelObj, m_tick );
	m_tick += 16;

	if (!calcResult) return;

	btVector3 pos( result[0] ), target( result[1] ), up( result[2] );

	cameraUpdate data;
	data.pos_ = result[0];
	data.target_ = result[1];
	data.up_ = result[2];
	data.fov_ = m_cameraFOV;
	data.aspect_ = m_cameraAspect;

	m_cameraUpdateTask->post( data );

}


boost::fusion::vector<btVector3, btVector3>
PlayerView::getMouseCursorRayDirect() const
{
	//TODO : 自前で実装
	return boost::fusion::vector<btVector3, btVector3>(
			btVector3( 0.f, 0.f, 0.f ), btVector3( 0.f, 1.f, 0.f ) );
}

}

void CameraUpdateTask(
		akasha::PlayerView::cameraUpdate const& data,
		boost::uuids::uuid const id,
		irrExt::TaskArg const& arg
		)
{
	auto* n = arg.store_.find( id );
	assert( n );

	irr::scene::ICameraSceneNode* c =
		static_cast<irr::scene::ICameraSceneNode*>( n );

	auto const& pos    = data.pos_;
	auto const& target = data.target_;
	auto const& up     = data.up_;
	auto const& fov    = data.fov_;

	c->setPosition(
			irr::core::vector3df( pos.getX(), pos.getY(), pos.getZ() ) );
	c->setTarget(
			irr::core::vector3df( target.getX(), target.getY(), target.getZ() ) );
	c->setUpVector(
			irr::core::vector3df( up.getX(), up.getY(), up.getZ() ) );

	c->setFOV( btRadians(fov) );

	c->setAspectRatio( data.aspect_ );

}
