#include "DPXReaderPlugin.hpp"

#include <tuttle/common/image/gilGlobals.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/Progress.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/gil/packed_pixel.hpp>

#include <boost/gil/extension/io/png_io.hpp>
#include <boost/gil/extension/io/png_dynamic_io.hpp>
#include <boost/cstdint.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/filesystem/fstream.hpp>


using namespace boost::gil;
namespace bfs = boost::filesystem;

typedef struct uint40_t {
    uint32_t rgb;
    uint8_t  a;
} uint40_t;

namespace tuttle {
namespace plugin {
namespace dpx {

template<class View>
DPXReaderProcess<View>::DPXReaderProcess( DPXReaderPlugin &instance )
: tuttle::plugin::ImageGilProcessor<View>( instance ), tuttle::plugin::Progress( instance ),
_plugin( instance )
{
    _filepath = instance.fetchStringParam( "Input filename" );
    assert(_filepath != NULL);
}

template<class View>
void DPXReaderProcess<View>::setupAndProcess( const OFX::RenderArguments &args )
{
    try
    {
        std::string sFilepath;
        // Fetch output image
        _filepath->getValue(sFilepath);
        if ( bfs::exists(sFilepath) ) {
            _plugin.getDpxImg().read(sFilepath);

            point2<ptrdiff_t> imageDims( _plugin.getDpxImg().width(),
                                         _plugin.getDpxImg().height() );

            double par = _plugin.getDstClip( )->getPixelAspectRatio();
            OfxRectD reqRect = { 0, 0, imageDims.x * par, imageDims.y };
            boost::scoped_ptr<OFX::Image> dst( _plugin.getDstClip( )->fetchImage( args.time, reqRect ) );
            OfxRectI bounds = dst->getBounds();
            if( !dst.get( ) )
                throw( tuttle::plugin::ImageNotReadyException( ) );
            // Build destination view
            this->_dstView = interleaved_view( std::abs(bounds.x2 - bounds.x1), std::abs(bounds.y2 - bounds.y1),
                                               static_cast<value_t*>(dst->getPixelData()),
                                               dst->getRowBytes() );

            // Set the render window
            this->setRenderWindow( args.renderWindow );
            // Call the base class process member
            this->process();
        } else
            throw( tuttle::plugin::PluginException( "Unable to open : %s", sFilepath.c_str() ) );
    }
    catch( tuttle::plugin::PluginException e )
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
void DPXReaderProcess<View>::multiThreadProcessImages( OfxRectI procWindow )
{
    try
    {
        std::string filepath;
        this->_filepath->getValue( filepath );
        readImage( this->_dstView, filepath );
    }
    catch( tuttle::plugin::PluginException err )
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
template<class View>
View& DPXReaderProcess<View>::readImage( View &dst, std::string & filepath ) throw(tuttle::plugin::PluginException )
{
    using namespace std;
    using namespace boost;
    using namespace mpl;
    using namespace boost::gil;

    switch (_plugin.getDpxImg().getComponentsType() )
    {
        case tuttle::io::DpxImage::eCompTypeR8G8B8:
        {
            rgba8_view_t src = interleaved_view( _plugin.getDpxImg().width(), _plugin.getDpxImg().height(),
                                                 ( rgba8_pixel_t* )( _plugin.getDpxImg().getData() ),
                                                 _plugin.getDpxImg().width() * sizeof( boost::uint32_t ) );

            copy_and_convert_pixels( clamp<rgb8_pixel_t>(flipped_up_down_view(src)), dst );
            break;
        }
        case tuttle::io::DpxImage::eCompTypeR8G8B8A8:
        {
            rgba8_view_t src = interleaved_view( _plugin.getDpxImg().width(), _plugin.getDpxImg().height(),
                                                 ( rgba8_pixel_t* )( _plugin.getDpxImg().getData() ),
                                                 _plugin.getDpxImg().width() * sizeof( boost::uint32_t ) );

            copy_and_convert_pixels( clamp<rgb8_pixel_t>(flipped_up_down_view(src)), dst );
            break;
        }
        case tuttle::io::DpxImage::eCompTypeA8B8G8R8:
        {
            abgr8_view_t src = interleaved_view( _plugin.getDpxImg().width(), _plugin.getDpxImg().height(),
                                                 ( abgr8_pixel_t* )( _plugin.getDpxImg().getData() ),
                                                 _plugin.getDpxImg().width() * sizeof( boost::uint32_t ) );

            copy_and_convert_pixels( clamp<rgb8_pixel_t>(flipped_up_down_view(src)), dst );
            break;
        }
        case tuttle::io::DpxImage::eCompTypeR10G10B10A10:
        {
            // Mutable reference to a RGBA10101010 pixel
            typedef const packed_channel_reference<uint40_t, 0,  10, true> rgba10101010_channel0_t;
            typedef const packed_channel_reference<uint40_t, 10, 10, true> rgba10101010_channel1_t;
            typedef const packed_channel_reference<uint40_t, 20, 10, true> rgba10101010_channel2_t;
            typedef const packed_channel_reference<uint40_t, 30, 10, true> rgba10101010_channel3_t;
            typedef vector4<rgba10101010_channel0_t, rgba10101010_channel1_t, rgba10101010_channel2_t, rgba10101010_channel3_t> rgba101010_channels_t;
            typedef packed_pixel<uint40_t, rgba101010_channels_t, rgba_layout_t> rgba10101010_pixel_t;
            // RGBA10101010 pixel value. It is a packed_pixel of size 4 bytes.
            typedef typename view_type_from_pixel<rgba10101010_pixel_t>::type rgba10101010_view_t;

            rgba10101010_view_t src = interleaved_view( _plugin.getDpxImg().width(), _plugin.getDpxImg().height(),
                                                       ( rgba10101010_pixel_t* )( _plugin.getDpxImg().getData() ),
                                                       _plugin.getDpxImg().width() );

            copy_and_convert_pixels( flipped_up_down_view( src ), color_converted_view<rgba16_pixel_t>( dst ) );
            break;
        }
        case tuttle::io::DpxImage::eCompTypeR10G10B10:
        {
            typedef const packed_channel_reference<bits32, 2,  10, true> rgb1010102_channel0_t;
            typedef const packed_channel_reference<bits32, 12, 10, true> rgb1010102_channel1_t;
            typedef const packed_channel_reference<bits32, 22, 10, true> rgb1010102_channel2_t;
            typedef vector3<rgb1010102_channel0_t, rgb1010102_channel1_t, rgb1010102_channel2_t> rgb101010_channels_t;
            typedef packed_pixel<bits32, rgb101010_channels_t, rgb_layout_t> rgb101010_pixel_t;
            typedef view_type_from_pixel<rgb101010_pixel_t>::type rgb101010_view_t;

            rgb101010_view_t src = interleaved_view( _plugin.getDpxImg().width(), _plugin.getDpxImg().height(),
                                                     ( rgb101010_pixel_t* )( _plugin.getDpxImg().getData() ),
                                                     _plugin.getDpxImg().width() * sizeof(boost::uint32_t) );

            copy_and_convert_pixels( color_converted_view<rgb16_pixel_t>(flipped_up_down_view(src)), dst );
            break;
        }
        case tuttle::io::DpxImage::eCompTypeA10B10G10R10:
        {
            /*
            typedef const packed_channel_reference<bits16, 0,  10, true> abgr10101010_channel0_t;
            typedef const packed_channel_reference<bits16, 10, 10, true> abgr10101010_channel1_t;
            typedef const packed_channel_reference<bits16, 20, 10, true> abgr10101010_channel2_t;
            typedef const packed_channel_reference<bits16, 30, 10, true> abgr10101010_channel3_t;

            typedef vector4<abgr10101010_channel0_t, abgr10101010_channel1_t, abgr10101010_channel2_t, abgr10101010_channel3_t> abgr10101010_channels_t;
            typedef packed_pixel<bits16, abgr10101010_channels_t, abgr_layout_t> abgr10101010_pixel_t;
            typedef view_type_from_pixel<abgr10101010_pixel_t>::type abgr10101010_view_t;

            abgr10101010_view_t src = interleaved_view( _plugin.getDpxImg().width(), _plugin.getDpxImg().height(),
                                                        ( abgr10101010_pixel_t* )( _plugin.getDpxImg().getData() ),
                                                        _plugin.getDpxImg().width() * sizeof(boost::uint32_t) );

            copy_and_convert_pixels( color_converted_view<rgb16_pixel_t>(flipped_up_down_view(src)), dst );
            */
            break;
        }
        default:
            break;
    }

    return dst;
}

}
}
}
