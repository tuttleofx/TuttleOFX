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
	if( _srcA->getRowBytes() == 0 )
		BOOST_THROW_EXCEPTION( exception::WrongRowBytes() );
	this->_srcViewA = this->getView( _srcA.get(), _plugin._clipSrcA->getPixelRod( args.time, args.renderScale ) );
	//	_srcPixelRodA = _srcA->getRegionOfDefinition(); // bug in nuke, returns bounds
	_srcPixelRodA = _plugin._clipSrcA->getPixelRod( args.time, args.renderScale );

	// clip B
	_srcB.reset( _plugin._clipSrcB->fetchImage( args.time ) );
	if( !_srcB.get() )
		BOOST_THROW_EXCEPTION( exception::ImageNotReady() );
	if( _srcB->getRowBytes() == 0 )
		BOOST_THROW_EXCEPTION( exception::WrongRowBytes() );
	this->_srcViewB = this->getView( _srcB.get(), _plugin._clipSrcB->getPixelRod( args.time, args.renderScale ) );
	//	_srcPixelRodB = _srcB->getRegionOfDefinition(); // bug in nuke, returns bounds
	_srcPixelRodB = _plugin._clipSrcB->getPixelRod( args.time, args.renderScale );

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
	const OfxPointI procWindowSize = {
		procWindowRoW.x2 - procWindowRoW.x1,
		procWindowRoW.y2 - procWindowRoW.y1
	};
	/// @todo tuttle:
	///  * fill only the good regions
	///  * add color choice?
	{
		// fill color
		typedef typename View::value_type Pixel;
		Pixel pixelZero; pixel_zeros_t<Pixel>()( pixelZero );
		const OfxRectI procWindowOutput = translateRegion( procWindowRoW, dstPixelRod );
		//TUTTLE_TCOUT_VAR( procWindowOutput );
		View dst = subimage_view( dstView,
				procWindowOutput.x1, procWindowOutput.y1,
				procWindowSize.x, procWindowSize.y );
		
		fill_pixels( dst, pixelZero );
	}
	{
		// fill with A
		const OfxRectI intersectRegionA = rectanglesIntersection( procWindowRoW, srcAPixelRod );
		const OfxPointI intersectRegionASize = {
			intersectRegionA.x2 - intersectRegionA.x1,
			intersectRegionA.y2 - intersectRegionA.y1
		};
		const OfxRectI procWindowSrcA = translateRegion( intersectRegionA, srcAPixelRod );
		const OfxRectI procWindowOutputA = translateRegion( intersectRegionA, dstPixelRod );
		//TUTTLE_TCOUT_VAR( intersectRegionA );
		//TUTTLE_TCOUT_VAR( procWindowSrcA );
		//TUTTLE_TCOUT_VAR( procWindowOutputA );

		View src = subimage_view( viewA,
				procWindowSrcA.x1, procWindowSrcA.y1,
				intersectRegionASize.x, intersectRegionASize.y );
		View dst = subimage_view( dstView,
				procWindowOutputA.x1, procWindowOutputA.y1,
				intersectRegionASize.x, intersectRegionASize.y );
		
		/// @todo tuttle: fill only the good regions
		copy_pixels( src, dst );
	}
	{
		// fill with B
		const OfxRectI intersectRegionB = rectanglesIntersection( procWindowRoW, srcBPixelRod );
		const OfxPointI intersectRegionBSize = {
			intersectRegionB.x2 - intersectRegionB.x1,
			intersectRegionB.y2 - intersectRegionB.y1
		};
		const OfxRectI procWindowSrcB = translateRegion( intersectRegionB, srcBPixelRod );
		const OfxRectI procWindowOutputB = translateRegion( intersectRegionB, dstPixelRod );
		//TUTTLE_TCOUT_VAR( intersectRegionB );
		//TUTTLE_TCOUT_VAR( procWindowSrcB );
		//TUTTLE_TCOUT_VAR( procWindowOutputB );

		View src = subimage_view( viewB,
				procWindowSrcB.x1, procWindowSrcB.y1,
				intersectRegionBSize.x, intersectRegionBSize.y );
		View dst = subimage_view( dstView,
				procWindowOutputB.x1, procWindowOutputB.y1,
				intersectRegionBSize.x, intersectRegionBSize.y );
		
		/// @todo tuttle: fill only the good regions
		copy_pixels( src, dst );
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
	const OfxPointI procWindowSize = {
		procWindowRoW.x2 - procWindowRoW.x1,
		procWindowRoW.y2 - procWindowRoW.y1
	};
	// fill with A
	const OfxRectI procWindowSrc = translateRegion( procWindowRoW, srcAPixelRod );
	const OfxRectI procWindowOutput = translateRegion( procWindowRoW, dstPixelRod );

	View src = subimage_view( viewA,
				procWindowSrc.x1, procWindowSrc.y1,
				procWindowSize.x, procWindowSize.y );
	View dst = subimage_view( dstView,
				procWindowOutput.x1, procWindowOutput.y1,
				procWindowSize.x, procWindowSize.y );
	
	/// @todo tuttle: fill only the good regions
	copy_pixels( src, dst );
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
