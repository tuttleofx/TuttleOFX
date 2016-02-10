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
#include <boost/foreach.hpp>

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
inline point2<F> transform( const ::tuttle::plugin::lens::LensDistortBrown3<F>& algo, const point2<F2>& src )
{
	return algo.apply(src);
}

template <typename F, typename F2>
inline point2<F> transform( const ::tuttle::plugin::lens::LensUndistortBrown3<F>& algo, const point2<F2>& src )
{
	return algo.apply(src);
}

template <typename F, typename F2>
inline point2<F> transform( const ::tuttle::plugin::lens::LensDistortPTLens<F>& algo, const point2<F2>& src )
{
	return algo.apply(src);
}

template <typename F, typename F2>
inline point2<F> transform( const ::tuttle::plugin::lens::LensUndistortPTLens<F>& algo, const point2<F2>& src )
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

	// If center is in rec
	if( algo._params.lensCenterDst.x > rec.x1 && algo._params.lensCenterDst.x < rec.x2 )
	{
		points.push_back( Point2( algo._params.lensCenterDst.x, rec.y1 ) );
		points.push_back( Point2( algo._params.lensCenterDst.x, rec.y2 ) );
	}
	if( algo._params.lensCenterDst.y > rec.y1 && algo._params.lensCenterDst.y < rec.y2 )
	{
		points.push_back( Point2( rec.x1, algo._params.lensCenterDst.y ) );
		points.push_back( Point2( rec.x2, algo._params.lensCenterDst.y ) );
	}

	const double xOffset = (rec.x2-rec.x1)/16.0;
	for( double x = rec.x1; x < rec.x2 + xOffset; x += xOffset )
	{
		points.push_back( Point2( x, rec.y1 ) );
		points.push_back( Point2( x, rec.y2 ) );
	}
	const double yOffset = (rec.y2-rec.y1)/16.0;
	for( double y = rec.y1; y < rec.y2 + yOffset; y += yOffset )
	{
		points.push_back( Point2( rec.x1, y ) );
		points.push_back( Point2( rec.x2, y ) );
	}
	BOOST_FOREACH( Point2& p, points )
	{
		p = algo.apply(p);
	}
	
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
		case eParamLensTypeBrown1:
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
		case eParamLensTypeBrown3:
		{
			if( params.distort )
			{
				return transformValues( LensDistortBrown3<double>(params), obj );
			}
			else
			{
				return transformValues( LensUndistortBrown3<double>(params), obj );
			}
		}
		case eParamLensTypePTLens:
		{
			if( params.distort )
				return transformValues( LensDistortPTLens<double>(params), obj );
			else
				return transformValues( LensUndistortPTLens<double>(params), obj );
		}
		case eParamLensTypeFisheye:
		{
			if( params.distort )
				return transformValues( LensDistortFisheye<double>(params), obj );
			else
				return transformValues( LensUndistortFisheye<double>(params), obj );
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
		case eParamLensTypeBrown1:
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
		case eParamLensTypeBrown3:
		{
			if( params.distort )
			{
				transformValuesApply( LensDistortBrown3<double>(params), obj );
			}
			else
			{
				transformValuesApply( LensUndistortBrown3<double>(params), obj );
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
	}
}


}
}
}

#endif
