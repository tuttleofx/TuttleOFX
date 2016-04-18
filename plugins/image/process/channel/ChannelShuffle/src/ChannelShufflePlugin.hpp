#ifndef _TUTTLE_PLUGIN_CHANNELSHUFFLE_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_CHANNELSHUFFLE_PLUGIN_HPP_

#include "ChannelShuffleDefinitions.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle
{
namespace plugin
{
namespace channelShuffle
{

struct ChannelShuffleProcessParams
{
    boost::array<int, 4> mapping;
};

/**
 * @brief ChannelShuffle plugin
 */
class ChannelShufflePlugin : public ImageEffectGilPlugin
{
public:
    typedef float Scalar;

public:
    ChannelShufflePlugin(OfxImageEffectHandle handle);

public:
    ChannelShuffleProcessParams getProcessParams() const;

    void changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName);

    //	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
    //	void getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );
    bool isIdentity(const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime);

    void render(const OFX::RenderArguments& args);

public:
    OFX::ChoiceParam* _paramRed;
    OFX::ChoiceParam* _paramGreen;
    OFX::ChoiceParam* _paramBlue;
    OFX::ChoiceParam* _paramAlpha;
};
}
}
}

#endif
