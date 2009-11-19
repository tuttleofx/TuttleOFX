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

#include <boost/gil/extension/io/png_io.hpp>
#include <boost/gil/extension/io/png_dynamic_io.hpp>
#include <boost/cstdint.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/filesystem/fstream.hpp>


using namespace boost::gil;
namespace bfs = boost::filesystem;

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
    // Mutable reference to a RGBA1010102 pixel
    typedef bit_aligned_pixel_reference<uint32_t, mpl::vector4_c<unsigned, 2, 10, 10, 10>, abgr_layout_t, true>  rgba1010102_ref_t;
    // A mutable iterator over RGBA1010102 pixels
    typedef bit_aligned_pixel_iterator<rgba1010102_ref_t> rgba1010102_ptr_t;
    // RGBA1010102 pixel value. It is a packed_pixel of size 4 bytes.
    typedef typename std::iterator_traits<rgba1010102_ptr_t>::value_type rgba1010102_pixel_t;
    typedef view_type_from_pixel<rgba1010102_pixel_t>::type rgba1010102_view_t;
    typedef bit_aligned_image4_type< 2, 10, 10, 10, abgr_layout_t >::type rgba1010102_image_t;

    rgba1010102_view_t src = interleaved_view( _plugin.getDpxImg().width(),  _plugin.getDpxImg().height(),
                                               (rgba1010102_pixel_t*)( _plugin.getDpxImg().getData() ),
                                               _plugin.getDpxImg().width() * sizeof(boost::uint32_t) );

    rgba16_image_t img(src.dimensions());
    rgba16_view_t viw(view(img));
    copy_and_convert_pixels(flipped_up_down_view(src), clamp<rgba16_pixel_t>(dst) );
    copy_and_convert_pixels(flipped_up_down_view(src), viw);

    png_write_view("c:/temp/out2.png", clamp<rgb16_pixel_t>(viw));

    return dst;
}

}
}
}
