#ifndef _TUTTLE_PLUGIN_PRINT_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_PRINT_PLUGIN_HPP_

#include "PrintDefinitions.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle
{
namespace plugin
{
namespace print
{

template <typename Scalar>
struct PrintProcessParams
{
    EParamMode _mode;
    EParamOutput _output;
    EParamColor _colorMode;
    boost::gil::point2<std::ptrdiff_t> _pixel;
    OfxRectI _region;
    bool _flip;
    bool _openGlViewer;
    int _cols;
};

/**
 * @brief Print plugin
 */
class PrintPlugin : public ImageEffectGilPlugin
{
public:
    typedef float Scalar;

public:
    PrintPlugin(OfxImageEffectHandle handle);

public:
    PrintProcessParams<Scalar> getProcessParams(const OfxPointD& renderScale = OFX::kNoRenderScale) const;

    void changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName);

    //	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
    //	void getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );
    bool isIdentity(const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime);

    void render(const OFX::RenderArguments& args);

public:
    OFX::ChoiceParam* _paramMode;
    OFX::ChoiceParam* _paramOutput;
    OFX::ChoiceParam* _paramColor;
    OFX::Int2DParam* _paramPixel;
    OFX::Int2DParam* _paramRegionMin;
    OFX::Int2DParam* _paramRegionMax;
    OFX::IntParam* _paramOutputColumns;
    OFX::BooleanParam* _paramFlip;
    OFX::BooleanParam* _paramOpenGlWindow;

private:
    void updateParams();
};
}
}
}

#endif
