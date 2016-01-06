#ifndef ENVIRONMENT_LAND__HPP
#define ENVIRONMENT_LAND__HPP



#include <wisp_v3/wisp.hpp>

#include "LandWater.hpp"
#include "Landform.hpp"

#include "../contactEvent/ContactEventManager.hpp"

#include <boost/filesystem.hpp>
#include <boost/noncopyable.hpp>

namespace akasha
{
namespace land
{
//ƒ‰ƒ“ƒh
class EnvironmentLand : private boost::noncopyable
	{

		//entityService
		irrExt::IrrExtention& m_irrEx;
		wisp::v3::WispHandle& m_client;

		//wator
		land::LandWater m_water;
		//form
		boost::optional<land::Landform> m_form;
		//light
		wisp::ObjectID m_lightID;
		//sky
		wisp::ObjectID m_sky;

	public:
		EnvironmentLand(
				irrExt::IrrExtention& irr,
				wisp::v3::WispHandle& c) :
			m_irrEx(irr),
			m_client(c){ }

		bool loadFromFile(const irr::io::path& p);

		void update(const btScalar step)
		{
			m_form->update(step);
		}


		boost::optional<btScalar> getLandHeight(
				const btScalar x, const btScalar z) const;

		boost::optional<btScalar>
			getLandHeight( btVector3 const& from, btScalar depth) const;

		void registContactGroup(contactEvent::ContactEventManager&) const;

	};
}
}

#endif
