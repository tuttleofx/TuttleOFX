/**
 * This plugin uses fftw3 which is GPL licensed, thus, this plugin (only) is GPL licensed.
 */

#ifndef _TUTTLE_PLUGIN_IFFT_PROCESS_HPP_
#define _TUTTLE_PLUGIN_IFFT_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <fftEngine/FftwWrapperCPU.hpp>
#include <fftEngine/FftwWrapperGPU.hpp>

#include <boost/scoped_ptr.hpp>

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
protected:
	IfftPlugin&    _plugin;               ///< @brief Rendering plugin

	OFX::Clip* _clipSrcRe;                ///< @brief Real source image clip
	View _srcViewRe;                      ///< @brief image to get real part from
	boost::scoped_ptr<OFX::Image> _srcMod; ///< @brief src view Re

	OFX::Clip* _clipSrcIm;                ///< @brief Imaginary source image clip
	View _srcViewIm;                      ///< @brief image to get imaginary part from
	boost::scoped_ptr<OFX::Image> _srcPhase; ///< @brief src view Im

	OFX::Clip* _clipDst;                 ///< Destination image clip

public:
	IfftProcess( IfftPlugin& instance );

	void setup( const OFX::RenderArguments& args );

	void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}
}

#include "IfftProcess.tcc"

#endif
