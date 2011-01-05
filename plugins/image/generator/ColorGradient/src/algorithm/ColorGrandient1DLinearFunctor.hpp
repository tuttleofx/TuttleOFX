#ifndef _TUTTLE_PLUGIN_COLORGRADIENT_1DLINEARFUNCTOR_HPP_
#define _TUTTLE_PLUGIN_COLORGRADIENT_1DLINEARFUNCTOR_HPP_

#include "../ColorGradientDefinitions.hpp"

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/image/gil/globals.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/numeric/pixel_numeric_operations.hpp>
#include <boost/math/special_functions/pow.hpp>

#include <cmath>
#include <vector>

namespace tuttle {
namespace plugin {
namespace colorGradient {
typedef boost::gil::point2<double> point_t;

class LineEquation
{
double _a, _b, _c;     ///< equation
point_t _AB;     ///< BC vector

public:
	LineEquation() {}
	LineEquation( const point_t& A, const point_t& B )
		: _AB( B - A )
	{
		// compute equation BC, ax + by + c = 0
		_a = B.y - A.y;
		_b = A.x - B.x;
		_c = ( A.y - B.y ) * A.x + ( B.x - A.x ) * A.y;
	}

	template <class Point>
	Point pointOrthogonalProjection( const Point& p ) const
	{
		// compute coordinates of M, the orthogonal projection of p on current line
		Point res;

		res.y = ( -( _a * p.x ) - ( _a * _AB.y * p.y / _AB.x ) - _c ) / ( _b - ( _a * _AB.y / _AB.x ) );
		res.x = ( -_c - _b * res.y ) / _a;
		return res;
	}

};

// Models a Unary Function
template <typename P>
// Models PixelValueConcept
class ColorGrandient1DLinearFunctor
{
public:
	typedef boost::gil::point2<double> point_t;

	typedef ColorGrandient1DLinearFunctor const_t;
	typedef P value_type;
	typedef value_type reference;
	typedef value_type const_reference;
	typedef point_t argument_type;
	typedef reference result_type;
	BOOST_STATIC_CONSTANT( bool, is_mutable = false );

	/*
	   struct ColoredPoint
	   {
	    point_t _position;
	    value_type _color;
	   };
	 */
	std::vector<point_t> _points;
	std::vector<value_type> _colors;
	std::vector<double> _distanceToB;
	LineEquation _droiteEquation;
	point_t _pB; ///< point B
	point_t _pC; ///< point C

	ColorGrandient1DLinearFunctor() {}

	ColorGrandient1DLinearFunctor( const point_t& size, const std::vector<point_t>& points, const std::vector<value_type>& colors )
		: _points( points )
		, _colors( colors )
	{
		assert( _points.size() == _colors.size() );
		assert( _points.size() >= 2 );

		_pB             = _points[0] * size; // to canonical coordinates
		_pC             = _points[1] * size;
		_droiteEquation = LineEquation( _pB, _pC );

		// fill _distanceToB
		_distanceToB.reserve( _points.size() );
		for( std::vector<point_t>::const_iterator it = _points.begin(), itEnd = _points.end();
		     it != itEnd;
		     ++it )
		{
			_distanceToB.push_back( distance( *it, _pB ) );
		}
		// sort _points, _colors, _distanceToB
	}

	double distance( const point_t& a, const point_t& b ) const
	{
		return std::sqrt( boost::math::pow<2>( b.x - a.x ) + boost::math::pow<2>( b.y - a.y ) );
	}

	result_type operator()( const point_t& pA ) const
	{
		using namespace boost::gil;
		point_t pM = _droiteEquation.pointOrthogonalProjection( pA );
		//		TUTTLE_COUT( "________________________________________" );
		//		TUTTLE_COUT_VAR( _pB );
		//		TUTTLE_COUT_VAR( _pC );
		//		TUTTLE_COUT_VAR( pA );
		//		TUTTLE_COUT_VAR( pM );
		double distB = distance( pM, _pB );
		double distC = distance( pM, _pC );
		double norm  = distB + distC;
		//		TUTTLE_COUT_VAR3( distB, distC, norm );
		distB /= norm;
		distC /= norm;
		//		TUTTLE_COUT_VAR2( _pB, _pC );
		//		TUTTLE_COUT_VAR2( distB, distC );
		// distB * _colors[0] + distC * _colors[1];
		return pixel_plus_t<result_type, result_type, result_type>() (
		           pixel_multiplies_scalar_t<result_type, double, result_type>() ( _colors[0], distB ),
		           pixel_multiplies_scalar_t<result_type, double, result_type>() ( _colors[1], distC )
		           );
	}

};

}
}
}

#endif
