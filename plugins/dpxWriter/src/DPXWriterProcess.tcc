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
		this->_filepath->getValue( filepath );
		writeImage( src, filepath );
        copy_pixels( src, dst );

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
void DPXWriterProcess<View>::writeImage( View& src, std::string& filepath ) throw( tuttle::plugin::PluginException )
{
	View flippedView = flipped_up_down_view( src );
	int bitDepth, compType;
	tuttle::io::DpxImage::EDPX_CompType eCompType = tuttle::io::DpxImage::eCompTypeUnknown;
	int packing = _compressed->getValue() == false;
	_bitDepth->getValue(bitDepth);
	_componentsType->getValue(compType);
	boost::uint8_t *pData = NULL;

	switch(bitDepth)
	{
		case 0: {
			bitDepth = 16;
			switch(compType) {
				case 0: {
					rgb16_image_t img(src.width(), src.height());
					rgb16_view_t vw(view(img));
					copy_and_convert_pixels(flippedView, vw);
					pData = (boost::uint8_t*)boost::gil::interleaved_view_get_raw_data(vw);
					eCompType = tuttle::io::DpxImage::eCompTypeR16G16B16;
					// always little endian (don't kid around ;) ).
					_dpxHeader.setBigEndian(false);
					// 16 bits default
					_dpxHeader.setBitSize(bitDepth);
					// 1 rgba element
					_dpxHeader.setElementNumber(1);
					_dpxHeader.setWidth(src.width());
					_dpxHeader.setHeight(src.height());
					// Uncompressed by default
					_dpxHeader.setPacking(packing);
					_dpxHeader.setDescriptor(_dpxImg.compTypeToDescriptor(eCompType));
					_dpxImg.setHeader(_dpxHeader);
					_dpxImg.setData(pData, false);
					_dpxImg.write(filepath);
					break;
				}
				case 1: {
					rgba16_image_t img(src.width(), src.height());
					rgba16_view_t vw(view(img));
					copy_and_convert_pixels(flippedView, vw);
					pData = (boost::uint8_t*)boost::gil::interleaved_view_get_raw_data(vw);
					eCompType = tuttle::io::DpxImage::eCompTypeR16G16B16A16;
					// always little endian (don't kid around ;) ).
					_dpxHeader.setBigEndian(false);
					// 16 bits default
					_dpxHeader.setBitSize(bitDepth);
					// 1 rgba element
					_dpxHeader.setElementNumber(1);
					_dpxHeader.setWidth(src.width());
					_dpxHeader.setHeight(src.height());
					// Uncompressed by default
					_dpxHeader.setPacking(packing);
					_dpxHeader.setDescriptor(_dpxImg.compTypeToDescriptor(eCompType));
					_dpxImg.setHeader(_dpxHeader);
					_dpxImg.setData(pData, false);
					_dpxImg.write(filepath);
					break;
				}
				case 2: {
					abgr16_image_t img(src.width(), src.height());
					abgr16_view_t vw(view(img));
					copy_and_convert_pixels(flippedView, vw);
					pData = (boost::uint8_t*)boost::gil::interleaved_view_get_raw_data(vw);
					eCompType = tuttle::io::DpxImage::eCompTypeA16B16G16R16;
					// always little endian (don't kid around ;) ).
					_dpxHeader.setBigEndian(false);
					// 16 bits default
					_dpxHeader.setBitSize(bitDepth);
					// 1 rgba element
					_dpxHeader.setElementNumber(1);
					_dpxHeader.setWidth(src.width());
					_dpxHeader.setHeight(src.height());
					// Uncompressed by default
					_dpxHeader.setPacking(packing);
					_dpxHeader.setDescriptor(_dpxImg.compTypeToDescriptor(eCompType));
					_dpxImg.setHeader(_dpxHeader);
					_dpxImg.setData(pData, false);
					_dpxImg.write(filepath);
					break;
				}
			}
			break;
		}
		case 1: {
			bitDepth = 12;
			rgba12_image_t img(src.width(), src.height());
			rgba12_view_t vw(view(img));
			copy_and_convert_pixels(flippedView, vw);
			pData = (boost::uint8_t*)boost::gil::interleaved_view_get_raw_data(vw);
			switch(compType) {
				case 0:
					eCompType = tuttle::io::DpxImage::eCompTypeR12G12B12;
					break;
				case 1:
					eCompType = tuttle::io::DpxImage::eCompTypeR12G12B12A12;
					break;
				case 2:
					eCompType = tuttle::io::DpxImage::eCompTypeA12B12G12R12;
					break;
			}
			break;
		}
		case 2: {
			bitDepth = 10;
			rgba10_image_t img(src.width(), src.height());
			rgba10_view_t vw(view(img));
			copy_and_convert_pixels(flippedView, vw);
			pData = (boost::uint8_t*)boost::gil::interleaved_view_get_raw_data(vw);
			switch(compType) {
				case 0:
					eCompType = tuttle::io::DpxImage::eCompTypeR10G10B10;
					break;
				case 1:
					eCompType = tuttle::io::DpxImage::eCompTypeR10G10B10A10;
					break;
				case 2:
					eCompType = tuttle::io::DpxImage::eCompTypeA10B10G10R10;
					break;
			}
			break;
		}
		case 3: {
			rgba8_image_t img(src.width(), src.height());
			rgba8_view_t vw(view(img));
			copy_and_convert_pixels(flippedView, vw);
			pData = (boost::uint8_t*)boost::gil::interleaved_view_get_raw_data(vw);
			switch(compType) {
				case 0:
					eCompType = tuttle::io::DpxImage::eCompTypeR8G8B8;
					break;
				case 1:
					eCompType = tuttle::io::DpxImage::eCompTypeR8G8B8A8;
					break;
				case 2:
					eCompType = tuttle::io::DpxImage::eCompTypeA8B8G8R8;
					break;
			}
			bitDepth = 8;
			break;
		}
	}
}

}
}
}
