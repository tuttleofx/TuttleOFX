#ifndef _TUTTLE_PLUGIN_FFMPEG_WRITER_PROCESS_HPP_
#define _TUTTLE_PLUGIN_FFMPEG_WRITER_PROCESS_HPP_

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
namespace writer {

/**
 * @brief FFMpeg process
 *
 */
template<class View>
class FFMpegWriterProcess : public ImageGilFilterProcessor<View>
{
protected :
    FFMpegWriterPlugin&	_plugin;		///< Rendering plugin
	VideoFFmpegWriter _writer;
	FFMpegProcessParams _params;
	
public:
    FFMpegWriterProcess( FFMpegWriterPlugin& instance );
	void preProcess();
	void postProcess();
    // Do some processing
    void multiThreadProcessImages( const OfxRectI& procWindow );
};

}
}
}
}

#include "FFMpegWriterProcess.tcc"

#endif
