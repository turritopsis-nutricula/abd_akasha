#ifndef RENDER_TYPES__HPP
#define RENDER_TYPES__HPP

#include "EMaterialTypes.h"

namespace akasha
{
struct test;

template<typename T>
	struct RenderType
	{
		static irr::video::E_MATERIAL_TYPE Type_;
	};

template<typename T>
	irr::video::E_MATERIAL_TYPE RenderType<T>::Type_ = (irr::video::E_MATERIAL_TYPE)0;
}
#endif
