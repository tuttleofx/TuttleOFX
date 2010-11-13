#ifndef _LENSDISTORTALGORITHM_HPP_
#define _LENSDISTORTALGORITHM_HPP_

#include "lensDistortProcessParams.hpp"

#include <tuttle/common/math/rectOp.hpp>
#include <tuttle/plugin/IProgress.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <ofxCore.h>

#include <boost/gil/image.hpp>
#include <boost/gil/utilities.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/gil/extension/numeric/sampler.hpp>
#include <boost/gil/extension/numeric/resample.hpp>

#include <cmath>

namespace tuttle {
namespace plugin {
namespace lens {

template <typename F, typename F2>
inline boost::gil::point2<F> transform( const NormalLensDistortParams<F>& params, const boost::gil::point2<F2>& src )
{
	assert( params._distort );
	assert( params._coef1 >= 0 );
	boost::gil::point2<F> pc( params.pixelToLensCenterNormalized( src ) ); // centered normalized space
	pc *= params._postScale;

	const F r2   = pc.x * pc.x + pc.y * pc.y; // distance to center squared
	const F coef = 1.0 + r2 * params._coef1; // distortion coef for current pixel
	pc.x *= coef;
	pc.y *= coef;

	pc *= params._preScale;
	return params.lensCenterNormalizedToPixel( pc ); // original space
}

template <typename F, typename F2>
inline boost::gil::point2<F> transform( const NormalLensUndistortParams<F>& params, const boost::gil::point2<F2>& src )
{
	assert( !params._distort );
	assert( params._coef1 >= 0 );
	boost::gil::point2<F> pc( params.pixelToLensCenterNormalized( src ) );
	pc *= params._postScale;

	F r = std::sqrt( pc.x * pc.x + pc.y * pc.y ); // distance to center
	// necessary values to calculate
	if( r == 0 || params._coef1 == 0 )
	{
		boost::gil::point2<F> tmp( src.x, src.y );
		return tmp;
	}

	// calculate the determinant delta = Q^3 + R^2
	F cQ    = -1.0 / ( 3.0 * params._coef1 );
	F cR    = -r / ( 2.f * params._coef1 );
	F delta = -cQ * cQ * cQ + cR * cR;
	F coef  = 0.0;
	F t;

	// negative or positive determinant
	if( delta > 0 )
	{
		coef = std::abs( cR ) + std::sqrt( delta );
		coef = -std::pow( coef, 1.0 / 3.0 );
		assert( coef != 0 );
		coef += cQ / coef;
	}
	else if( delta < 0 )
	{
		assert( cQ >= 0 );
		assert( cR / ( sqrt( cQ * cQ * cQ ) ) <= 1 && cR / ( sqrt( cQ * cQ * cQ ) ) >= -1 );
		t    = std::acos( cR / ( sqrt( cQ * cQ * cQ ) ) );
		coef = -2 * std::sqrt( cQ ) * std::cos( ( t - M_PI_2 ) / 3.0 );
	}
	else
	{
		assert( 0 ); // Untreated case..
	}

	// get coordinates into distorded image from distorded center
	coef /= r;
	coef  = std::abs( coef );

	pc *= coef;
	pc *= params._preScale;
	return params.lensCenterNormalizedToPixel( pc ); // to the original space
}

/**
 * @todo support for fisheye...
 */
template <typename F, typename F2>
inline boost::gil::point2<F> transform( const FisheyeLensDistortParams<F>& params, const boost::gil::point2<F2>& src )
{
	boost::gil::point2<F> pc( params.pixelToLensCenterNormalized( src ) );
	pc *= params._postScale;

	// F r2 = pc.x * pc.x + pc.y * pc.y; // distance to center
	// F coef = params._postScale * ( 1.0 + r2 * params._coef1 + ( r2 * r2 ) * params._coef2 ); // distortion coef for current pixel
	// pc.x *= coef;
	// pc.y *= coef;
	F r = std::sqrt( pc.x * pc.x + pc.y * pc.y ); // distance to center
	if( r == 0 )
	{
		boost::gil::point2<F> tmp( src.x, src.y );
		return tmp;
	}
	F coef = 0.5 * std::tan( r * params._coef1 ) / ( std::tan( 0.5 * params._coef1 ) * r );

	pc *= coef;
	pc *= params._preScale;
	return params.lensCenterNormalizedToPixel( pc ); // to the original space
}

/**
 * @todo support for fisheye...
 */
template <typename F, typename F2>
inline boost::gil::point2<F> transform( const FisheyeLensUndistortParams<F>& params, const boost::gil::point2<F2>& src )
{
	boost::gil::point2<F> pc( params.pixelToLensCenterNormalized( src ) );
	pc *= params._postScale;

	F r = std::sqrt( pc.x * pc.x + pc.y * pc.y ); // distance to center
	if( r == 0 || params._coef1 == 0 )
	{
		boost::gil::point2<F> tmp( src.x, src.y );
		return tmp;
	}
	F coef = std::atan( 2.0 * r * std::tan( 0.5 * params._coef1 ) ) / params._coef1;
	//if(rd<0) rd = -rd;
	coef /= r;

	pc *= coef;
	pc *= params._preScale;
	return params.lensCenterNormalizedToPixel( pc ); // to the original space
}

/**
 * @todo support for advanced lens...
 */
template <typename F, typename F2>
inline boost::gil::point2<F> transform( const AdvancedLensDistortParams<F>& params, const boost::gil::point2<F2>& src )
{
	boost::gil::point2<F> pc( params.pixelToLensCenterNormalized( src ) );
	pc *= params._postScale;

	F r2 = pc.x * pc.x + pc.y * pc.y; // distance to center
	F r4 = r2 * r2;
	pc.x *= ( 1.0 + r2 * params._coef1 + r4 * params._coef2 );
	pc.y *= ( 1.0 + ( r2 * params._coef1 + r4 * params._coef2 ) / params._squeeze );

	pc *= params._preScale;
	return params.lensCenterNormalizedToPixel( pc ); // to the original space
}

template<class Params>
typename Params::Point2 transformValues( const Params& params, const typename Params::Point2& p )
{
	return transform( params, p );
}

/**
 * @brief get bounding box after the transformation of rec
 */
template<class Params>
OfxRectD transformValues( const Params& params, const OfxRectD& rec )
{
	typedef typename Params::Point2 Point2;
	std::vector<Point2> points;

	// center in rec ?
	if( params._lensCenterDst.x > rec.x1 && params._lensCenterDst.x < rec.x2 )
	{
		points.push_back( transform( params, Point2( params._lensCenterDst.x, rec.y1 ) ) );
		points.push_back( transform( params, Point2( params._lensCenterDst.x, rec.y2 ) ) );
	}
	if( params._lensCenterDst.y > rec.y1 && params._lensCenterDst.y < rec.y2 )
	{
		points.push_back( transform( params, Point2( rec.x1, params._lensCenterDst.y ) ) );
		points.push_back( transform( params, Point2( rec.x2, params._lensCenterDst.y ) ) );
	}

	// A B
	// C D
	Point2 outA( rec.x1, rec.y1 );
	Point2 outB( rec.x2, rec.y1 );
	Point2 outC( rec.x1, rec.y2 );
	Point2 outD( rec.x2, rec.y2 );
	points.push_back( transform( params, outA ) );
	points.push_back( transform( params, outB ) );
	points.push_back( transform( params, outC ) );
	points.push_back( transform( params, outD ) );

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
			if( params._distort )
			{
				return transformValues<NormalLensDistortParams<double> >( params, obj );
			}
			else
			{
				return transformValues<NormalLensUndistortParams<double> >( params, obj );
			}
		}
		case eParamLensTypeFisheye:
		{
			if( params._distort )
				return transformValues<FisheyeLensDistortParams<double> >( params, obj );
			else
				return transformValues<FisheyeLensUndistortParams<double> >( params, obj );
		}
		case eParamLensTypeAdvanced:
		{
			if( params._distort )
				return transformValues<AdvancedLensDistortParams<double> >( params, obj );
			else
				return transformValues<AdvancedLensUndistortParams<double> >( params, obj );
		}
	}
	BOOST_THROW_EXCEPTION( exception::Unsupported()
	    << exception::user( "Outside of the plugin fonctionnalities." ) );
	return obj;
}

/**
 * @brief Apply the transformation on all points inside the vector
 */
template<class Params, typename F2>
inline void transformValuesApply( const Params& params, std::vector<boost::gil::point2<F2> >& vec )
{
	for( typename std::vector<boost::gil::point2<F2> >::iterator it = vec.begin(), itEnd = vec.end();
	     it != itEnd;
	     ++it )
	{
		*it = transform( params, *it );
	}
}

template<class Params, typename F2>
inline void transformValuesApply( const Params& params, std::vector<std::vector<boost::gil::point2<F2> > >& vec )
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
			if( params._distort )
			{
				return transformValuesApply<NormalLensDistortParams<double> >( params, obj );
			}
			else
			{
				return transformValuesApply<NormalLensUndistortParams<double> >( params, obj );
			}
		}
		case eParamLensTypeFisheye:
		{
			if( params._distort )
				return transformValuesApply<FisheyeLensDistortParams<double> >( params, obj );
			else
				return transformValuesApply<FisheyeLensUndistortParams<double> >( params, obj );
		}
		case eParamLensTypeAdvanced:
		{
			if( params._distort )
				return transformValuesApply<AdvancedLensDistortParams<double> >( params, obj );
			else
				return transformValuesApply<AdvancedLensUndistortParams<double> >( params, obj );
		}
	}
}

