#ifndef AKASHA_MODEL_FLEXIBLEJOINTBEHAVIOR_HPP_
#define AKASHA_MODEL_FLEXIBLEJOINTBEHAVIOR_HPP_

#include "../event/event.hpp"
#include "../../../bullet/ConstraintWrapper.hpp"
#include "../../../bullet/btFractureBody.hpp"
#include "../../../../AkashaSystem/AkashaSystem.hpp"

#include <boost/fusion/include/vector_tie.hpp>
#include <boost/fusion/include/make_vector.hpp>

namespace akasha {
namespace model {
namespace element_v2 {

//fwd
class AdapterParameter;

namespace behavior {


//fwd
class FlexibleXJointBehavior;
class FlexibleYJointBehavior;
class FlexibleZJointBehavior;

namespace detail {

// JointBase
template<typename JointPolicy, typename AxisPolicy>
	class FlexibleJointBase
	{

		typedef typename JointPolicy::jointType JoiytType;
		using jointArg = typename JointPolicy::argType;

	private:
		JoiytType m_joint;  //<! EntityJoint
		jointArg  m_arg;

		btTransform m_parentPartOffset; // In parentBody

		int m_minePartCount;
		btTransform m_mineJointFrame;
		btTransform m_minePartOffset;

		btTransform m_parentJointFrame; // In parentBody
		btQuaternion m_zeroBasis;

		btScalar m_targetAngle;
		float m_angleStep; //!< angle of one tick
		btScalar m_angleBuffer;  //!< 更新前の設定角度

		btScalar m_needEnergy;  //!< 角度変更エネルギー
		btScalar m_energyRatio;  //!< エネルギー効率
		btScalar m_breakImpulse;  //!< 破損運動量

		btScalar m_baseSpringParam;
		btScalar m_baseDamperParam;

		std::vector<boost::shared_ptr<AdapterParameter>> m_adapters;

	public:
		template<typename T>
			void apply(T const&){ }
		//init
		void apply( phase::Initialize const& i)
		{
			this->init( i.info_.data_ , *(i.paramPtr_));
		}
		//connect
		void apply( phase::ElementConnect const& c)
		{
			this->connect(
					c.direct1_,
					c.rerativeTrans_,
					c.parentBody_,
					c.mineBody_,
					c.paramRef_,
					c.parentsParams_);
		}

		void init( element::ElementData const& data, ElementParameter const& param);

		void connect(
				DirectType const& direct,
				btMatrix3x3 const& relativeBasis,
				boost::shared_ptr<btFractureBody> parentBody,
				boost::shared_ptr<btFractureBody> mineBody,
				ElementParameter const& param,
				std::vector<boost::shared_ptr<ElementParameter>> const& parentsParams);

		void setTargetParameter(ElementParameter const& p, float fps, float subStep)
		{
			updateParameter(p, fps, subStep);
		}
		void updateParameter(ElementParameter const&, float fps, float subStep);
		void updateAngle();
		void setConstraintAngle(btScalar const ang);
		void updateSpringAndDamper(btScalar const sp, btScalar const dam);
	};

////////////////////////////////////////////////////////////////
//AnglerPolicy
struct XAxisJointUpdater;
struct YAxisJointUpdater;
struct ZAxisJointUpdater;

////////////////////////////////////////////////////////////////
// JointConstractPolicy
struct HingeJoint;
struct G6DJoint;
struct G6DSpringJoint;

//
// ERP = h*Kp  /  (h*Kp + Kd) ...1
// CFM =   1   /  (h*Kp + Kd) ...2
//
//h = StepSize : Kp = Spring : Kd = Damping
//
//
//h*Kp = ERP * ( CFM + 2 * CFM * ERP )
//Kd   = (1+ERP) / ( CFM * (1 + 2* ERP) )
//
//h*Kp = ERP / CFM;
//Kd = (1-ERP) / CFM;

////////////////////////////////////////////////////////////////
//AnglerPolicy
//!X軸
struct XAxisJointUpdater
{
	template<typename BaseType>
		static void
		update( btTransform& frame,
				const btQuaternion& zero,
				const BaseType& rad )
		{
			frame.getBasis().setRotation(
					zero
					* btQuaternion(
						btVector3( 1.f, 0.f, 0.f ),
						rad ) );
		}

