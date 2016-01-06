/*
 * BasicElementFiguer.hpp
 *
 *  Created on: 2013/03/11
 *      Author: ely
 */

#ifndef COREELEMENTFIGUER_HPP_
#define COREELEMENTFIGUER_HPP_

#include <wisp_v3/wisp.hpp>
#include <wisp_v3/irrExt.hpp>

#include "LinearMath/btTransform.h"
#include "LinearMath/btMotionState.h"
#include <boost/scoped_ptr.hpp>

namespace akasha
{
//fwd
class NodeUpdateTask;
namespace model
{
namespace element
{
namespace figuer
{

namespace detail
{
//fwd
class FiguerImpl;
}

class BasicElementFiguer
{

public:
	BasicElementFiguer();
	~BasicElementFiguer();

	void create(
			std::string const& mshName,
			irrExt::IrrExtention&,
			boost::shared_ptr<NodeUpdateTask> const&
			);
	void create(irrExt::IrrExtention&, boost::shared_ptr<NodeUpdateTask> const&
			);

	void setTransform( btTransform const& t );

private:
	boost::scoped_ptr<detail::FiguerImpl> impl_;
};


struct FiguerMotionState : public btMotionState
{

	btTransform buf_;
	btTransform offset_;
	BasicElementFiguer* figuer_;

	FiguerMotionState() : buf_( btTransform::getIdentity() ), offset_( btTransform::getIdentity() ), figuer_( 0 ) {}

	virtual void getWorldTransform( btTransform& worldTrans ) const
	{
		//worldTrans = buf_;
		worldTrans = btTransform::getIdentity();
	}


	virtual void setWorldTransform( const btTransform& worldTrans )
	{
		figuer_->setTransform( worldTrans * offset_ );
	}

};
}
}
}
}

#endif /* COREELEMENTFIGUER_HPP_ */
