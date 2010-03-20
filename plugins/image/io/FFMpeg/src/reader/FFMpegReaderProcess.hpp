#ifndef _TUTTLE_PLUGIN_FFMPEG_READER_PROCESS_HPP_
#define _TUTTLE_PLUGIN_FFMPEG_READER_PROCESS_HPP_

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
namespace ffmpeg {
namespace reader {

/**
 * @brief FFMpeg process
 *
 */
template<class View>
class FFMpegReaderProcess : public ImageGilFilterProcessor<View>
{
protected :
    FFMpegReaderPlugin&	_plugin;		///< Rendering plugin

public:
    FFMpegReaderProcess( FFMpegReaderPlugin& instance );

	void setup( const OFX::RenderArguments& args );
    // Do some processing
    void multiThreadProcessImages( const OfxRectI& procWindow );
};

}
}
}
}

#include "FFMpegReaderProcess.tcc"

#endif
