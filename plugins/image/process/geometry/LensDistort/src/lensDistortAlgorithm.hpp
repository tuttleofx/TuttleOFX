#ifndef _LENSDISTORTALGORITHM_HPP_
#define _LENSDISTORTALGORITHM_HPP_

#include "lensDistortProcessParams.hpp"

#include <tuttle/plugin/numeric/rectOp.hpp>
#include <tuttle/plugin/IProgress.hpp>
#include <tuttle/plugin/exceptions.hpp>
#include <terry/globals.hpp>
#include <terry/point/operations.hpp>

#include <ofxsCore.h>

#include <boost/gil/image.hpp>
#include <boost/gil/utilities.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/assert.hpp>

#include <cmath>

namespace terry {

using namespace boost::gil;

template <typename F, typename F2>
inline point2<F> transform( const ::tuttle::plugin::lens::LensDistortBrown1<F>& algo, const point2<F2>& src )
{
	return algo.apply(src);
}

template <typename F, typename F2>
inline point2<F> transform( const ::tuttle::plugin::lens::LensUndistortBrown1<F>& algo, const point2<F2>& src )
{
	return algo.apply(src);
}

template <typename F, typename F2>
inline point2<F> transform( const ::tuttle::plugin::lens::LensDistortFisheye<F>& algo, const point2<F2>& src )
{
	return algo.apply(src);
}

template <typename F, typename F2>
inline point2<F> transform( const ::tuttle::plugin::lens::LensUndistortFisheye<F>& algo, const point2<F2>& src )
{
	return algo.apply(src);
}

template <typename F, typename F2>
inline point2<F> transform( const ::tuttle::plugin::lens::LensDistortAdvanced<F>& algo, const point2<F2>& src )
{
	return algo.apply(src);
}

}

namespace tuttle {
namespace plugin {
namespace lens {

template<class DistortFunc>
typename DistortFunc::Point2 transformValues( const DistortFunc& algo, const typename DistortFunc::Point2& p )
{
	return algo.apply( p );
}

/**
 * @brief get bounding box after the transformation of rec
 */
template<class DistortFunc>
OfxRectD transformValues( const DistortFunc& algo, const OfxRectD& rec )
{
	typedef typename DistortFunc::Point2 Point2;
	std::vector<Point2> points;

	// center in rec ?
	if( algo._params.lensCenterDst.x > rec.x1 && algo._params.lensCenterDst.x < rec.x2 )
	{
		points.push_back( algo.apply( Point2( algo._params.lensCenterDst.x, rec.y1 ) ) );
		points.push_back( algo.apply( Point2( algo._params.lensCenterDst.x, rec.y2 ) ) );
	}
	if( algo._params.lensCenterDst.y > rec.y1 && algo._params.lensCenterDst.y < rec.y2 )
	{
		points.push_back( algo.apply( Point2( rec.x1, algo._params.lensCenterDst.y ) ) );
		points.push_back( algo.apply( Point2( rec.x2, algo._params.lensCenterDst.y ) ) );
	}

	// A B
	// C D
	Point2 outA( rec.x1, rec.y1 );
	Point2 outB( rec.x2, rec.y1 );
	Point2 outC( rec.x1, rec.y2 );
	Point2 outD( rec.x2, rec.y2 );
	points.push_back( algo.apply( outA ) );
	points.push_back( algo.apply( outB ) );
	points.push_back( algo.apply( outC ) );
	points.push_back( algo.apply( outD ) );

	return pointsBoundingBox( points );
}

/**
 * @param lensType
 * @param params
 * @param obj: objet to transform (can be OfxRectD, boost::gil::point2<>)
 */
template<class Obj>
inline Obj transformValues( const EParamLensType lensType, const LensDistortProcessParams<double>& params, const Obj& obj )
{
	switch( lensType )
	{
		case eParamLensTypeStandard:
		{
			if( params.distort )
			{
				return transformValues( LensDistortBrown1<double>(params), obj );
			}
			else
			{
				return transformValues( LensUndistortBrown1<double>(params), obj );
			}
		}
		case eParamLensTypeFisheye:
		{
			if( params.distort )
				return transformValues( LensDistortFisheye<double>(params), obj );
			else
				return transformValues( LensUndistortFisheye<double>(params), obj );
		}
		case eParamLensTypeAdvanced:
		{
			if( params.distort )
				return transformValues( LensDistortAdvanced<double>(params), obj );
			else
				return transformValues( LensDistortAdvanced<double>(params), obj );
		}
	}
	BOOST_THROW_EXCEPTION( exception::Unsupported()
	    << exception::user( "Unknown lens type." ) );
	return obj;
}

/**
 * @brief Apply the transformation on all points inside the vector
 */
template<class DistortFunc, typename F2>
inline void transformValuesApply( const DistortFunc& params, std::vector<boost::gil::point2<F2> >& vec )
{
	for( typename std::vector<boost::gil::point2<F2> >::iterator it = vec.begin(), itEnd = vec.end();
	     it != itEnd;
	     ++it )
	{
		*it = params.apply( *it );
	}
}

template<class DistortFunc, typename F2>
inline void transformValuesApply( const DistortFunc& params, std::vector<std::vector<boost::gil::point2<F2> > >& vec )
{
	for( typename std::vector<std::vector<boost::gil::point2<F2> > >::iterator it = vec.begin(), itEnd = vec.end();
	     it != itEnd;
	     ++it )
	{
		transformValuesApply( params, *it );
	}
}

template<class Obj>
inline void transformValuesApply( const EParamLensType lensType, const LensDistortProcessParams<double>& params, Obj& obj )
{
	switch( lensType )
	{
		case eParamLensTypeStandard:
		{
			if( params.distort )
			{
				transformValuesApply( LensDistortBrown1<double>(params), obj );
			}
			else
			{
				transformValuesApply( LensUndistortBrown1<double>(params), obj );
			}
			break;
		}
		case eParamLensTypeFisheye:
		{
			if( params.distort )
				transformValuesApply( LensDistortFisheye<double>(params), obj );
			else
				transformValuesApply( LensUndistortFisheye<double>(params), obj );
			break;
		}
		case eParamLensTypeAdvanced:
		{
			//if( params.distort )
			transformValuesApply( LensDistortAdvanced<double>(params), obj );
			//else
			//	transformValuesApply( LensUndistortAdvanced<double>(params), obj );
			break;
		}
	}
}


}
}
}

#endif
