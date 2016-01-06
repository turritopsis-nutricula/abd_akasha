/*
 * btFractureWelding.hpp
 *
 *  Created on: 2012/09/09
 *      Author: ely
 */

#ifndef BTFRACTUREWELDING_HPP_
#define BTFRACTUREWELDING_HPP_

#include "btFractureBody.hpp"

class btFractureWelding
{
	btFractureBody& m_body0;
	btFractureBody& m_body1;

	 btScalar m_breakApplideImpulse;
public:
	btFractureWelding() = delete;
	btFractureWelding(btFractureBody& b0, btFractureBody& b1) :
		m_body0(b0), m_body1(b1)
	{

	}

	/*void
	runWeld()
	{
		m_body1.resetRootBody(&m_body0);
	}*/

};

#endif /* BTFRACTUREWELDING_HPP_ */
