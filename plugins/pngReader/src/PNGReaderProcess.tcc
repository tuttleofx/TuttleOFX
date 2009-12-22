#include <tuttle/common/image/gilGlobals.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/Progress.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <cassert>
#include <cmath>
#include <vector>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/scoped_ptr.hpp>
#include <boost/gil/gil_all.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/gil/extension/dynamic_image/dynamic_image_all.hpp>
#include <boost/gil/extension/io/png_io.hpp>
#include <boost/gil/extension/io/png_dynamic_io.hpp>

namespace tuttle {
namespace plugin {
namespace png {

using namespace boost::gil;
namespace bfs = boost::filesystem;

typedef any_image < boost::mpl::vector
                    < rgba8_image_t, rgba16_image_t, rgba32f_image_t,
                      rgb8_image_t,  rgb16_image_t,  rgb32f_image_t >
                    > any_image_t;

template<class View>
PNGReaderProcess<View>::PNGReaderProcess( PNGReaderPlugin& instance )
	: tuttle::plugin::ImageGilProcessor<View>( instance ),
	tuttle::plugin::Progress( instance ),
	_plugin( instance )
{
	_filepath = instance.fetchStringParam( "Input filename" );
	assert( _filepath != NULL );
}

template<class View>
void PNGReaderProcess<View>::setupAndProcess( const OFX::RenderArguments& args )
{
	try
	{
		std::string sFilepath;
		// Fetch output image
		_filepath->getValue( sFilepath );
		if( bfs::exists( sFilepath ) )
		{
			point2<ptrdiff_t> pngDims = png_read_dimensions( sFilepath );
			double par                = _plugin.getDstClip()->getPixelAspectRatio();
			OfxRectD reqRect          = { 0, 0, pngDims.x * par, pngDims.y };
			boost::scoped_ptr<OFX::Image> dst( _plugin.getDstClip()->fetchImage( args.time, reqRect ) );
			OfxRectI bounds = dst->getBounds();
			if( !dst.get() )
				throw( ImageNotReadyException() );
			// Build destination view
			this->_dstView = interleaved_view( std::abs( bounds.x2 - bounds.x1 ), std::abs( bounds.y2 - bounds.y1 ),
			                                   static_cast<value_t*>( dst->getPixelData() ),
			                                   dst->getRowBytes() );

			// Set the render window
			this->setRenderWindow( args.renderWindow );
			// Call the base class process member
			this->process();
		}
		else
			throw PluginException( "Unable to open : " + sFilepath );
	}
	catch( PluginException& e )
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
void PNGReaderProcess<View>::multiThreadProcessImages( OfxRectI procWindow )
{
	try
	{
		std::string filepath;
		this->_filepath->getValue( filepath );
		readImage( this->_dstView, filepath );
	}
	catch( PluginException& e )
	{
		COUT_EXCEPTION( e );
	}
}

/*
 * struct FunctorPlus {
 *  template <typename DstV, typename SrcAV, typename SrcBV>
 *  static DstV merge()(const SrcAV & srcA, const SrcBV & srcB) const {
 *      return (DstV)(srcA + srcB);
 *  }
 * };
 *
 */
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
View& PNGReaderProcess<View>::readImage( View& dst, std::string& filepath ) throw( PluginException )
{
	any_image_t anyImg;

	png_read_image( filepath, anyImg );
	copy_and_convert_pixels( subimage_view( flipped_up_down_view( view( anyImg ) ), 0, 0, dst.width(), dst.height() ), dst );
	return dst;
}

}
}
}