	static constexpr int getAxisNumber()
	{
		return 3;
	}
};

struct YAxisJointUpdater
{
	template<typename BaseType>
		static void
		update( btTransform& frame,
				const btQuaternion& zero,
				const BaseType& rad )
		{
			frame.getBasis().setRotation(
					zero
					* btQuaternion(
						btVector3( 0.f, -1.f, 0.f ),
						rad ) );
		}
	static constexpr int getAxisNumber()
	{
		return 4;
	}
};

struct ZAxisJointUpdater
{
	template<typename BaseType>
		static void
		update(
				btTransform& frame,
				const btQuaternion& zero,
				const BaseType& rad )
		{
			frame.getBasis().setRotation(
					zero
					* btQuaternion(
						btVector3( 0.f, 0.f, 1.f ),
						-rad ) );
		}
	static constexpr int getAxisNumber()
	{
		return 5;
	}
};

////////////////////////////////////////////////////////////////
// JointConstractPolicy
struct HingeJoint
{
	typedef joint::ConstraintWrapper<btHingeConstraint> jointType;
	typedef jointType::ConstraintArg detailType;

	using argType =
		boost::fusion::vector<btScalar, btScalar>;

	//joint引数生成
	static detailType
		getConstractArg()
		{
			return detailType();
		}

	//設定
	static argType modify( jointType& j )
	{
		auto tree = system::GetSettingTree(
				"MIZUNUKI.Physics.Model.Joints.HingeJoint");
		assert(tree);
		auto const& jointProper = *tree;
		btHingeConstraint& c = j.getConstraint();
		c.setLimit( 0.0f, 0.0f, 1.0f, 0.94f, 1.0f );
		//_softness=0.9f, btScalar _biasFactor=0.3f, btScalar _relaxationFactor=1.0f

		//ERPとCFMのセッティング
		btScalar erp = jointProper.get<btScalar>( "erp" ); //0.99f;
		btScalar cfm = jointProper.get<btScalar>( "cfm" ); //0.0f;

		c.setParam( BT_CONSTRAINT_STOP_ERP, erp );
		c.setParam( BT_CONSTRAINT_STOP_CFM, cfm );
		c.setParam( BT_CONSTRAINT_CFM, 0.0f );

		return boost::fusion::make_vector( erp, cfm );
	}

	//初期位置に
	template<class BodyType>
		static void
		init( jointType& j, BodyType& a, BodyType& b )
		{
			//角度変更した位置にワープ
			btHingeConstraint& c = j.getConstraint();
			b.setWorldTransform(
					a.getWorldTransform() * c.getAFrame()
					* c.getBFrame().inverse() );

		}

	//角度
	template<class AnglerPolicy>
		static void
		updateAngle( jointType& j,
				const btQuaternion& zero,
				btScalar angle,
				const boost::type<AnglerPolicy>& )
		{
			AnglerPolicy::update(
					j.getConstraint().getBFrame(),
					zero, angle );
		}
};

////////////////
struct G6DJoint
{
	struct constraintParam
	{
		btScalar erp_;
		btScalar cfm_;
		btScalar Kp_; //<! spring conf
		btScalar Kd_; //<! damper conf
	};

	using argType = constraintParam;

	typedef joint::ConstraintWrapper<btGeneric6DofConstraint> jointType;
	typedef jointType::ConstraintArg detailType;

	//joint引数生成
	static detailType
		getConstractArg()
		{
			return detailType();
		}

