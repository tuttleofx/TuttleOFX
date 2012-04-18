#ifndef _TERRY_GENERATOR_CONSTANT_HPP_
#define _TERRY_GENERATOR_CONSTANT_HPP_

#include <boost/gil/utilities.hpp>

#include <cmath>

namespace terry {
namespace generator {

// Models a Unary Function
template <typename P>
// Models PixelValueConcept
struct ConstantFunctor
{
	typedef boost::gil::point2<double>    point_t;
	typedef ConstantFunctor const_t;
	typedef P value_type;
	typedef value_type reference;
	typedef value_type const_reference;
	typedef point_t argument_type;
	typedef reference result_type;
	BOOST_STATIC_CONSTANT( bool, is_mutable = false );

	value_type _color;

	ConstantFunctor() {}
	ConstantFunctor( const value_type& color )
		: _color( color )
	{
	}

	result_type operator()( const point_t& ) const
	{
		return _color;
	}

};

}
}

#endif
