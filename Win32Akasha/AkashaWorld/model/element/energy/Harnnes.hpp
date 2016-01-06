/*
 * Harnnes.hpp
 *
 *  Created on: 2012/04/14
 *      Author: ely
 */

#ifndef HARNNES_HPP_
#define HARNNES_HPP_





#include "LinkType.hpp"
#include "TractionLink.hpp"

#include <Riceball/VTBL_Load.hpp>
namespace akasha
{
namespace model
{
namespace energy
{


TEMPLATE_VTBL(
		EnergyLinkWrapper,
		(LinkType ,  getType,      (0),        0)
		(float,      getTension,   (0),        0)
		(void,       applyTension, ((float)),  1)
		(float,       getConductance, (0),     0)
		(void,       applyValue,    ((float)), 1)
		)

	class EnergyLinkPair{

		int _id0;
		int _id1;
		EnergyLinkWrapper _0;
		EnergyLinkWrapper _1;
		float tension_;
	public:
		EnergyLinkPair() = delete;
		EnergyLinkPair(int i0, int i1, EnergyLinkWrapper const& w0, EnergyLinkWrapper const& w1) :
			_id0(i0), _id1(i1), _0(w0), _1(w1), tension_(0){

			}


		void
			tensionUpdate()
			{

				tension_ = _1.getTension()-_0.getTension();
				_0.applyTension(tension_);
				_1.applyTension(-tension_);
			}

		void
			valueUpdate()
			{
				float c0= _0.getConductance(), c1 = _1.getConductance();
				assert( (c0+c1) !=0.f );
				float v =  tension_ * ( (c0*c1) / (c0 + c1) );
				_0.applyValue( v );
				_1.applyValue( -v );

			}

	};
//			struct EnergyLinkWrapper{ };

struct NonType{ };
typedef boost::variant<
	NonType,
	boost::shared_ptr<Gear>
		> LinkInterfaceType;
}
}

}

#endif /* HARNNES_HPP_ */