	//設定
	template<typename AnglerPolicy>
		static argType modify(
				jointType& j,
				ElementParameter const& param,
				boost::type<AnglerPolicy> const& )
		{
			btGeneric6DofConstraint& c = j.getConstraint();
			int i;

			for ( i = 0; i < 3; i++ )
				c.setLimit( i + 3, 0.000f, 0.000f );


			auto tree = system::GetSettingTree(
					"MIZUNUKI.Physics.Model.Joints.AxisJoint");
			assert(tree);
			auto const& jointProper = *tree;
			//ERPとCFMのセッティング
			btScalar erpR =
				jointProper.get<btScalar>( "Rotate.erp" );
			btScalar cfmR =
				jointProper.get<btScalar>( "Rotate.cfm" );
			btScalar erpL =
				jointProper.get<btScalar>( "Linear.erp" );
			btScalar cfmL =
				jointProper.get<btScalar>( "Linear.cfm" );
			for ( i = 0; i < 3; i++ )
			{
				c.setParam( BT_CONSTRAINT_STOP_ERP, erpL, i );
				c.setParam( BT_CONSTRAINT_STOP_CFM, cfmL, i );
				c.setParam( BT_CONSTRAINT_CFM, 0.0f, i );
			}
			for ( i = 3; i < 6; i++ )
			{
				c.setParam( BT_CONSTRAINT_STOP_ERP, erpR, i );
				c.setParam( BT_CONSTRAINT_STOP_CFM, cfmR, i );
				c.setParam( BT_CONSTRAINT_CFM, 0.0f, i );
			}

			//h*Kp = ERP / CFM;
			// Kd = (1-ERP) / CFM;
			btScalar const dCfm = cfmR + SIMD_EPSILON;
			return constraintParam{ erpR, cfmR, erpR/dCfm, (btScalar(1.0)-erpR)/dCfm};
		}

	//初期位置に
	template<class BodyType>
		static void
		init( jointType& j, BodyType& a, BodyType& b )
		{
			//角度変更した位置にワープ
			btGeneric6DofConstraint& c = j.getConstraint();
			b.setWorldTransform(
					a.getWorldTransform() * c.getFrameOffsetA()
					* c.getFrameOffsetB().inverse() );

		}

	//角度更新用
	template<class AnglerPolicy>
		static void
		updateAngle( jointType& j,
				const btQuaternion& zero,
				btScalar angle,
				const boost::type<AnglerPolicy>& )
		{
			AnglerPolicy::update(
					j.getConstraint().getFrameOffsetB(),
					zero,
					angle );
		}

	template<typename AnglerPolicy>
		static void
		setStiffnessAndDamper(
				jointType& j,
				btScalar const spring,
				btScalar const damper,
				argType const& arg,
				boost::type<AnglerPolicy> const& )
		{
			// ERP = h*Kp / (h*kp + kd)
			//  CFM = 1 / (h*Kp + kd)
			btScalar const kp(arg.Kp_ * spring);
// 			btScalar const kd(arg.Kd_ * btScalar(1.0)/(damper+SIMD_EPSILON));
			btScalar const kd(arg.Kd_ * (btScalar(0.5)+damper));
			btScalar const erp( kp / (kp + kd));
			btScalar const cfm( btScalar(1.)/(kp+kd));

			j.getConstraint().setParam(
					BT_CONSTRAINT_STOP_ERP,
					erp,
					AnglerPolicy::getAxisNumber());

			j.getConstraint().setParam(
					BT_CONSTRAINT_STOP_CFM,
					cfm,
					AnglerPolicy::getAxisNumber());
		}

};

////////////////////////////////////////////////////////////////
// Spring
struct G6DSpringJoint
{
	struct baseParam
	{
		btScalar spring_;
		btScalar damper_;
	};
	using argType = baseParam;

	typedef joint::ConstraintWrapper<btGeneric6DofSpringConstraint> jointType;
	typedef jointType::ConstraintArg detailType;

	//joint引数生成
	static detailType
		getConstractArg()
		{
			return detailType();
		}

