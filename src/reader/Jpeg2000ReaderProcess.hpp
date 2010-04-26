#ifndef _TUTTLE_PLUGIN_JPEG2000_READER_PROCESS_HPP_
#define _TUTTLE_PLUGIN_JPEG2000_READER_PROCESS_HPP_

#include <tuttle/common/image/gilGlobals.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/PluginException.hpp>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/scoped_ptr.hpp>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>
#include <openjpeg/J2KReader.hpp>


namespace tuttle {
namespace plugin {
namespace jpeg2000 {
namespace reader {

/**
 * @brief FFMpeg process
 *
 */
template<class View>
class Jpeg2000ReaderProcess : public ImageGilProcessor<View>
{
protected :
    Jpeg2000ReaderPlugin&	_plugin;		///< Rendering plugin

public:
    Jpeg2000ReaderProcess( Jpeg2000ReaderPlugin& instance );
	~Jpeg2000ReaderProcess();
	void setup( const OFX::RenderArguments& args );

	// Do some processing
    void multiThreadProcessImages( const OfxRectI& procWindowRoW );

	template<class Layout>
	void switchLayoutCopy();

	template<class WorkingPixel>
	void switchPrecisionCopy(const View & wv);
};

}
}
}
}

#include "Jpeg2000ReaderProcess.tcc"

#endif
