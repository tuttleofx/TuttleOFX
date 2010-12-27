#ifndef _TUTTLE_PLUGIN_COLORGRADIENT_2DLINEARFUNCTOR_HPP_
#define _TUTTLE_PLUGIN_COLORGRADIENT_2DLINEARFUNCTOR_HPP_

#include "../ColorGradientDefinitions.hpp"

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/image/gil/globals.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/numeric/pixel_numeric_operations.hpp>
#include <boost/gil/extension/numeric/pixel_numeric_operations_assign.hpp>
#include <boost/math/special_functions/pow.hpp>

#include <cmath>
#include <vector>

namespace tuttle {
namespace plugin {
namespace colorGradient {

// Models a Unary Function
template <typename P>
// Models PixelValueConcept
class ColorGrandient2DLinearFunctor
{
public:
	typedef double Scalar;
	typedef boost::gil::point2<Scalar> point_t;

	typedef ColorGrandient2DLinearFunctor const_t;
	typedef P value_type;
	typedef value_type reference;
	typedef value_type const_reference;
	typedef point_t argument_type;
	typedef reference result_type;
	BOOST_STATIC_CONSTANT( bool, is_mutable = false );

	typedef std::vector<point_t> PointsVector;
	typedef std::vector<value_type> ColorsVector;
	typedef std::vector<Scalar> DistancesVector;
	PointsVector _points;
	ColorsVector _colors;

	mutable DistancesVector _distances; // temporary values

	ColorGrandient2DLinearFunctor() {}

	ColorGrandient2DLinearFunctor( const point_t& size, const std::vector<point_t>& points, const std::vector<value_type>& colors )
		: _points( points )
		, _colors( colors )
	{
		assert( _points.size() == _colors.size() );
		assert( _points.size() >= 2 );

		_distances.resize( _points.size() );

		for( PointsVector::iterator it = _points.begin(), itEnd = _points.end();
		     it != itEnd;
		     ++it )
		{
			( *it ) *= size; // to canonical coordinates
		}
	}

private:
	Scalar distance( const point_t& a, const point_t& b ) const
	{
		//		return std::sqrt( boost::math::pow<2>(b.x - a.x) + boost::math::pow<2>(b.y - a.y) );
		return boost::math::pow<2>( b.x - a.x ) + boost::math::pow<2>( b.y - a.y );
	}

	void computeDistances( const point_t& p ) const
	{
		double sum                            = 0;
		DistancesVector::iterator it_dist     = _distances.begin(), itEnd_dist = _distances.end();
		PointsVector::const_iterator it_point = _points.begin();

		for( ;
		     it_dist != itEnd_dist;
		     ++it_dist, ++it_point )
		{
			*it_dist = 1.0 / distance( p, *it_point );
			sum     += *it_dist;
		}
		for( it_dist = _distances.begin();
		     it_dist != itEnd_dist;
		     ++it_dist )
		{
			*it_dist /= sum; // normalize values
		}
	}

public:
	result_type operator()( const point_t& p ) const
	{
		using namespace boost::gil;
		computeDistances( p );
		result_type outputColor;
		pixel_zeros_t<result_type>() ( outputColor );

		typename ColorsVector::const_iterator it_color = _colors.begin(), itEnd_color = _colors.end();
		DistancesVector::const_iterator it_dist = _distances.begin();

		for( ;
		     it_color != itEnd_color;
		     ++it_color, ++it_dist )
		{
			// outputColor += color * dist
			pixel_plus_assign_t<result_type, result_type>() (
			    pixel_multiplies_scalar_t<result_type, Scalar, result_type>() ( *it_color, *it_dist ),
			    outputColor
			    );
		}
		return outputColor;
	}

};

}
}
}

#endif
