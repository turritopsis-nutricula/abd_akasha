#ifndef AKASHA_WORLD_CONTACT_EVENT_MANAGER_HPP_
#define AKASHA_WORLD_CONTACT_EVENT_MANAGER_HPP_

#include "EventGroup.hpp"

#include <boost/weak_ptr.hpp>
#include <boost/function.hpp>
#include <boost/unordered_map.hpp>

#include <BulletCollision/CollisionDispatch/btCollisionObject.h>

//fwd
class btPersistentManifold;
namespace akasha
{
namespace contactEvent
{

struct BodyPair
{
	using bodyPtr_t = btCollisionObject const*;
	btPersistentManifold* manifold_;
	bodyPtr_t object0_;
	bodyPtr_t object1_;

	bool operator<(BodyPair const& r) const
	{
		return this->manifold_ < r.manifold_;
	}
	bool operator==(BodyPair const& o) const
	{
		return this->manifold_ == o.manifold_;
	}

	void set(bodyPtr_t l, bodyPtr_t r)
	{

		object0_ = std::move(l);
		object1_ = std::move(r);

	}
};
class ContactEventManager
{
public:
	using excuteType =
		boost::function<
		void (btPersistentManifold*,btCollisionObject const*, btCollisionObject const*)>;
	using inputBufferType = std::vector< BodyPair >;
private:
	using bitType = unsigned int;
	std::vector<boost::weak_ptr<btCollisionObject>> m_bodyCache;
	boost::unordered_map< void const*, bitType> m_bodyMap;
	boost::unordered_map< bitType, excuteType> m_excuterMap;
public:
	template<typename BodyType>
		void registBody(
				EventGroup group,
				boost::shared_ptr<BodyType> const& body)
		{
			regist(group, body);
		}
	void registExcuter(
			EventGroup group0,
			EventGroup group1,
			excuteType e)
	{
		m_excuterMap[
			static_cast<bitType>(group0) |
			static_cast<bitType>(group1)] = std::move(e);
	}
	void excute( inputBufferType const& pairBuffer) const
	{
		for (auto const& pair : pairBuffer)
		{
			auto const& first = pair.object0_;
			auto const& second = pair.object1_;
			auto b0 = m_bodyMap.find(first);
			auto b1 = m_bodyMap.find(second);

			if (b0 == m_bodyMap.cend() || b1 == m_bodyMap.cend())
				continue;

			auto exe = m_excuterMap.find( b0->second | b1->second );
			if (exe != m_excuterMap.cend())
			{
				if (b0->second < b1->second)
					exe->second(pair.manifold_, first, second);
				else
					exe->second(pair.manifold_, second, first);
			}

		}
	}
	std::size_t getRegistBodyCount() const
	{
		return m_bodyMap.size();
	}
	std::size_t clear() //TODO : need name change
	{
		std::vector<boost::weak_ptr<btCollisionObject>> newCache;
		boost::unordered_map< void const*, bitType> newMap;

		for (auto& w : m_bodyCache)
		{
			if (auto lock = w.lock())
			{
				newCache.push_back(lock);
				assert(m_bodyMap.count(lock.get()) != 0);
				newMap[lock.get()] = m_bodyMap[lock.get()];
			}
		}

		std::size_t old( m_bodyCache.size() );
		m_bodyCache.swap( newCache );
		m_bodyMap.swap( newMap );

		return old - m_bodyCache.size();
	}
private:
	void regist(EventGroup eg,boost::shared_ptr<btCollisionObject> o)
	{
		m_bodyCache.push_back(o);
		m_bodyMap[o.get()] = static_cast<bitType>(eg);
	}
};

} // namespace collisionEvent
} // namespace akasha
#endif /* end of include guard */
