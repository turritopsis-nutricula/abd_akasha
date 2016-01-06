#ifndef AKASHA_CONVERT_IRRLICHT_X3D__HPP
#define AKASHA_CONVERT_IRRLICHT_X3D__HPP

namespace akasha
{
template<>
	X3DAUDIO_VECTOR convertType(const irr::core::vector3df& from)
{
	X3DAUDIO_VECTOR v = {from.X, from.Y, from.Z};
	return std::move(v);
}
}


#endif
