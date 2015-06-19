#ifndef _TUTTLE_PLUGIN_IMAGEGILPROCESSOR_HPP_
#define _TUTTLE_PLUGIN_IMAGEGILPROCESSOR_HPP_

#include "ImageProcessor.hpp"

#include <terry/globals.hpp>
#include <tuttle/plugin/ofxToGil/image.hpp>

namespace tuttle {
namespace plugin {

/**
 * @brief Base class that can be used to process images of any type using boost::gil library view to access images.
 */
template <class View>
class ImageGilProcessor : public ImageProcessor
{
public:
	typedef typename View::value_type Pixel;
	typedef typename terry::image_from_view<View>::type Image;

protected:
	View _dstView; ///< image to process into

public:
	ImageGilProcessor( OFX::ImageEffect& effect, const EImageOrientation imageOrientation )
		: ImageProcessor( effect, imageOrientation )
	{}
	virtual ~ImageGilProcessor() {}

	virtual void setup( const OFX::RenderArguments& args )
	{
		ImageProcessor::setup( args );
		_dstView = getView( _dst.get(), _dstPixelRod );

		#if(TUTTLE_INIT_IMAGE_BUFFERS)
		const OfxRectI dstRenderWin = this->translateRoWToOutputClipCoordinates( args.renderWindow );
		View dstToFill = boost::gil::subimage_view( _dstView,
			dstRenderWin.x1, dstRenderWin.y1,
			dstRenderWin.x2 - dstRenderWin.x1, dstRenderWin.y2 - dstRenderWin.y1 );
		const boost::gil::rgba32f_pixel_t errorColor( 1.0, 0.0, 0.0, 1.0 );
		fill_pixels( dstToFill, errorColor );
		#endif
	}

	/**
	 * @brief Return a full gil view of an image.
	 */
	View getView( OFX::Image* img, const OfxRectI& pixelRod ) const
	{
		return tuttle::plugin::getGilView<View>( img, pixelRod, _imageOrientation );
	}
	template<typename CustomView>
	CustomView getCustomView( OFX::Image* img, const OfxRectI& pixelRod ) const
	{
		return tuttle::plugin::getGilView<CustomView>( img, pixelRod, _imageOrientation );
	}
};

}
}

#endif

