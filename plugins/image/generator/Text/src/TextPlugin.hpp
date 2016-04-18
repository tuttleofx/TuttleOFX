#ifndef _TUTTLE_PLUGIN_TEXT_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_TEXT_PLUGIN_HPP_

#include <tuttle/plugin/context/GeneratorPlugin.hpp>
#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

#include "TextDefinitions.hpp"

namespace tuttle
{
namespace plugin
{
namespace text
{

struct TextProcessParams
{
    std::string _text;

    std::string _fontPath;
    int _font;

    int _fontX;
    int _fontY;
    OfxRGBAColourD _fontColor;
    OfxRGBAColourD _backgroundColor;
    boost::gil::point2<double> _position;
    double _letterSpacing;
    EParamVAlign _vAlign;
    EParamHAlign _hAlign;
    bool _verticalFlip;
    bool _italic;
    bool _bold;
};

/**
 * @brief Text plugin
 */
class TextPlugin : public GeneratorPlugin
{
public:
    TextPlugin(OfxImageEffectHandle handle);

public:
    TextProcessParams getProcessParams(const OfxPointD& renderScale = OFX::kNoRenderScale) const;
    void getClipPreferences(OFX::ClipPreferencesSetter& clipPreferences);

    void render(const OFX::RenderArguments& args);

private:
    template <class View>
    void render(const OFX::RenderArguments& args);
    template <class View, template <typename> class Functor>
    void render(const OFX::RenderArguments& args);

    template <class View, template <typename> class Functor>
    void render_if(const OFX::RenderArguments& args, boost::mpl::false_);
    template <class View, template <typename> class Functor>
    void render_if(const OFX::RenderArguments& args, boost::mpl::true_);

public:
    OFX::StringParam* _paramText; ///< the text to rasterize in the image

    OFX::StringParam* _paramFontPath;
#ifndef __WINDOWS__
    OFX::ChoiceParam* _paramFont;
#endif
    OFX::IntParam* _paramSize;
    OFX::DoubleParam* _paramRatio;
    OFX::RGBAParam* _paramColor;
    OFX::RGBAParam* _paramBackgroundColor;
    OFX::Double2DParam* _paramPosition;
    OFX::DoubleParam* _paramLetterSpacing;
    OFX::ChoiceParam* _paramVAlign;
    OFX::ChoiceParam* _paramHAlign;
    OFX::BooleanParam* _paramVerticalFlip;
    OFX::BooleanParam* _paramItalic;
    OFX::BooleanParam* _paramBold;

    OFX::ChoiceParam* _paramMerge;
};
}
}
}

#endif
