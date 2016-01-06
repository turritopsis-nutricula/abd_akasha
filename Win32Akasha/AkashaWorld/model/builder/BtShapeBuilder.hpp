#ifndef AKASHA_MODEL_BUILDER_BT_SHAPEBUILDER
#define AKASHA_MODEL_BUILDER_BT_SHAPEBUILDER

#include "../../bullet/fluidDynamics/buoyant/BuoyantAuthoizer.hpp"

#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletCollision/CollisionShapes/btCylinderShape.h>

#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>
#include <boost/functional/hash.hpp>
#include <boost/type.hpp>
#include <boost/static_assert.hpp>

#include <boost/mpl/map.hpp>

#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/value_at_key.hpp>

namespace akasha {
namespace model {
namespace builder {
//当たり判定タイプ
namespace shapeBase
{
namespace detail
{
template<int N> struct ShapeTag{
	constexpr static int value = N;
};
}
struct Panel : detail::ShapeTag<0>{ };
struct Disc: detail::ShapeTag<1>{};
struct Pin: detail::ShapeTag<2>{ };
struct Sphere : detail::ShapeTag<3>{ };
constexpr static int TagNumber = 3;

struct ShapeParameter
{
	int size_;
	bool
		operator==(ShapeParameter const& rh) const
		{
			return size_ == rh.size_;
		}
};
inline std::size_t hash_value( ShapeParameter const& d )
{
	// 複数の値のハッシュ値を組み合わせてハッシュ値を計算するには、
	// boost::hash_combine を使います。
	std::size_t h = 0;
	boost::hash_combine(h, d.size_);
	return h;
}

}


namespace detail
{
template<int N>
	btConvexShape*
	CreateRawShape(shapeBase::ShapeParameter const&);
template<int N>
	boost::shared_ptr<btCollisionShape>
	CreateShape(shapeBase::ShapeParameter const& p)
	{
		return boost::shared_ptr<btCollisionShape>(
				CreateRawShape<N>(p)
				);
	}
}
class BtShapeBuilder
{
	typedef boost::unordered_map<
		shapeBase::ShapeParameter,
		boost::shared_ptr<btCollisionShape>> cacher_type;
	boost::array<cacher_type, shapeBase::TagNumber+1> m_caches;
	boost::shared_ptr<bullet::buoyant::BuoyantAuhoizer> m_buoyantAuhoizer;

public:
	BtShapeBuilder() :
		m_buoyantAuhoizer(boost::make_shared<bullet::buoyant::BuoyantAuhoizer>())
	{}

	template<typename ShapeTag>
		boost::shared_ptr<btCollisionShape>
		getShape(boost::type<ShapeTag> const&, shapeBase::ShapeParameter const& p)
		{
			constexpr int N = ShapeTag::value;
			cacher_type& m = m_caches[N];
			if (m.count(p)==0)
			{
				m[p] = m_buoyantAuhoizer->auhoize(detail::CreateRawShape<ShapeTag::value>(p));
			}

			return m.at(p);

		}

};
}
}}
#endif /* end of include guard */
