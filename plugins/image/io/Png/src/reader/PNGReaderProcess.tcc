#include <tuttle/common/image/gilGlobals.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
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

#include "PNGReaderDefinitions.hpp"
#include "PNGReaderProcess.hpp"

namespace tuttle {
namespace plugin {
namespace png {
namespace reader {

using namespace boost::gil;
namespace bfs = boost::filesystem;

typedef any_image < boost::mpl::vector
                    < rgba8_image_t, rgba16_image_t, rgba32f_image_t,
                      rgb8_image_t,  rgb16_image_t,  rgb32f_image_t >
                    > any_image_t;

template<class View>
PNGReaderProcess<View>::PNGReaderProcess( PNGReaderPlugin& instance )
	: ImageGilProcessor<View>( instance ),
	_plugin( instance )
{
	assert( _plugin._filepath != NULL );
	this->setNoMultiThreading();
}

template<class View>
void PNGReaderProcess<View>::setup( const OFX::RenderArguments& args )
{
	std::string sFilepath = _plugin._filepath->getValue();
	if( ! bfs::exists( sFilepath ) )
		throw( PluginException( "Unable to open : " + sFilepath ) );
	point2<std::ptrdiff_t> pngDims = png_read_dimensions( sFilepath );
	double par                = _plugin.getDstClip()->getPixelAspectRatio();
	OfxRectD reqRect          = { 0, 0, pngDims.x * par, pngDims.y };

	// Fetch output image
	boost::scoped_ptr<OFX::Image> dst( _plugin.getDstClip()->fetchImage( args.time, reqRect ) );
	OfxRectI bounds = dst->getBounds();
	if( !dst.get() )
		throw( ImageNotReadyException() );

	COUT_VAR4(reqRect.x1,reqRect.y1,reqRect.x2,reqRect.y2);
	OfxRectD canonicalRod = _plugin.getDstClip()->getCanonicalRod(args.time);
	COUT_VAR4(canonicalRod.x1,canonicalRod.y1,canonicalRod.x2,canonicalRod.y2);
	OfxRectI pixelRod = _plugin.getDstClip()->getPixelRod(args.time);
	COUT_VAR4(pixelRod.x1,pixelRod.y1,pixelRod.x2,pixelRod.y2);
	
	// Build destination view
	this->_dstView = this->getView( dst.get(), _plugin.getDstClip()->getPixelRod(args.time) );
}

/**
 * @brief Function called by rendering thread each time
 *        a process must be done.
 *
 * @param[in] procWindow  Processing window
 */
template<class View>
void PNGReaderProcess<View>::multiThreadProcessImages( const OfxRectI& procWindow )
{
	readImage( this->_dstView, _plugin._filepath->getValue() );
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
View& PNGReaderProcess<View>::readImage( View& dst, const std::string& filepath ) throw( PluginException )
{
	any_image_t anyImg;
	try
	{
		png_read_image( filepath, anyImg );
	}
	catch( PluginException& e )
	{
		COUT_EXCEPTION( e );
		return dst;
	}
	copy_and_convert_pixels( subimage_view( flipped_up_down_view( view( anyImg ) ), 0, 0, dst.width(), dst.height() ), dst );
	return dst;
}

}
}
}
}
