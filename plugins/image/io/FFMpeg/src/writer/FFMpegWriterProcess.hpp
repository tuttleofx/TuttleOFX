#ifndef _TUTTLE_PLUGIN_FFMPEG_WRITER_PROCESS_HPP_
#define _TUTTLE_PLUGIN_FFMPEG_WRITER_PROCESS_HPP_

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

#include <ffmpeg/VideoFFmpegWriter.hpp>

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
protected:
	FFMpegWriterPlugin& _plugin;        ///< Rendering plugin
	FFMpegProcessParams _params;

public:
	FFMpegWriterProcess( FFMpegWriterPlugin& instance );

	void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}
}

#include "FFMpegWriterProcess.tcc"

#endif
