#include "MergePlugin.hpp"
#include "MergeDefinitions.hpp"
#include <terry/merge/ViewsMerging.hpp>

#include <tuttle/plugin/numeric/rectOp.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <terry/numeric/operations.hpp>
#include <terry/numeric/init.hpp>

#include <boost/gil/extension/color/hsl.hpp>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace merge {

template<class View, class Functor>
MergeProcess<View, Functor>::MergeProcess( MergePlugin& instance )
	: ImageGilProcessor<View>( instance, eImageOrientationIndependant )
	, _plugin( instance )
{}

template<class View, class Functor>
void MergeProcess<View, Functor>::setup( const OFX::RenderArguments& args )
{
	ImageGilProcessor<View>::setup( args );

	// sources view
	// clip A
	_srcA.reset( _plugin._clipSrcA->fetchImage( args.time ) );
	if( !_srcA.get() )
		BOOST_THROW_EXCEPTION( exception::ImageNotReady() );
	if( _srcA->getRowDistanceBytes() == 0 )
		BOOST_THROW_EXCEPTION( exception::WrongRowBytes() );
	
	if( OFX::getImageEffectHostDescription()->hostName == "uk.co.thefoundry.nuke" )
	{
		// bug in nuke, getRegionOfDefinition() on OFX::Image returns bounds
		_srcPixelRodA   = _plugin._clipSrcA->getPixelRod( args.time, args.renderScale );
	}
	else
	{
		_srcPixelRodA = _srcA->getRegionOfDefinition();
	}
	this->_srcViewA = this->getView( _srcA.get(), _srcPixelRodA );

	// clip B
	_srcB.reset( _plugin._clipSrcB->fetchImage( args.time ) );
	if( !_srcB.get() )
		BOOST_THROW_EXCEPTION( exception::ImageNotReady() );
	if( _srcB->getRowDistanceBytes() == 0 )
		BOOST_THROW_EXCEPTION( exception::WrongRowBytes() );
	
	if( OFX::getImageEffectHostDescription()->hostName == "uk.co.thefoundry.nuke" )
	{
		// bug in nuke, getRegionOfDefinition() on OFX::Image returns bounds
		_srcPixelRodB   = _plugin._clipSrcB->getPixelRod( args.time, args.renderScale );
	}
	else
	{
		_srcPixelRodB = _srcB->getRegionOfDefinition();
	}
	this->_srcViewB = this->getView( _srcB.get(), _srcPixelRodB );

	// Make sure bit depths are the same
	if( _srcA->getPixelDepth() != this->_dst->getPixelDepth() ||
	    _srcB->getPixelDepth() != this->_dst->getPixelDepth() ||
	    _srcA->getPixelComponents() != this->_dst->getPixelComponents() ||
	    _srcB->getPixelComponents() != this->_dst->getPixelComponents() )
	{
		BOOST_THROW_EXCEPTION( exception::BitDepthMismatch() );
	}

	_params = _plugin.getProcessParams( args.renderScale );
}

template <typename View, typename Value> GIL_FORCEINLINE 
void fill_pixels(const View& dstView, const Value& val, const OfxRectI& region )
{
	View dst = subimage_view( dstView,
			region.x1, region.y1,
			region.x2-region.x1, region.y2-region.y1 );
	
	fill_pixels( dst, val );
}


template <typename View> GIL_FORCEINLINE
void copy_pixels( const View& src, const OfxRectI& srcRegion, const View& dst, const OfxRectI& dstRegion )
{
	const OfxPointI regionSize = {
		srcRegion.x2 - srcRegion.x1,
		srcRegion.y2 - srcRegion.y1
	};
	BOOST_ASSERT( regionSize.x == ( dstRegion.x2 - dstRegion.x1 ) );
	BOOST_ASSERT( regionSize.y == ( dstRegion.y2 - dstRegion.y1 ) );
	
	View srcCrop = subimage_view( src,
			srcRegion.x1, srcRegion.y1,
			regionSize.x, regionSize.y );
	View dstCrop = subimage_view( dst,
			dstRegion.x1, dstRegion.y1,
			regionSize.x, regionSize.y );

	copy_pixels( srcCrop, dstCrop );
	
}

