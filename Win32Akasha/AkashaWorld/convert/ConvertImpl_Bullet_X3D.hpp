#ifndef CONVERT_IMPL_BULLET_X3D__HPP
#define CONVERT_IMPL_BULLET_X3D__HPP

//TODO: ˆêŽž‚µ‚Ì‚¬
struct X3DAUDIO_VECTOR
{
	float x_;
	float y_;
	float z_;
};


namespace akasha
{
template<>
	X3DAUDIO_VECTOR convertType(const btVector3& from)
{
	X3DAUDIO_VECTOR v = {from.getX(), from.getY(), from.getZ()};
	return v;
}


namespace convert
{

template<>
	X3DAUDIO_VECTOR&& convertImpl(const btVector3& from)
{
	X3DAUDIO_VECTOR v = {from.getX(), from.getY(), from.getZ()};
	return std::move(v);
}


}
}
#endif
