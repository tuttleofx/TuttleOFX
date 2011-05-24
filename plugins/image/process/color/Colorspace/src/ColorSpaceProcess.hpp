#ifndef _TUTTLE_COLORSPACE_PROCESS_HPP_
#define _TUTTLE_COLORSPACE_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>
#include <tuttle/plugin/image/gil/globals.hpp>

#include <tuttle/plugin/color/colorSpaceAPI.hpp>

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
namespace colorspace {

template<class View>
class ColorSpaceProcess : public ImageGilFilterProcessor<View>
{
protected:
	tuttle::plugin::color::ColorSpaceAPI csAPI;
	tuttle::plugin::color::EParamGradationLaw	_gradationIn;
	double						_GammaValueIn;
	double						_BlackPointIn;
	double						_WhitePointIn;
	double						_GammaSensitoIn;

	tuttle::plugin::color::EParamLayout		_layoutIn;
	tuttle::plugin::color::EParamLayout		_layoutOut;
	tuttle::plugin::color::EParamTemp		_tempColorIn;
	tuttle::plugin::color::EParamTemp		_tempColorOut;

	ColorSpacePlugin& _plugin; ///< Rendering plugin

public:
	ColorSpaceProcess( ColorSpacePlugin& instance );

	void setup( const OFX::RenderArguments& args );

	void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}

#include "ColorSpaceProcess.tcc"

#endif
