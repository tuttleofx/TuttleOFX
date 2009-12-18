/**
 * @file EXRWriterProcess.tcc
 * @brief
 * @author
 * @date    16/12/09 15:34
 *
 */


#include "EXRWriterDefinitions.hpp"
#include "EXRWriterProcess.hpp"

#include <tuttle/common/image/gilGlobals.hpp>
#include <half/gilHalf.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/Progress.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <cstdlib>
#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/cstdint.hpp>

namespace tuttle {
namespace plugin {
namespace exr {
namespace writer {

using namespace boost::gil;

template<class View>
EXRWriterProcess<View>::EXRWriterProcess( EXRWriterPlugin &instance )
: tuttle::plugin::ImageGilProcessor<View>( instance )
, tuttle::plugin::Progress( instance )
, _plugin( instance )
{
	_filepath = instance.fetchStringParam( kOutputFilename );
	_bitDepth = instance.fetchChoiceParam( kParamBitDepth );
	_componentsType = instance.fetchChoiceParam( kParamComponentsType );
	_compressed = instance.fetchBooleanParam( kParamCompressed );
}

template<class View>
void EXRWriterProcess<View>::setupAndProcess( const OFX::RenderArguments &args )
{
    try
    {
        boost::scoped_ptr<OFX::Image> src( _plugin.getSrcClip( )->fetchImage( args.time ) );
        if( !src.get( ) )
            throw( ImageNotReadyException( ) );
        OfxRectI sBounds = src->getBounds( );
        OFX::BitDepthEnum srcBitDepth = src->getPixelDepth( );
        OFX::PixelComponentEnum srcComponents = src->getPixelComponents( );

        // Build source view
        this->_srcView = interleaved_view( std::abs( sBounds.x2 - sBounds.x1 ), std::abs( sBounds.y2 - sBounds.y1 ),
                                           static_cast < value_t* > ( src->getPixelData( ) ),
                                           src->getRowBytes( ) );

        boost::scoped_ptr<OFX::Image> dst( _plugin.getDstClip( )->fetchImage( args.time ) );
        if( !dst.get( ) )
            throw( ImageNotReadyException( ) );
        OfxRectI dBounds = dst->getBounds( );
        OFX::BitDepthEnum dstBitDepth = dst->getPixelDepth( );
        OFX::PixelComponentEnum dstComponents = dst->getPixelComponents( );

        // Make sure bit depths are same
        if( srcBitDepth != dstBitDepth || srcComponents != dstComponents )
        {
            throw( BitDepthMismatchException( ) );
        }

        // Build destination view
        this->_dstView = interleaved_view( std::abs( dBounds.x2 - dBounds.x1 ), std::abs( dBounds.y2 - dBounds.y1 ),
                                           static_cast < value_t* > ( dst->getPixelData( ) ),
                                           dst->getRowBytes( ) );

        // Set the render window
        this->setRenderWindow( args.renderWindow );
        // Call the base class process member
        this->process( );
    }
    catch( PluginException e )
    {
        COUT_EXCEPTION( e );
    }
}

/**
 * @brief Function called by rendering thread each time 
 *        a process must be done.
 *
 * @param[in] procWindow  Processing window
 */
template<class View>
void EXRWriterProcess<View>::multiThreadProcessImages( OfxRectI procWindow )
{
    try
    {
        View src = subimage_view( this->_srcView, procWindow.x1, procWindow.y1,
                                  procWindow.x2 - procWindow.x1,
                                  procWindow.y2 - procWindow.y1 );
        View dst = subimage_view( this->_dstView, procWindow.x1, procWindow.y1,
                                  procWindow.x2 - procWindow.x1,
                                  procWindow.y2 - procWindow.y1 );
		std::string filepath;
		int bitDepth, compType;
		this->_filepath->getValue( filepath );
		int packing = _compressed->getValue() == false;
		_bitDepth->getValue(bitDepth);
		_componentsType->getValue(compType);
		switch(bitDepth)
		{
			case 0: {
				switch(compType) {
					case 0: {
						break;
					}
					case 1: {
						break;
					}
					case 2: {
						break;
					}
				}
				break;
			}
			case 1: {
				switch(compType) {
					case 0:
//						writeImage<rgb12_image_t>( src, filepath, 12, tuttle::io::ExrImage::eCompTypeR12G12B12, packing );
						throw(PluginException("EXR Writer: Unsupported 12 bits rgb image..."));
						break;
					case 1:
//						writeImage<rgba12_image_t>( src, filepath, 12, tuttle::io::ExrImage::eCompTypeR12G12B12A12, packing );
						throw(PluginException("EXR Writer: Unsupported 12 bits rgba image..."));
						break;
					case 2:
//						writeImage<abgr12_image_t>( src, filepath, 12, tuttle::io::ExrImage::eCompTypeA12B12G12R12, packing );
						throw(PluginException("EXR Writer: Unsupported 12 bits abgr image..."));
						break;
				}
				break;
			}
			case 2: {
				switch(compType) {
					case 0:
//						writeImage<rgb10_packed_image_t>( src, filepath, 10, tuttle::io::ExrImage::eCompTypeR10G10B10, packing );
						throw(PluginException("EXR Writer: Unsupported 10 bits rgb image..."));
						break;
					case 1:
						// Unsupported
//						writeImage<rgba10_view_t>( src, filepath, 10, tuttle::io::ExrImage::eCompTypeR10G10B10A10, packing );
						throw(PluginException("EXR Writer: Unsupported 10 bits rgba image..."));
						break;
					case 2:
//						writeImage<abgr10_view_t>( src, filepath, 10, tuttle::io::ExrImage::eCompTypeA10B10G10R10, packing );
						throw(PluginException("EXR Writer: Unsupported 10 bits abgr image..."));
						break;
				}
				break;
			}
			case 3: {
				switch(compType) {
					case 0:
						break;
					case 1:
						break;
					case 2:
						break;
				}
				break;
			}
		}
    }
    catch( PluginException err )
    {
        COUT_EXCEPTION( err );
    }
}

/**
 * @brief Function called to apply an anisotropic blur
 *
 * @param[out]  dst     Destination image view
 * @param[in]   amplitude     Amplitude of the anisotropic blur
 * @param dl    spatial discretization.
 * @param da    angular discretization.
 * @param gauss_prec    precision of the gaussian function
 * @param fast_approx   Tell to use the fast approximation or not.
 *
 * @return Result view of the blurring process
 */


//// TODO: FINISH REFACTORING !
template<class View>
template<class CONV_IMAGE>
void EXRWriterProcess<View>::writeImage( View& src, std::string& filepath, int bitDepth, int eCompType, int packing ) throw( tuttle::plugin::PluginException )
{
	View flippedView = flipped_up_down_view( src );
	CONV_IMAGE img(src.width(), src.height());
	typename CONV_IMAGE::view_t vw(view(img));
	copy_and_convert_pixels(flippedView, vw);
	boost::uint8_t *pData = (boost::uint8_t*)boost::gil::interleaved_view_get_raw_data(vw);
}

}
}
}
}
