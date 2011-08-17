#ifndef _TUTTLE_PLUGIN_FFMPEG_WRITER_PROCESS_HPP_
#define _TUTTLE_PLUGIN_FFMPEG_WRITER_PROCESS_HPP_

#include <ffmpeg/VideoFFmpegWriter.hpp>

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <terry/globals.hpp>

#include <boost/scoped_ptr.hpp>

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
