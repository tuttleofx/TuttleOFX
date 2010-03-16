#ifndef _TUTTLE_PLUGIN_TEXT_PROCESS_HPP_
#define _TUTTLE_PLUGIN_TEXT_PROCESS_HPP_

#include <tuttle/common/image/gilGlobals.hpp>
#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/typedefs.hpp>
#include "boost_gil_extension_freetype/freegil.hpp"

#include <boost/scoped_ptr.hpp>

#include <cstdlib>
#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>


namespace boost {
namespace gil {

typedef rgb8_pixel_t text_pixel_t;

struct glyph_t
{
	char ch;
	FT_Face face;
	text_pixel_t color;
};

}
}

namespace tuttle {
namespace plugin {
namespace text {

/**
 * @brief Text process
 *
 */
template<class View>
class TextProcess : public ImageGilFilterProcessor<View>
{
protected :
    TextPlugin&    _plugin;        ///< Rendering plugin
	std::vector<FT_Glyph_Metrics> _metrics;
	std::vector<int> _kerning;
	std::vector<boost::shared_ptr<bgil::glyph_t> > _glyphs;
	
public:
    TextProcess( TextPlugin& instance );

	void setup( const OFX::RenderArguments& args );

    // Do some processing
    void multiThreadProcessImages( const OfxRectI& procWindow );
};

}
}
}

#include "TextProcess.tcc"

#endif
