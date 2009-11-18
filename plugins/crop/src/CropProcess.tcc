/**
 * @file CropProcess.tcc
 * @brief
 * @author
 * @date    01/10/09 14:14
 *
 */

#include "OFX/common/utils/global.hpp"
#include "OFX/common/math/rectOp.hpp"
#include "OFX/plugin/ImageGilProcessor.hpp"
#include "OFX/plugin/Progress.hpp"
#include "OFX/plugin/PluginException.hpp"
#include "OFX/common/image/gilGlobals.hpp"

#include <cmath>
#include <vector>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/gil/extension/numeric/sampler.hpp>
#include <boost/gil/extension/numeric/resample.hpp>

namespace OFX {

using namespace boost::gil;

template<class View>
CropProcess<View>::CropProcess( CropPlugin &instance )
: OFX::ofx::ImageGilProcessor<View>( instance )
, OFX::ofx::Progress( instance )
, _plugin( instance )
{
    _upBand     = instance.fetchIntParam( kParamUp );
    _downBand   = instance.fetchIntParam( kParamDown );
    _leftBand   = instance.fetchIntParam( kParamLeft );
    _rightBand  = instance.fetchIntParam( kParamRight );
    _anamorphic = instance.fetchBooleanParam( kParamAnamorphic );
}

template<class View>
void CropProcess<View>::setupAndProcess( const OFX::RenderArguments &args )
{
    try
    {
        COUT("Crop setup");
        // SOURCE
        boost::scoped_ptr<OFX::Image> src( _plugin.getSrcClip( )->fetchImage( args.time ) );
        _renderScale = src->getRenderScale( );
        _par = _plugin.getSrcClip( )->getPixelAspectRatio( );
        _srcBounds = src->getBounds( );
        _srcROD  = src->getRegionOfDefinition( );
        _clipROD = _plugin.getSrcClip( )->getRegionOfDefinition( args.time );
        _clipROD.x1 /= _par / src->getRenderScale( ).x;
        _clipROD.x2 /= _par / src->getRenderScale( ).x;
        _clipROD.y1 *= src->getRenderScale( ).y;
        _clipROD.y2 *= src->getRenderScale( ).y;

        point2<int> srcImgCorner = point2<int>( static_cast<int>( - _srcBounds.x1 ),
                                                static_cast<int>( - _srcBounds.y1 ) );
        if( !src.get( ) )
            throw( ImageNotReadyException( ) );
        OFX::BitDepthEnum srcBitDepth = src->getPixelDepth( );
        OFX::PixelComponentEnum srcComponents = src->getPixelComponents( );
        typename image_from_view<View>::type imResized;

        // Build destination view
        View srcTileView = interleaved_view( std::abs( _srcBounds.x2 - _srcBounds.x1 ),
                                             std::abs( _srcBounds.y2 - _srcBounds.y1 ),
                                             static_cast<value_t*>(src->getPixelData()),
                                             src->getRowBytes() );

        // Stretch source if necessary
        if ( _anamorphic->getValue( ) ) {
            OfxRectD croppedRect = getCrop( );
            OfxRectD dProcRenderRect = { args.renderWindow.x1, args.renderWindow.y1,
                                         args.renderWindow.x2, args.renderWindow.y2 };
            OfxRectD finalProcWin = intersection( croppedRect, dProcRenderRect );
            if ( finalProcWin.x1 < finalProcWin.x2 && finalProcWin.y1 < finalProcWin.y2 ) {
                imResized.recreate( int( finalProcWin.x2 - finalProcWin.x1 ),
                                    int( finalProcWin.y2 - finalProcWin.y1 ) );
                resize_view( srcTileView, view( imResized ), bilinear_sampler( ) );
                this->_srcView = subimage_view( view( imResized ),
                                                srcImgCorner.x,
                                                srcImgCorner.y,
                                                int( _clipROD.x2 - _clipROD.x1 ) ,
                                                int( _clipROD.y2 - _clipROD.y1 ) );
            } else {
                this->_srcView = subimage_view( srcTileView, srcImgCorner.x, srcImgCorner.y,
                                                int( _clipROD.x2 - _clipROD.x1 ),
                                                int( _clipROD.y2 - _clipROD.y1 ) );
            }
        } else {
            this->_srcView = subimage_view( srcTileView, srcImgCorner.x, srcImgCorner.y,
                                            int( _clipROD.x2 - _clipROD.x1 ),
                                            int( _clipROD.y2 - _clipROD.y1 ) );
        }

        // DESTINATION
        boost::scoped_ptr<OFX::Image> dst( _plugin.getDstClip( )->fetchImage( args.time ) );
        if( !dst.get( ) )
            throw( ImageNotReadyException( ) );
        OfxRectI dstImgRod = dst->getRegionOfDefinition( );
        OfxRectI dstImgBounds = dst->getBounds();
        OfxRectD dstClipROD = _plugin.getDstClip( )->getRegionOfDefinition( args.time );
        dstClipROD.x1 /= _par / src->getRenderScale( ).x;
        dstClipROD.x2 /= _par / src->getRenderScale( ).x;
        dstClipROD.y1 *= src->getRenderScale( ).y;
        dstClipROD.y2 *= src->getRenderScale( ).y;

        point2<int> dstImgCorner = point2<int>( static_cast<int>( -dstImgBounds.x1 ),
                                                static_cast<int>( -dstImgBounds.y1 ) );

        OFX::BitDepthEnum dstBitDepth = dst->getPixelDepth( );
        OFX::PixelComponentEnum dstComponents = dst->getPixelComponents( );

        // Make sure bit depths are same
        if( srcBitDepth != dstBitDepth || srcComponents != dstComponents )
        {
            throw(BitDepthMismatchException( ) );
        }

        // Build destination view
        View dstTileView = interleaved_view( std::abs( dstImgBounds.x2 - dstImgBounds.x1 ),
                                             std::abs( dstImgBounds.y2 - dstImgBounds.y1 ),
                                             static_cast<value_t*>( dst->getPixelData( ) ),
                                             dst->getRowBytes( ) );

        this->_dstView = subimage_view( dstTileView, dstImgCorner.x, dstImgCorner.y,
                                        int( dstClipROD.x2 - dstClipROD.x1 ),
                                        int( dstClipROD.y2 - dstClipROD.y1 ) );

        // Begin progression
        progressBegin( std::abs( dstImgBounds.y2 - dstImgBounds.y1 ), "Crop" );

        this->setRenderWindow( args.renderWindow );
        // Call the base class process member
        this->process( );
        progressEnd( );
        COUT("/Crop setup");
    }
    catch( PluginException e )
    {
        COUT_EXCEPTION( e );
    }
}

/**
  * Return rectangle where source pixels have to be copied in the destination.
  */
template<class View>
OfxRectD CropProcess<View>::getCrop( ) const {
    double up      = _upBand->getValue( )    * _renderScale.y;
    double down    = _downBand->getValue( )  * _renderScale.y;
    double left    = _leftBand->getValue( )  * _renderScale.x;
    double right   = _rightBand->getValue( ) * _renderScale.x;
    if ( up > _clipROD.y2 )
        up = _clipROD.y2;
    if ( down < _clipROD.y1 )
        down = _clipROD.y1;
    if ( right > _clipROD.x2 )
        right = _clipROD.x2;
    if ( left < _clipROD.x1 )
        left = _clipROD.x1;
    OfxRectD rect = { left, down, _clipROD.x2 - right, _clipROD.y2 - up };
    return rect;
}

/**
 * @brief Function called by rendering thread each time 
 *        a process must be done.
 *
 * @param[in] procWindow  Processing window
 */
template<class View>
void CropProcess<View>::multiThreadProcessImages( OfxRectI procWindow )
{
    typedef typename View::x_iterator iterator;
    value_t fillingColor = get_black( _srcView );
    int nc      = _srcView.num_channels( );

    OfxRectD croppedRect = getCrop( );

    if (croppedRect.x1 < croppedRect.x2 && croppedRect.y1 < croppedRect.y2) {
        OfxRectD dProcWinRect = { procWindow.x1, procWindow.y1, procWindow.x2, procWindow.y2 };
        OfxRectD finalProcWin = intersection(croppedRect, dProcWinRect);
        OfxRectI iFinalProcWin = { (int)finalProcWin.x1, (int)finalProcWin.y1, (int)finalProcWin.x2, (int)finalProcWin.y2 };
        COUT_VAR4(procWindow.x1, procWindow.y1, procWindow.x2, procWindow.y2);
        COUT_VAR4(iFinalProcWin.x1, iFinalProcWin.y1, iFinalProcWin.x2, iFinalProcWin.y2);

        // If no image copy is needed, fill destination with black
        if ( iFinalProcWin.x1 == iFinalProcWin.x2 || iFinalProcWin.y1 == iFinalProcWin.y2 ) {
            for ( int y = procWindow.y1; y < procWindow.y2; y++ ) {
                if( progressForward( ) )
                    return;
                iterator dIt = this->_dstView.x_at( procWindow.x1, y );
                // Right band
                for ( int x = procWindow.x1; x < procWindow.x2; x++ ) {
                    *dIt = fillingColor;
                    dIt++;
                }
            }
        } else {
            // Fill bands
            // bottom band
            for ( int y = procWindow.y1; y < iFinalProcWin.y1; y++ ) {
                if( progressForward( ) )
                    return;
                iterator dIt = this->_dstView.x_at( procWindow.x1, y );
                // Here goes the image pixel copy
                for ( int x = procWindow.x1; x < procWindow.x2; x++ ) {
                    *dIt = fillingColor;
                    dIt++;
                }
            }

            // Top band
            for ( int y = iFinalProcWin.y2; y < procWindow.y2; y++ ) {
                if( progressForward( ) )
                    return;
                iterator dIt = this->_dstView.x_at( procWindow.x1, y );
                // Here goes the image pixel copy
                for ( int x = procWindow.x1; x < procWindow.x2; x++ ) {
                    *dIt = fillingColor;
                    dIt++;
                }
            }

            // Image copy
            for ( int y = iFinalProcWin.y1; y < iFinalProcWin.y2; y++ ) {
                if( progressForward( ) )
                    return;
                iterator sIt = _srcView.x_at( procWindow.x1, y );
                iterator dIt = this->_dstView.x_at( procWindow.x1, y );

                // Left band
                for ( int x = procWindow.x1; x < iFinalProcWin.x1; x++ ) {
                    *dIt = fillingColor;
                    dIt++;
                    sIt++;
                }

                // Here goes the image pixel copy
                for ( int x = iFinalProcWin.x1; x < iFinalProcWin.x2; x++ ) {
                    for ( int c = 0; c < nc; c++) {
                        (*dIt)[c] = (*sIt)[c];
                    }
                    dIt++;
                    sIt++;
                }

                // Right band
                for ( int x = iFinalProcWin.x2; x < procWindow.x2; x++ ) {
                    *dIt = fillingColor;
                    dIt++;
                }
            }
        }
    }
}

}
