/**
 * This plugin uses fftw3 which is GPL licensed, thus, this plugin (only) is GPL licensed.
 */

#ifndef _TUTTLE_PLUGIN_FFTTRANSFORM_PROCESS_HPP_
#define _TUTTLE_PLUGIN_FFTTRANSFORM_PROCESS_HPP_

#include <fftEngine/FftwWrapperCPU.hpp>
#ifdef with_cuda
#include <fftEngine/FftwWrapperGPU.hpp>
#endif

#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

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
namespace fft {

/**
 * @brief FftTransform process
 *
 */
template<class View>
class FftProcess : public ImageGilFilterProcessor<View>
{
protected:
	FftPlugin&    _plugin;        ///< Rendering plugin

public:
	FftProcess( FftPlugin& instance );

	// Do some processing
	void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}
}

#include "FftProcess.tcc"

#endif
