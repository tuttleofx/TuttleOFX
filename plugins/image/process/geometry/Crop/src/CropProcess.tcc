#include <tuttle/plugin/global.hpp>
#include <tuttle/common/math/rectOp.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>
#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/image/gil/basic_colors.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/numeric/sampler.hpp>
#include <boost/gil/extension/numeric/resample.hpp>


namespace tuttle {
namespace plugin {
namespace crop {

using namespace boost::gil;

template<class View>
CropProcess<View>::CropProcess( CropPlugin& instance )
	: ImageGilFilterProcessor<View>( instance )
	, _plugin( instance )
{
	_upBand     = instance.fetchIntParam( kParamUp );
	_downBand   = instance.fetchIntParam( kParamDown );
	_leftBand   = instance.fetchIntParam( kParamLeft );
	_rightBand  = instance.fetchIntParam( kParamRight );
	_anamorphic = instance.fetchBooleanParam( kParamAnamorphic );
}

/**
 * @todo tuttle: needs to be rewrite
 */
template<class View>
void CropProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args );

	// SOURCE
	_renderScale = this->_src->getRenderScale();
	_par         = _plugin._clipSrc->getPixelAspectRatio();
	_srcBounds   = this->_src->getBounds();
	_srcROD      = this->_src->getRegionOfDefinition();
	_clipROD     = _plugin._clipSrc->getCanonicalRod( args.time );
	_clipROD.x1 /= _par / this->_src->getRenderScale().x;
	_clipROD.x2 /= _par / this->_src->getRenderScale().x;
	_clipROD.y1 *= this->_src->getRenderScale().y;
	_clipROD.y2 *= this->_src->getRenderScale().y;

	point2<int> srcImgCorner = point2<int>( static_cast<int>( -_srcBounds.x1 ),
	                                        static_cast<int>( -_srcBounds.y1 ) );
	OFX::EBitDepth srcBitDepth         = this->_src->getPixelDepth();
	OFX::EPixelComponent srcComponents = this->_src->getPixelComponents();
	typename image_from_view<View>::type imResized;

	// Stretch source if necessary
	if( _anamorphic->getValue() )
	{
		OfxRectD croppedRect     = getCrop();
		OfxRectD dProcRenderRect = {
			args.renderWindow.x1, args.renderWindow.y1,
			args.renderWindow.x2, args.renderWindow.y2
		};
		OfxRectD finalProcWin = rectanglesIntersection( croppedRect, dProcRenderRect );
		if( finalProcWin.x1 < finalProcWin.x2 && finalProcWin.y1 < finalProcWin.y2 )
		{
			View srcTileView = this->getView( this->_src.get(), _plugin._clipSrc->getPixelRod( args.time ) );
			imResized.recreate( int(finalProcWin.x2 - finalProcWin.x1), ///@todo tuttle: to change !
			                    int(finalProcWin.y2 - finalProcWin.y1) );
			resize_view( srcTileView, view( imResized ), bilinear_sampler() );
			this->_srcView = subimage_view( view( imResized ),
			                                srcImgCorner.x,
			                                srcImgCorner.y,
			                                int(_clipROD.x2 - _clipROD.x1),
			                                int(_clipROD.y2 - _clipROD.y1) );
		}
	}

	// DESTINATION
	OfxRectI dstImgRod    = this->_dst->getRegionOfDefinition();
	OfxRectI dstImgBounds = this->_dst->getBounds();
	OfxRectD dstClipROD   = _plugin._clipDst->getCanonicalRod( args.time );
	dstClipROD.x1 /= _par / this->_src->getRenderScale().x;
	dstClipROD.x2 /= _par / this->_src->getRenderScale().x;
	dstClipROD.y1 *= this->_src->getRenderScale().y;
	dstClipROD.y2 *= this->_src->getRenderScale().y;

	point2<int> dstImgCorner = point2<int>( static_cast<int>( -dstImgBounds.x1 ),
	                                        static_cast<int>( -dstImgBounds.y1 ) );

	OFX::EBitDepth dstBitDepth         = this->_dst->getPixelDepth();
	OFX::EPixelComponent dstComponents = this->_dst->getPixelComponents();

	// Make sure bit depths are same
	if( srcBitDepth != dstBitDepth || srcComponents != dstComponents )
	{
		BOOST_THROW_EXCEPTION( exception::BitDepthMismatch() );
	}

	// Build destination view
	View dstTileView = this->getView( this->_dst.get(), _plugin._clipDst->getPixelRod( args.time ) );

