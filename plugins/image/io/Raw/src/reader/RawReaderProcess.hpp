#ifndef _TUTTLE_PLUGIN_RAWREADERPROCESS_HPP_
#define _TUTTLE_PLUGIN_RAWREADERPROCESS_HPP_

#include <tuttle/plugin/ImageGilProcessor.hpp>

#include <libraw/libraw.h>

namespace tuttle {
namespace plugin {
namespace raw {
namespace reader {

/**
 *
 */
template<class View>
class RawReaderProcess : public ImageGilProcessor<View>
{

public:
	RawReaderProcess( RawReaderPlugin& instance );

	void setup( const OFX::RenderArguments& args );
	void multiThreadProcessImages( const OfxRectI& procWindowRoW );

	View& readImage( View& dst, const std::string& filepath );

private:
	RawReaderPlugin&    _plugin;        ///< Rendering plugin
	RawReaderProcessParams _params;

	LibRaw _rawProcessor;
	libraw_iparams_t& _p1;
	libraw_image_sizes_t& _size;
	libraw_colordata_t& _color;
	libraw_thumbnail_t& _thumbnail;
	libraw_imgother_t& _p2;
	libraw_output_params_t& _out;

};

}
}
}
}

#include "RawReaderProcess.tcc"

#endif