	//設定
	template<typename AnglerPolicy>
		static argType
		modify(
				jointType& j,
				ElementParameter const& param,
				boost::type<AnglerPolicy> const& )
		{
			btGeneric6DofSpringConstraint& c = j.getConstraint();
			int i;

			for ( i = 0; i < 3; i++ )
				c.setLimit( i + 3, 0.000f, 0.000f );

			auto tree = system::GetSettingTree(
					"MIZUNUKI.Physics.Model.Joints.AxisJoint");
			assert(tree);
			auto const& jointProper = *tree;
			//ERPとCFMのセッティング
			btScalar erpR = jointProper.get<btScalar>( "Rotate.erp" ); //0.98f
			btScalar cfmR = jointProper.get<btScalar>( "Rotate.cfm" ); //0.0
			btScalar erpL = jointProper.get<btScalar>( "Linear.erp" ); //,0.7f
			btScalar cfmL = jointProper.get<btScalar>( "Linear.cfm" ); //0.0
			for ( i = 0; i < 3; i++ )
			{
				//m_constraint->setParam(BT_CONSTRAINT_STOP_ERP, 0.2f,i);
				c.setParam( BT_CONSTRAINT_STOP_ERP, erpL, i );
				c.setParam( BT_CONSTRAINT_STOP_CFM, cfmL, i );
				c.setParam( BT_CONSTRAINT_CFM, 0.0f, i );
			}
			for ( i = 3; i < 6; i++ )
			{
				c.setParam( BT_CONSTRAINT_STOP_ERP, erpR, i );
				c.setParam( BT_CONSTRAINT_STOP_CFM, cfmR, i );
				c.setParam( BT_CONSTRAINT_CFM, 0.0000001f, i );
			}

			constexpr int axis = AnglerPolicy::getAxisNumber();
			c.setLimit( axis , 1.000f, -1.000f );
			c.enableSpring( axis , true );

			baseParam ret_param;
			if ( auto s = param.getProperty( "connectStiffness", boost::type<btScalar>() ) )
				ret_param.spring_ = (*s) * 1000.f;
			else
				ret_param.spring_ = 10.f * 1000.f; //適当

			if ( auto d = param.getProperty( "connectDamper", boost::type<btScalar>() ) )
				ret_param.damper_ = *d;
			else
				ret_param.damper_ = 100.f; //適当

			return ret_param;
		}

	//初期位置に
	template<class BodyType>
		static void
		init( jointType& j, BodyType& a, BodyType& b )
		{
			//角度変更した位置にワープ
			btGeneric6DofSpringConstraint& c = j.getConstraint();
			b.setWorldTransform(
					a.getWorldTransform() * c.getFrameOffsetA()
					* c.getFrameOffsetB().inverse() );

		}

	//角度更新用
	template<class AnglerPolicy>
		static void
		updateAngle( jointType& j,
				const btQuaternion& zero,
				btScalar angle,
				const boost::type<AnglerPolicy>& )
		{
			AnglerPolicy::update(
					j.getConstraint().getFrameOffsetB(),
					zero,
					angle );
		}

	template<typename AnglerPolicy>
		static void
		setStiffnessAndDamper(
				jointType& j,
				btScalar const spring,
				btScalar const damper,
				argType const& arg,
				boost::type<AnglerPolicy> const& )
		{
			constexpr int axis = AnglerPolicy::getAxisNumber();

			j.getConstraint().setStiffness( axis, spring*arg.spring_ );
			j.getConstraint().setDamping(
					axis, 1.f / (damper*arg.damper_ + SIMD_EPSILON) );

		}

};

} // namespace detail

class FlexibleXJointBehavior :
	public detail::FlexibleJointBase<
	detail::G6DJoint,
	detail::XAxisJointUpdater>{ };
class FlexibleYJointBehavior :
	public detail::FlexibleJointBase<
	detail::G6DJoint,
	detail::YAxisJointUpdater>{ };
class FlexibleZJointBehavior :
	public detail::FlexibleJointBase<
	detail::G6DJoint,
	detail::ZAxisJointUpdater>{ };

namespace helper
{

struct JointBehaviorDispather
{

	template<typename EventType, typename JointPolicy, typename UpdatePolicy>
		static void jointInvoke(
				EventType const&,
				detail::FlexibleJointBase<
				JointPolicy, UpdatePolicy>&,
				ElementParameter&
				)
		{
		}

	template< typename JointPolicy, typename UpdatePolicy>
		static void jointInvoke(
				event::StepStart const& e,
				detail::FlexibleJointBase<
				JointPolicy, UpdatePolicy>& b,
				ElementParameter& p
				)
		{
			b.setTargetParameter(
					p, e.fps_, e.subStep_);
		}


	template< typename JointPolicy, typename UpdatePolicy>
		static void jointInvoke(
				event::PreTick const& e,
				detail::FlexibleJointBase<
				JointPolicy, UpdatePolicy>& b,
				ElementParameter& p
				)
		{
			b.updateAngle();
		}
};

} // namespace helper
} // namespace behavior
} // namespace element_v2
} // namespace model
} // namespace akasha
#endif
