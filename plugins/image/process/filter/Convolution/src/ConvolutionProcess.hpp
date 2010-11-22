#ifndef _TUTTLE_PLUGIN_CONVOLUTION_PROCESS_HPP_
#define _TUTTLE_PLUGIN_CONVOLUTION_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle {
namespace plugin {
namespace convolution {

/**
 * @brief Convolution process
 *
 */
template<class View>
class ConvolutionProcess : public ImageGilFilterProcessor<View>
{
public:
	typedef float Scalar;
	typedef typename View::value_type Pixel;
	typedef typename View::point_t Point;
	typedef typename View::coord_t Coord;
	typedef typename image_from_view<View>::type Image;

protected:
	ConvolutionPlugin&    _plugin;        ///< Rendering plugin
	ConvolutionProcessParams _params;

public:
	ConvolutionProcess( ConvolutionPlugin& instance );

	void setup( const OFX::RenderArguments& args );
	void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}

#include "ConvolutionProcess.tcc"

#endif