	this->_dstView = subimage_view( dstTileView, dstImgCorner.x, dstImgCorner.y,
	                                int(dstClipROD.x2 - dstClipROD.x1),
	                                int(dstClipROD.y2 - dstClipROD.y1) );

	// Begin progression
	this->progressBegin( std::abs( dstImgBounds.y2 - dstImgBounds.y1 ), "Crop" );
}

/**
 * Return rectangle where source pixels have to be copied in the destination.
 */
template<class View>
OfxRectD CropProcess<View>::getCrop() const
{
	double up    = _upBand->getValue()    * _renderScale.y;
	double down  = _downBand->getValue()  * _renderScale.y;
	double left  = _leftBand->getValue()  * _renderScale.x;
	double right = _rightBand->getValue() * _renderScale.x;

	if( up > _clipROD.y2 )
		up = _clipROD.y2;
	if( down < _clipROD.y1 )
		down = _clipROD.y1;
	if( right > _clipROD.x2 )
		right = _clipROD.x2;
	if( left < _clipROD.x1 )
		left = _clipROD.x1;
	OfxRectD rect = { left, down, _clipROD.x2 - right, _clipROD.y2 - up };
	return rect;
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template<class View>
void CropProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	///@todo tuttle: is it possible to place the crop depending on the RoW and not on image source RoD ?
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );

	typedef typename View::x_iterator x_iterator;
	Pixel fillingColor = get_black( this->_srcView );

	OfxRectD croppedRect = getCrop();

	if( croppedRect.x1 < croppedRect.x2 && croppedRect.y1 < croppedRect.y2 )
	{
		OfxRectD dProcWinRect  = { procWindowOutput.x1, procWindowOutput.y1, procWindowOutput.x2, procWindowOutput.y2 };
		OfxRectD finalProcWin  = rectanglesIntersection( croppedRect, dProcWinRect );
		OfxRectI iFinalProcWin = { (int)finalProcWin.x1, (int)finalProcWin.y1, (int)finalProcWin.x2, (int)finalProcWin.y2 };

		// If no image copy is needed, fill destination with black
		if( iFinalProcWin.x1 == iFinalProcWin.x2 || iFinalProcWin.y1 == iFinalProcWin.y2 )
		{
			for( int y = procWindowOutput.y1; y < procWindowOutput.y2; ++y )
			{
				if( this->progressForward() )
					return;
				x_iterator dIt = this->_dstView.x_at( procWindowOutput.x1, y );
				// Right band
				for( int x = procWindowOutput.x1; x < procWindowOutput.x2; ++x )
				{
					*dIt = fillingColor;
					++dIt;
				}
			}
		}
		else
		{
			// Fill bands
			// bottom band
			for( int y = procWindowOutput.y1; y < iFinalProcWin.y1; ++y )
			{
				if( this->progressForward() )
					return;
				x_iterator dIt = this->_dstView.x_at( procWindowOutput.x1, y );
				// Here goes the image pixel copy
				for( int x = procWindowOutput.x1; x < procWindowOutput.x2; ++x )
				{
					*dIt = fillingColor;
					++dIt;
				}
			}

			// Top band
			for( int y = iFinalProcWin.y2; y < procWindowOutput.y2; ++y )
			{
				if( this->progressForward() )
					return;
				x_iterator dIt = this->_dstView.x_at( procWindowOutput.x1, y );
				// Here goes the image pixel copy
				for( int x = procWindowOutput.x1; x < procWindowOutput.x2; ++x )
				{
					*dIt = fillingColor;
					++dIt;
				}
			}

			// Image copy
			for( int y = iFinalProcWin.y1; y < iFinalProcWin.y2; ++y )
			{
				if( this->progressForward() )
					return;
				x_iterator sIt = this->_srcView.x_at( procWindowOutput.x1, y );
				x_iterator dIt = this->_dstView.x_at( procWindowOutput.x1, y );

				// Left band
				for( int x = procWindowOutput.x1; x < iFinalProcWin.x1; ++x )
				{
					*dIt = fillingColor;
					++dIt;
					++sIt;
				}

				// Here goes the image pixel copy
				for( int x = iFinalProcWin.x1; x < iFinalProcWin.x2; ++x )
				{
					( *dIt ) = ( *sIt );
					++dIt;
					++sIt;
				}

				// Right band
				for( int x = iFinalProcWin.x2; x < procWindowOutput.x2; ++x )
				{
					*dIt = fillingColor;
					++dIt;
				}
			}
		}
	}
}

}
}
}
