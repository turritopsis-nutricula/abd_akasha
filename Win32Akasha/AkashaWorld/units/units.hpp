#ifndef AKASHA_WORLD_UNITS_UNITS_HPP_
#define AKASHA_WORLD_UNITS_UNITS_HPP_

#include <boost/units/systems/si.hpp>
#include <boost/units/systems/si/prefixes.hpp>
#include <boost/units/pow.hpp>
#include <boost/units/io.hpp>

// extension boost::units
namespace boost
{
namespace units
{
// L^3 T^-1
typedef derived_dimension<
	length_base_dimension, 3,
	time_base_dimension,  -1
		>::type flow_velocity_dimension;

namespace si
{
// flow_velocity
typedef unit< flow_velocity_dimension, si::system > flow_velocity;
static const flow_velocity cubic_meter_per_second;


} // namespace si
} // namespace units
} // namespace boost

namespace akasha
{
namespace units
{

// reql number type
using real = float;

// boost::units::expand
using namespace boost::units;


using length_type      = quantity< si::length,      real>;
using mass_type        = quantity< si::mass,        real>;
using time_type        = quantity< si::time,        real>;
using temperature_type = quantity< si::temperature, real>;
using velocity_type    = quantity< si::velocity,    real>;

using force_type            = quantity< si::force,             real>;
using torque_type           = quantity< si::torque,            real>;
using angular_velocity_type = quantity< si::angular_velocity,  real>;
using angular_momentum_type = quantity< si::angular_momentum,  real>;
using moment_inertia_type   = quantity< si::moment_of_inertia, real>;

using volume_type      = quantity< si::volume,      real>;
using pressure_type    = quantity< si::pressure,    real>;

using flow_velocity_type = quantity< si::flow_velocity, real>;

} // namespace units
} // namespace akasha
#endif /* end of include guard */
