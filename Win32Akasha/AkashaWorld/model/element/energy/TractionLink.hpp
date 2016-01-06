/*
 * TractionLink.hpp
 *
 *  Created on: 2012/07/30
 *      Author: ely
 */

#ifndef TRACTIONLINK_HPP_
#define TRACTIONLINK_HPP_

//#include "../../EnergyLinkGraph.hpp"
#include "LinkType.hpp"
#include <Riceball/VTBL_Load.hpp>

#include <boost/graph/adjacency_list.hpp>
#include <boost/range/algorithm/for_each.hpp>

namespace akasha{ namespace model{ namespace energy{

/*
	struct TractionRevolution
	{
	float velocity_;
	float position_;
	};


	struct TractionSoruce
	{
	float traction_;
	TractionRevolution rev_;

	float
	getOutputTraction() const
	{
	return traction_;
	}

	void
	applyRevolution(TractionRevolution const& r)
	{
	rev_ = r;
	}
	};

	TEMPLATE_VTBL(TractionTransferWrapper,
	(void, calcTraction, ((float&)), 0)
	(void, calcRevolution, ((TractionRevolution&)), 0)
	)


	TEMPLATE_VTBL(TractionEffecterWrapper,
	(void, applyTraction, ((float)), 1)
	(TractionRevolution, getRevolution, (0), 0)
	(void, forcedRevolution, ((TractionRevolution const&)), 1)
	)



	struct TractionPath
	{

	boost::shared_ptr<TractionSoruce> source_;
	std::vector<TractionTransferWrapper> paths_;
	TractionEffecterWrapper effector_;

	void
	tractionPhase()
	{
	float t = source_->getOutputTraction();
	boost::for_each(paths_, [&t](TractionTransferWrapper const& tt){ tt.calcTraction(t); });
	effector_.applyTraction(t);
	}

	void
	revolutionPhace()
	{
	TractionRevolution r(effector_.getRevolution());
	boost::for_each(paths_, [&r](TractionTransferWrapper const& tt){ tt.calcRevolution(r); });
	source_->applyRevolution(r);
	}

	};
	*/

struct Gear
{

	float inertia_;
	float velocity_;
	float rotation_;

	float ratio_;

	Gear() : inertia_(1.f), velocity_(0.f), rotation_(0.f), ratio_(1.f){}

	float
		getMomentum() const
		{
			return inertia_ * velocity_;
		}

	void
		setVelocity(float v)
		{
			velocity_ = v;
		}
	void
		applyMomentum(float momentum)
		{
			velocity_ += momentum / inertia_;
		}

	void
		update(float step)
		{
			rotation_ += velocity_ * step;

			while(rotation_ > SIMD_PI)
				rotation_ -= SIMD_2_PI;
			while(rotation_ <= -SIMD_PI)
				rotation_ += SIMD_2_PI;
		}

};


struct GearAssimbly
{
	typedef boost::shared_ptr<Gear> gearPtrType;
	std::vector<gearPtrType> geares_;


	float
		getAssimblyInertia() const
		{
			float r = 0.f;
			boost::for_each(geares_, [&r](gearPtrType const& g){ r += (g->inertia_) * 1.0 / (g->ratio_*g->ratio_); });
			return r;
		}


	void
		update(float step)
		{
			float m = 0.f;
			boost::for_each(geares_, [&m](gearPtrType& g){
					m += g->inertia_ * g->velocity_ / g->ratio_;
					});

			float v = m / getAssimblyInertia();
			boost::for_each(geares_, [v, step](gearPtrType& g){
					g->velocity_ = v / g->ratio_;
					g->update(step);
					});


		}
};


}}}

#endif /* TRACTIONLINK_HPP_ */
