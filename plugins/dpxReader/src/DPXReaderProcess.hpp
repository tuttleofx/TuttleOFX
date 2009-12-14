#ifndef DPX_READER_PROCESS_HPP
#define DPX_READER_PROCESS_HPP

#include <tuttle/common/utils/global.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/Progress.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/filesystem/fstream.hpp>

namespace tuttle {
namespace plugin {
namespace dpx {

/**
 * @brief Base class for the denoising processor
 *
 */
template<class View>
class DPXReaderProcess : public tuttle::plugin::ImageGilProcessor<View>,
	public tuttle::plugin::Progress
{
protected:
	typedef typename View::value_type value_t;
	OFX::StringParam*   _filepath;      ///< File path
	DPXReaderPlugin&    _plugin;        ///< Rendering plugin

	template<class T, class DST_V>
	void bitStreamToView(DST_V & dst, const int nc, const int channelSize);

public:
	DPXReaderProcess<View>( DPXReaderPlugin & instance );

	// set up and run a processor
	void setupAndProcess( const OFX::RenderArguments& args );

	// Do some processing
	void multiThreadProcessImages( OfxRectI procWindow );

	// Read dpx image
	View& readImage( View& dst, std::string& filepath ) throw( tuttle::plugin::PluginException );
};

}
}
}

#include "DPXReaderProcess.tcc"

#endif
