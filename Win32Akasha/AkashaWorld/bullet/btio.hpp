#ifndef BULLET_IO_HPP_
#define BULLET_IO_HPP_

#include <LinearMath/btTransform.h>
#include <ostream>


#define OSTREAM(type, value_name, os_name) \
template<typename CharType, typename Traits> \
std::basic_ostream<CharType, Traits>& \
operator<<(std::basic_ostream<CharType, Traits>& os_name, type const& value_name)

//btVector3
OSTREAM(btVector3, v, os)
{
	os << v.getX() << "," << v.getY() << "," << v.getZ();
	return os;
}

// Matrix3x3
OSTREAM(btMatrix3x3, m, os)
{
	os << "|" << m.getRow(0) << "|\n" <<
		"|" << m.getRow(1) << "|\n" <<
		"|" << m.getRow(2) << "|";
	return os;
}

// Quaternion
OSTREAM(btQuaternion, q, os)
{
	os << q.getX() << "," << q.getY() << "," << q.getZ() << ":" << q.getW();
	return os;
}

// Transform
OSTREAM(btTransform, t, os){
	os << t.getOrigin() << "\n" << t.getBasis();
	return os;
}
#undef OSTREAM

#endif /* end of include guard */
