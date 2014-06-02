#ifndef _TUTTLE_PLUGIN_AV_READER_PROCESS_HPP_
#define _TUTTLE_PLUGIN_AV_READER_PROCESS_HPP_

#include <AvTranscoder/DatasStructures/Image.hpp>

#include <terry/globals.hpp>

#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <boost/scoped_ptr.hpp>

namespace tuttle {
namespace plugin {
namespace av {
namespace reader {

/**
 * @brief Audio Video process
 *
 */
template<class View>
class AVReaderProcess : public ImageGilProcessor<View>
{
protected:
	AVReaderPlugin& _plugin;

public:
	AVReaderProcess( AVReaderPlugin& instance );

	void setup( const OFX::RenderArguments& args );
	void multiThreadProcessImages( const OfxRectI& procWindowRoW );
	
	template<typename FileView>
	View& readImage( View& dst, avtranscoder::Image& image );
	
private:
	boost::scoped_ptr<avtranscoder::Image> _imageToEncode;
};

}
}
}
}

#include "AVReaderProcess.tcc"

#endif
