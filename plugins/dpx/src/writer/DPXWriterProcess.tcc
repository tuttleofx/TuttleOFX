/**
 * @file DPXWriterProcess.tcc
 * @brief
 * @author
 * @date    16/12/09 15:34
 *
 */


#include "DPXWriterDefinitions.hpp"
#include "DPXWriterProcess.hpp"

#include <tuttle/common/image/gilGlobals.hpp>
#include <tuttle/common/image/gilViewTypes.hpp>
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
namespace dpx {
namespace writer {

using namespace boost::gil;

template<class View>
DPXWriterProcess<View>::DPXWriterProcess( DPXWriterPlugin &instance )
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
void DPXWriterProcess<View>::setupAndProcess( const OFX::RenderArguments &args )
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
void DPXWriterProcess<View>::multiThreadProcessImages( OfxRectI procWindow )
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
						writeImage<rgb16_image_t>( src, filepath, 16, tuttle::io::DpxImage::eCompTypeR16G16B16, packing );
						break;
					}
					case 1: {
						writeImage<rgba16_image_t>( src, filepath, 16, tuttle::io::DpxImage::eCompTypeR16G16B16A16, packing );
						break;
					}
					case 2: {
						writeImage<abgr16_image_t>( src, filepath, 16, tuttle::io::DpxImage::eCompTypeA16B16G16R16, packing );
						break;
					}
				}
				break;
			}
			case 1: {
				switch(compType) {
					case 0:
						// Todo to get it working:
						// swap half byte then swap shorts (16 bits)
						// ex: 0405 -> 4050 -> 5040
						// or: roll short by 4 bits right.
//						writeImage<rgb12_image_t>( src, filepath, 12, tuttle::io::DpxImage::eCompTypeR12G12B12, packing );
						throw(PluginException("DPX Writer: Unsupported 12 bits rgb image..."));
						break;
					case 1:
//						writeImage<rgba12_image_t>( src, filepath, 12, tuttle::io::DpxImage::eCompTypeR12G12B12A12, packing );
						throw(PluginException("DPX Writer: Unsupported 12 bits rgba image..."));
						break;
					case 2:
//						writeImage<abgr12_image_t>( src, filepath, 12, tuttle::io::DpxImage::eCompTypeA12B12G12R12, packing );
						throw(PluginException("DPX Writer: Unsupported 12 bits abgr image..."));
						break;
				}
				break;
			}
			case 2: {
				switch(compType) {
					case 0:
//						writeImage<rgb10_packed_image_t>( src, filepath, 10, tuttle::io::DpxImage::eCompTypeR10G10B10, packing );
						throw(PluginException("DPX Writer: Unsupported 10 bits rgb image..."));
						break;
					case 1:
						// Unsupported
//						writeImage<rgba10_view_t>( src, filepath, 10, tuttle::io::DpxImage::eCompTypeR10G10B10A10, packing );
						throw(PluginException("DPX Writer: Unsupported 10 bits rgba image..."));
						break;
					case 2:
//						writeImage<abgr10_view_t>( src, filepath, 10, tuttle::io::DpxImage::eCompTypeA10B10G10R10, packing );
						throw(PluginException("DPX Writer: Unsupported 10 bits abgr image..."));
						break;
				}
				break;
			}
			case 3: {
				switch(compType) {
					case 0:
						writeImage<rgb8_image_t>( src, filepath, 8, tuttle::io::DpxImage::eCompTypeR8G8B8, packing );
						break;
					case 1:
						writeImage<rgba8_image_t>( src, filepath, 8, tuttle::io::DpxImage::eCompTypeR8G8B8A8, packing );
						break;
					case 2:
						writeImage<abgr8_image_t>( src, filepath, 8, tuttle::io::DpxImage::eCompTypeA8B8G8R8, packing );
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
void DPXWriterProcess<View>::writeImage( View& src, std::string& filepath, int bitDepth, tuttle::io::DpxImage::EDPX_CompType eCompType, int packing ) throw( tuttle::plugin::PluginException )
{
	View flippedView = flipped_up_down_view( src );
	CONV_IMAGE img(src.width(), src.height());
	typename CONV_IMAGE::view_t vw(view(img));
	copy_and_convert_pixels(flippedView, vw);
	boost::uint8_t *pData = (boost::uint8_t*)boost::gil::interleaved_view_get_raw_data(vw);
	// always little endian (don't kid around ;) ).
	_dpxHeader.setBigEndian(false);
	// 16 bits default
	_dpxHeader.setBitSize(bitDepth);
	// 1 rgba element
	_dpxHeader.setElementNumber(1);
	_dpxHeader.setElementOffset(sizeof(tuttle::io::DpxHeader), 0);
	_dpxHeader.setWidth(src.width());
	_dpxHeader.setHeight(src.height());
	// Uncompressed by default
	_dpxHeader.setPacking(packing);
	_dpxHeader.setDescriptor(_dpxImg.compTypeToDescriptor(eCompType));
	_dpxImg.setHeader(_dpxHeader);
	_dpxImg.setData(pData, false);
	_dpxImg.write(filepath);
}

}
}
}
}
