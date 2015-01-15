#ifndef _TUTTLE_PLUGIN_COLORMATRIX_PROCESS_HPP_
#define _TUTTLE_PLUGIN_COLORMATRIX_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>

namespace tuttle {
namespace plugin {
namespace colorMatrix {

/**
 * @brief ColorMatrix process
 *
 */
template<class View>
class ColorMatrixProcess : public ImageGilFilterProcessor<View>
{
public:
	typedef typename View::value_type Pixel;
	typedef typename boost::gil::channel_type<View>::type Channel;

protected:
    ColorMatrixPlugin&       _plugin; ///< Rendering plugin
	ColorMatrixProcessParams _params; ///< parameters

public:
    ColorMatrixProcess( ColorMatrixPlugin& effect );

	void setup( const OFX::RenderArguments& args );

    void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}

#include "ColorMatrixProcess.tcc"

#endif
