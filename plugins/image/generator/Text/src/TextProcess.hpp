#ifndef _TUTTLE_PLUGIN_TEXT_PROCESS_HPP_
#define _TUTTLE_PLUGIN_TEXT_PROCESS_HPP_

#include <tuttle/plugin/ImageGilProcessor.hpp>

#include <terry/freetype/freegil.hpp>
#include <boost/gil/typedefs.hpp>

#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle
{
namespace plugin
{
namespace text
{

/**
 * @brief Text process
 *
 */
template <class View, class Functor>
class TextProcess : public ImageGilProcessor<View>
{
public:
    typedef typename View::value_type Pixel;
    typedef terry::rgb8_pixel_t text_pixel_t;
    struct glyph_t
    {
        char ch;
        FT_Face face;
    };

    /**
     * to be used with boost::ptr_container
     */
    struct make_glyph
    {
        FT_Face _face;

        make_glyph(FT_Face face)
            : _face(face)
        {
        }

        glyph_t* operator()(char ch)
        {
            glyph_t* u = new glyph_t();

            u->ch = ch;
            u->face = _face;
            return u;
        }
    };

protected:
    OFX::Clip* _clipSrc; ///< Source image clip
    boost::scoped_ptr<OFX::Image> _src;
    OfxRectI _srcPixelRod;
    View _srcView; ///< @brief source clip (filters have only one input)

    TextPlugin& _plugin; ///< Rendering plugin
    std::vector<FT_Glyph_Metrics> _metrics;
    std::vector<int> _kerning;
    boost::ptr_vector<glyph_t> _glyphs;
    View _dstViewForGlyphs;
    boost::gil::point2<int> _textCorner;
    boost::gil::point2<int> _textSize;
    Pixel _foregroundColor;
    TextProcessParams _params;
    std::string _text;

public:
    TextProcess(TextPlugin& instance);

    void setup(const OFX::RenderArguments& args);

    void multiThreadProcessImages(const OfxRectI& procWindowRoW);
};
}
}
}

#include "TextProcess.tcc"

#endif
