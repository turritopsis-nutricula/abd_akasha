#ifndef AKASHA_WORLD_COLLISIONEVENT_EVNET_GROUP_HPP_
#define AKASHA_WORLD_COLLISIONEVENT_EVNET_GROUP_HPP_

#define BIT(x) (1<<(x))
namespace akasha
{
namespace contactEvent
{
enum struct EventGroup : unsigned int
	{
		BasicElement = BIT( 0 ),
		BasicLand    = BIT( 1 )
	};
} // namespace collisionEvent
} // namespace akasha
#undef BIT

#endif /* end of include guard */
