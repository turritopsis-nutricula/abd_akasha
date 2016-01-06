#include "FlexibleJointBehavior.hpp"

#include "../detail/AdapterParameter.hpp"

#include "../../../bullet/btio.hpp"
#include <boost/range/algorithm/for_each.hpp>

//fwd
void GetTransform(
		btTransform& baseTransform,
		std::vector<boost::shared_ptr<
		akasha::model::element_v2::AdapterParameter>> const& adapters
		);
float NormalizeAngle(float r)
{
	while ( r <= SIMD_PI )
		r += SIMD_2_PI;
	while ( r > SIMD_PI )
		r -= SIMD_2_PI;
	return r;
}
float NormalizeDegAngle(float deg)
{
	return NormalizeAngle(btRadians(deg));
}
namespace akasha {
namespace model {
namespace element_v2 {
namespace behavior {
namespace detail {


template<typename JointPolicy, typename AxisPolicy>
	void FlexibleJointBase<JointPolicy, AxisPolicy>::init(
			element::ElementData const& data, ElementParameter const& param)
	{
		m_targetAngle = 0.f;
		m_angleBuffer = 0.f;
		m_needEnergy = 0.f;
		m_energyRatio = 100.f;
		m_breakImpulse = FLT_MAX;

		//angleがあったら設定
		if (
				auto ang = data.getParameterValue( boost::type<element::parameter::angle>() )
			)
		{
			m_targetAngle = btRadians( *ang );
			m_angleBuffer = m_targetAngle;
			//BaseType::getParameters().setValue(boost::type<parameter::angle>(), *a);
		}


	}

template<typename JointPolicy, typename AxisPolicy>
	void FlexibleJointBase<JointPolicy,AxisPolicy>::connect(
			element_v2::DirectType const& direct,
			btMatrix3x3 const& relativeTrans,
			boost::shared_ptr<btFractureBody> parentBody,
			boost::shared_ptr<btFractureBody> mineBody,
			ElementParameter const& param,
			std::vector<boost::shared_ptr<element_v2::ElementParameter>> const& parentsParams)
	{
		//TODO
		if (!parentBody)
		{
			assert(false && "FlexibleJointBehavior#connect parentBody null.");
			return;
		}

		for (auto const& param: parentsParams)
		{
			if (param->isAdapter())
				m_adapters.push_back(param->getAdapterParameter());
		}

		btTransform parentTrans =
			parentBody->getWorldTransform();
		GetTransform(parentTrans, m_adapters);

		btTransform mine = direct::GetChildTransform( direct );
		btTransform joint = direct::GetJointTransform( direct );
		bool const isRelative = direct::IsRelativeConnect( direct );
		if (isRelative)
		{
			btTransform t(relativeTrans);
			joint = t * joint;
			mine = t  * mine;
		}

		joint.getOrigin() *= 0.6f;
		mine.getOrigin() *= 0.6f;

		// save zero basis
		(mine.inverseTimes(joint)).getBasis().getRotation( m_zeroBasis );

		//parent joint frame save
		m_parentJointFrame = joint;

		//zero位置へ移動
		mineBody->setWorldTransform( parentTrans * mine );


		m_joint.constract(
				parentBody,
				mineBody,
				parentTrans * joint,
				JointPolicy::getConstractArg());


		//セッティング
		m_arg =
			JointPolicy::modify( m_joint, param, boost::type<AxisPolicy>() );


		//コンストライン更新
		auto const& p = param.getParameter();
		{
			m_targetAngle = NormalizeDegAngle(
					p.getValue(boost::type<element::parameter::angle>()));
			setConstraintAngle( m_targetAngle );
			updateSpringAndDamper(
					p.getValue(boost::type<element::parameter::spring>()),
					p.getValue(boost::type<element::parameter::damper>()));
		}
		// connect
		m_joint.connect();

		// position Init
		JointPolicy::init(
				m_joint,
				*( ::helper::FractureBodyHelper {parentBody.get()} .getRootBody() ),
				*( ::helper::FractureBodyHelper {mineBody.get()} .getRootBody() )
				);
	}

template<typename JointPolicy, typename AxisPolicy>
	void FlexibleJointBase<JointPolicy,AxisPolicy>::updateParameter(
			ElementParameter const& elemParam, float fps, float subStep)
	{
		auto const& p = elemParam.getParameter();

		//damageCheck();
		updateSpringAndDamper(
				p.getValue(boost::type<element::parameter::spring>()),
				p.getValue(boost::type<element::parameter::damper>())
				);
		m_angleBuffer = m_targetAngle;
		m_targetAngle =
			NormalizeDegAngle(p.getValue(boost::type<element::parameter::angle>()));

		m_angleStep = NormalizeAngle(m_targetAngle -m_angleBuffer) * subStep / fps;
	}
template<typename JointPolicy, typename AxisPolicy>
	void FlexibleJointBase<JointPolicy,AxisPolicy>::updateAngle()
	{
		m_angleBuffer += m_angleStep;
		setConstraintAngle(m_angleBuffer);
	}
template<typename JointPolicy, typename AxisPolicy>
	void FlexibleJointBase<JointPolicy,AxisPolicy>::setConstraintAngle(
			btScalar const ang){

		btTransform base( m_parentJointFrame );
		GetTransform(base, m_adapters);

		m_joint.getConstraint().getFrameOffsetA() = base;
		JointPolicy::updateAngle(
				m_joint,
				m_zeroBasis,
				ang, boost::type<AxisPolicy>() );

	}
template<typename JointPolicy, typename AxisPolicy>
	void FlexibleJointBase<JointPolicy,AxisPolicy>::updateSpringAndDamper(
			btScalar const sp, btScalar const dam)
	{
		//TODO:: targetVelocity = (-dx*spring) * damper;  maxForce=(-dx*spring)
		// add "GETA"
		JointPolicy::setStiffnessAndDamper(
				m_joint,
				btClamped( sp, 0.f, 1.f ),
				btClamped( dam, 0.f, 10.f),
				m_arg,
				boost::type<AxisPolicy>()
				);
	}


} // namespace detail

// G6DJoint
//FlexibleXJointBehavior
template class detail::FlexibleJointBase<
detail::G6DJoint,
	detail::XAxisJointUpdater>;
//FlexibleYJointBehavior
template class detail::FlexibleJointBase<
detail::G6DJoint,
	detail::YAxisJointUpdater>;
//FlexibleZJointBehavior
template class detail::FlexibleJointBase<
detail::G6DJoint,
	detail::ZAxisJointUpdater>;

// G6DSpringJoint
//FlexibleXJointBehavior
template class detail::FlexibleJointBase<
detail::G6DSpringJoint,
	detail::XAxisJointUpdater>;
//FlexibleYJointBehavior
template class detail::FlexibleJointBase<
detail::G6DSpringJoint,
	detail::YAxisJointUpdater>;
//FlexibleZJointBehavior
template class detail::FlexibleJointBase<
detail::G6DSpringJoint,
	detail::ZAxisJointUpdater>;
} // namespace behavior

} // namespace element_v2
} // namespace model
} // namespace akasha

void GetTransform(
		btTransform& baseTransform,
		std::vector<boost::shared_ptr<
		akasha::model::element_v2::AdapterParameter>> const& adapters
		)
{
	for (auto& a : adapters)
		a->jointTransform( baseTransform );
}
