#ifndef _TUTTLE_PLUGIN_AV_WRITER_PROCESS_HPP_
#define _TUTTLE_PLUGIN_AV_WRITER_PROCESS_HPP_

#include "AVWriterPlugin.hpp"

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <terry/globals.hpp>

#include <AvTranscoder/decoder/VideoGenerator.hpp>
#include <AvTranscoder/data/decoded/VideoFrame.hpp>

namespace tuttle {
namespace plugin {
namespace av {
namespace writer {

/**
 * @brief FFMpeg process
 *
 */
template<class View>
class AVWriterProcess : public ImageGilFilterProcessor<View>
{
protected:
	AVWriterPlugin& _plugin; ///< Rendering plugin
	AVProcessParams _params;

	avtranscoder::VideoGenerator& _videoStream; ///< The output video stream (has link, no ownership)
	avtranscoder::VideoFrame _videoFrame; ///< The video frame with the encoded data

public:
	AVWriterProcess( AVWriterPlugin& instance );

	void setup( const OFX::RenderArguments& args );
	void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}
}

#include "AVWriterProcess.tcc"

#endif
