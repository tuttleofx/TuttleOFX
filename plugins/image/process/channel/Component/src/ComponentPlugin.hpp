#ifndef _TUTTLE_PLUGIN_COMPONENT_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_COMPONENT_PLUGIN_HPP_

#include "ComponentDefinitions.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle
{
namespace plugin
{
namespace component
{

/**
 * @brief Component plugin
 */
class ComponentPlugin : public ImageEffectGilPlugin
{
public:
    typedef float Scalar;

public:
    ComponentPlugin(OfxImageEffectHandle handle);

public:
    void changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName);

    //	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
    //	void getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );
    bool isIdentity(const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime);
    void getClipPreferences(OFX::ClipPreferencesSetter& clipPreferences);
    /*
    template< typename dstComponents >
    void renderWithDstComponent( const OFX::RenderArguments &args, OFX::EPixelComponent srcComponents, OFX::EBitDepth
    bitDepth );
    */
    void render(const OFX::RenderArguments& args);

public:
    OFX::ChoiceParam* _paramToComponent;
    OFX::ChoiceParam* _paramToGrayMethod;
    OFX::BooleanParam* _paramPremultiplied;
};
}
}
}

#endif
