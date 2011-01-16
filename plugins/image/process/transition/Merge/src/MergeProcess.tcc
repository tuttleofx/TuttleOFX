#include "gil/toolbox/ViewsMerging.hpp"
#include "MergePlugin.hpp"
#include "MergeDefinitions.hpp"
#include "MergeProcess.hpp"
#include <boost/gil/extension/color/hsl.hpp>
#include <tuttle/common/math/rectOp.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/numeric/pixel_numeric_operations.hpp>

namespace tuttle {
namespace plugin {
namespace merge {

template<class View, class Functor>
MergeProcess<View, Functor>::MergeProcess( MergePlugin& instance )
	: ImageGilProcessor<View>( instance )
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
	this->_srcViewA = this->getView( _srcA.get(), _plugin._clipSrcA->getPixelRod( args.time ) );
	//	_srcPixelRodA = _srcA->getRegionOfDefinition(); // bug in nuke, returns bounds
	_srcPixelRodA = _plugin._clipSrcA->getPixelRod( args.time );

	// clip B
	_srcB.reset( _plugin._clipSrcB->fetchImage( args.time ) );
	if( !_srcB.get() )
		BOOST_THROW_EXCEPTION( exception::ImageNotReady() );
	if( _srcB->getRowBytes() == 0 )
		BOOST_THROW_EXCEPTION( exception::WrongRowBytes() );
	this->_srcViewB = this->getView( _srcB.get(), _plugin._clipSrcB->getPixelRod( args.time ) );
	//	_srcPixelRodB = _srcB->getRegionOfDefinition(); // bug in nuke, returns bounds
	_srcPixelRodB = _plugin._clipSrcB->getPixelRod( args.time );

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

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template<class View, class Functor>
void MergeProcess<View, Functor>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	OfxPointI procWindowSize = {
		procWindowRoW.x2 - procWindowRoW.x1,
		procWindowRoW.y2 - procWindowRoW.y1
	};

	OfxRectI srcRodA = translateRegion( _srcPixelRodA, _params._offsetA );
	OfxRectI srcRodB = translateRegion( _srcPixelRodB, _params._offsetB );
	
	OfxRectI intersect = rectanglesIntersection( srcRodA, srcRodB );
	OfxRectI procIntersect = rectanglesIntersection( procWindowRoW, intersect );
	OfxPointI procIntersectSize = {
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
			/// @todo tuttle:
			///  * add color choice
			///  * fill A and B
			///  * fill only the good regions
			Pixel pixelZero; pixel_zeros_t<Pixel>()( pixelZero );
			const OfxRectI procWindowOutput = translateRegion( procWindowRoW, this->_dstPixelRod );
			View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
													  procWindowSize.x, procWindowSize.y );
			fill_pixels( dst, pixelZero );
			break;
		}
		case eParamRodA:
		{
			// fill with A
			const OfxRectI procWindowSrc = translateRegion( procWindowRoW, srcRodA );
			const OfxRectI procWindowOutput = translateRegion( procWindowRoW, this->_dstPixelRod );

			View src = subimage_view( this->_srcViewA, procWindowSrc.x1, procWindowSrc.y1,
													   procWindowSize.x, procWindowSize.y );
			View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
													  procWindowSize.x, procWindowSize.y );
			/// @todo tuttle: fill only the good regions
			copy_pixels( src, dst );
			break;
		}
		case eParamRodB:
		{
			// fill with B
			const OfxRectI procWindowSrc = translateRegion( procWindowRoW, srcRodB );
			const OfxRectI procWindowOutput = translateRegion( procWindowRoW, this->_dstPixelRod );

			View src = subimage_view( this->_srcViewB, procWindowSrc.x1, procWindowSrc.y1,
													   procWindowSize.x, procWindowSize.y );
			View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
													  procWindowSize.x, procWindowSize.y );
			/// @todo tuttle: fill only the good regions
			copy_pixels( src, dst );
			break;
		}
	}

	View srcViewA_inter = subimage_view( this->_srcViewA,
						   procIntersect.x1 - srcRodA.x1,
						   procIntersect.y1 - srcRodA.y1,
						   procIntersectSize.x,
						   procIntersectSize.y );
	View srcViewB_inter = subimage_view( this->_srcViewB,
						   procIntersect.x1 - srcRodB.x1,
						   procIntersect.y1 - srcRodB.y1,
						   procIntersectSize.x,
						   procIntersectSize.y );

	View dstView_inter = subimage_view( this->_dstView,
	                              procIntersect.x1 - this->_dstPixelRod.x1,
	                              procIntersect.y1 - this->_dstPixelRod.y1,
	                              procIntersectSize.x,
	                              procIntersectSize.y );

	merge_pixels( srcViewA_inter, srcViewB_inter, dstView_inter, Functor() );


}

}
}
}