template<class View>
void fillAroundIntersection(
		const View& viewA, const OfxRectI& srcAPixelRod,
		const View& viewB, const OfxRectI& srcBPixelRod,
		const View& dstView, const OfxRectI& dstPixelRod,
		const OfxRectI& procWindowRoW
	)
{
	using namespace terry;
	using namespace terry::numeric;
	using namespace boost::gil;
	/// @todo tuttle:
	///  * fill only the good regions
	///  * add color choice?
	
	////////////////////////////////////////////////////
	// Schema of different cases:
	// x: fill with a color
	//
	////////////////////////////////////////////////////
	// _____________
	// |            |xxxxxxxx
	// |  copy A    |xxxxxxxx
	// |      ______|_______
	// |     |      |       |
	// |     |Merge |       |
	// |_____|______|       |
	// xxxxxx|    copy B    |
	// xxxxxx|______________|
	// 
	////////////////////////////////////////////////////
	//       _______
	// xxxxxx|      |xxxxxxxx
	// xxxxxx|      |xxxxxxxx
	// ______|______|_______
	// |     |      |       |
	// |     |Merge |       |
	// |_____|______|_______|
	// xxxxxx|      |xxxxxxxx
	// xxxxxx|______|xxxxxxxx
	//
	////////////////////////////////////////////////////
	// _____________
	// |            |xxxxxxxx
	// |  copy A    |xxxxxxxx
	// |____________|xxxxxxxx
	// xxxxxxxxxxxxxxxxxxxxxx
	// xxxxxx________________
	// xxxxxx|    copy B    |
	// xxxxxx|______________|
	//
	////////////////////////////////////////////////////
	// _____________________
	// |                    |
	// |_ _ _ ______ _ _ _ _|
	// |     |      |       |
	// |     |Merge |       |
	// |_ _ _|______|_ _ _ _|
	// |____________________|
	// 
	////////////////////////////////////////////////////
	
	if( ! rectangleContainsAnother( srcAPixelRod, srcBPixelRod ) )
	{
		// fill color
		typedef typename View::value_type Pixel;
		Pixel pixelZero; pixel_zeros_t<Pixel>()( pixelZero );
		const OfxRectI procWindowOutput = translateRegion( procWindowRoW, dstPixelRod );
		
		fill_pixels( dstView, pixelZero, procWindowOutput );
	}
	{
		// fill with A
		const OfxRectI intersectRegionA = rectanglesIntersection( procWindowRoW, srcAPixelRod );
		const OfxRectI procWindowSrcA = translateRegion( intersectRegionA, srcAPixelRod );
		const OfxRectI procWindowOutputA = translateRegion( intersectRegionA, dstPixelRod );
		//TUTTLE_LOG_VAR( TUTTLE_INFO, intersectRegionA );
		//TUTTLE_LOG_VAR( TUTTLE_INFO, procWindowSrcA );
		//TUTTLE_LOG_VAR( TUTTLE_INFO, procWindowOutputA );
		
		/// @todo tuttle: fill only the good regions
		copy_pixels( viewA, procWindowSrcA, dstView, procWindowOutputA );
	}
	{
		// fill with B
		const OfxRectI intersectRegionB = rectanglesIntersection( procWindowRoW, srcBPixelRod );
		const OfxRectI procWindowSrcB = translateRegion( intersectRegionB, srcBPixelRod );
		const OfxRectI procWindowOutputB = translateRegion( intersectRegionB, dstPixelRod );
		//TUTTLE_LOG_VAR( TUTTLE_INFO, intersectRegionB );
		//TUTTLE_LOG_VAR( TUTTLE_INFO, procWindowSrcB );
		//TUTTLE_LOG_VAR( TUTTLE_INFO, procWindowOutputB );
		
		/// @todo tuttle: fill only the good regions
		copy_pixels( viewB, procWindowSrcB, dstView, procWindowOutputB );
	}
}

template<class View>
void fillAroundIntersection(
		const View& viewA, const OfxRectI& srcAPixelRod,
		const OfxRectI& srcBPixelRod,
		const View& dstView, const OfxRectI& dstPixelRod,
		const OfxRectI& procWindowRoW
	)
{
	// fill with A
	const OfxRectI procWindowSrc = translateRegion( procWindowRoW, srcAPixelRod );
	const OfxRectI procWindowOutput = translateRegion( procWindowRoW, dstPixelRod );

	/// @todo tuttle: fill only the good regions
	copy_pixels( viewA, procWindowSrc, dstView, procWindowOutput );
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template<class View, class Functor>
void MergeProcess<View, Functor>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace terry;
	using namespace terry::numeric;

	const OfxRectI srcRodA = translateRegion( _srcPixelRodA, _params._offsetA );
	const OfxRectI srcRodB = translateRegion( _srcPixelRodB, _params._offsetB );
	
	const OfxRectI intersect = rectanglesIntersection( srcRodA, srcRodB );
	const OfxRectI procIntersect = rectanglesIntersection( procWindowRoW, intersect );
	const OfxPointI procIntersectSize = {
		procIntersect.x2 - procIntersect.x1,
		procIntersect.y2 - procIntersect.y1
	};

	/// @todo tuttle: fill only the good regions
	switch( _params._rod )
	{
		case eParamRodIntersect:
		{
			// nothing to do in this case
			// this is the standard case
			// we merge the intersection of the 2 images
			break;
		}
		case eParamRodUnion:
		{
			fillAroundIntersection(
					this->_srcViewA, srcRodA,
					this->_srcViewB, srcRodB,
					this->_dstView, this->_dstPixelRod,
					procWindowRoW
				);
			break;
		}
		case eParamRodA:
		{
			fillAroundIntersection(
					this->_srcViewA, srcRodA,
					srcRodB,
					this->_dstView, this->_dstPixelRod,
					procWindowRoW
				);
			break;
		}
		case eParamRodB:
		{
			fillAroundIntersection(
					this->_srcViewB, srcRodB,
					srcRodA,
					this->_dstView, this->_dstPixelRod,
					procWindowRoW
				);
			break;
		}
	}

	View srcViewA_inter = subimage_view(	this->_srcViewA,
						procIntersect.x1 - srcRodA.x1,
						procIntersect.y1 - srcRodA.y1,
						procIntersectSize.x,
						procIntersectSize.y );
	View srcViewB_inter = subimage_view(	this->_srcViewB,
						procIntersect.x1 - srcRodB.x1,
						procIntersect.y1 - srcRodB.y1,
						procIntersectSize.x,
						procIntersectSize.y );

	View dstView_inter = subimage_view(	this->_dstView,
						procIntersect.x1 - this->_dstPixelRod.x1,
						procIntersect.y1 - this->_dstPixelRod.y1,
						procIntersectSize.x,
						procIntersectSize.y );

	merge_views( srcViewA_inter, srcViewB_inter, dstView_inter, Functor() );
}

}
}
}
