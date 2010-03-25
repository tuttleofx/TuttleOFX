/**
 * This plugin uses fftw3 which is GPL licensed, thus, this plugin (only) is GPL licensed.
 */

#ifndef _TUTTLE_PLUGIN_IFFT_PROCESS_HPP_
#define _TUTTLE_PLUGIN_IFFT_PROCESS_HPP_

#include <fftEngine/FftwWrapperCPU.hpp>
#include <fftEngine/FftwWrapperGPU.hpp>

#include <tuttle/common/image/gilGlobals.hpp>
#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/scoped_ptr.hpp>

#include <cstdlib>
#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>


namespace tuttle {
namespace plugin {
namespace fftTransform {
namespace ifft {

/**
 * @brief FftTransform process
 *
 */
template<class View>
class IfftProcess : public ImageGilProcessor<View>
{
protected :
    IfftPlugin&    _plugin;               ///< @brief Rendering plugin

	OFX::Clip     *_srcClipRe;            ///< @brief Real source image clip
	View _srcViewRe;                      ///< @brief image to get real part from
	boost::scoped_ptr<OFX::Image> _srcMod; ///< @brief src view Re

	OFX::Clip     *_srcClipIm;            ///< @brief Imaginary source image clip
	View _srcViewIm;                      ///< @brief image to get imaginary part from
	boost::scoped_ptr<OFX::Image> _srcPhase; ///< @brief src view Im

    OFX::Clip           *_dstClip;       ///< Destination image clip

public:
    IfftProcess( IfftPlugin& instance );
	void setup( const OFX::RenderArguments& args );
    // Do some processing
    void multiThreadProcessImages( const OfxRectI& procWindow );
};

}
}
}
}

#include "IfftProcess.tcc"

#endif
