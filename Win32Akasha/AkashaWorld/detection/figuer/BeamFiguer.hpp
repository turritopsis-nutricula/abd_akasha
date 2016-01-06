/*
 * BeamFiguer.hpp
 *
 *  Created on: 2013/03/15
 *      Author: ely
 */

#ifndef BEAMFIGUER_HPP_
#define BEAMFIGUER_HPP_

//wisp
#include <wisp_v3/wisp.hpp>
#include <wisp_v3/irrExt.hpp>

#include "LinearMath/btMotionState.h"
#include "LinearMath/btTransform.h"


#include <boost/scoped_ptr.hpp>
#include <string>

namespace akasha
{
//fwd
class NodeUpdateTask;

namespace detection
{
class BeamFiguer : public btMotionState
	{
	public:
		BeamFiguer() = delete;
		BeamFiguer(
				std::string const& texPath,
				irrExt::IrrExtention&,
				boost::shared_ptr<NodeUpdateTask> const&);
		virtual ~BeamFiguer();

		virtual void getWorldTransform(btTransform&) const;
		virtual void setWorldTransform(btTransform const&);

	private:
		class BeamFiguerImpl;

		boost::scoped_ptr<BeamFiguerImpl> m_impl;

	};
}
}

#endif /* BEAMFIGUER_HPP_ */