/**
 * @brief Set each pixel in the destination view as the result of a sampling function over the transformed coordinates of the source view
 * @ingroup ImageAlgorithms
 *
 * The provided implementation works for 2D image views only
 */
template <typename Sampler, // Models SamplerConcept
          typename SrcView, // Models RandomAccess2DImageViewConcept
          typename DstView, // Models MutableRandomAccess2DImageViewConcept
          typename MapFn>
// Models MappingFunctionConcept
void resample_pixels( const SrcView& src_view, const DstView& dst_view, const MapFn& dst_to_src, const OfxRectI& procWindow, tuttle::plugin::IProgress* p, Sampler sampler = Sampler() )
{
	typename DstView::point_t dst_p;
	typename DstView::value_type black;
	color_convert( boost::gil::rgba32f_pixel_t( 0.0, 0.0, 0.0, 0.0 ), black );
	for( dst_p.y = procWindow.y1; dst_p.y < procWindow.y2; ++dst_p.y )
	{
		typename DstView::x_iterator xit = dst_view.row_begin( dst_p.y );
		for( dst_p.x = procWindow.x1; dst_p.x < procWindow.x2; ++dst_p.x )
		{
			if( !boost::gil::sample( sampler, src_view, transform( dst_to_src, dst_p ), xit[dst_p.x] ) )
			{
				xit[dst_p.x] = black; // if it is outside of the source image
			}
		}
		if( p->progressForward() )
			return;
	}
}

}
}
}

#endif
