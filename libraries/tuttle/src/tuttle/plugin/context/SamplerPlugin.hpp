#ifndef _TUTTLE_PLUGIN_CONTEXT_SAMPLERPLUGIN_HPP_
#define _TUTTLE_PLUGIN_CONTEXT_SAMPLERPLUGIN_HPP_

#include "SamplerDefinition.hpp"
#include <tuttle/plugin/ImageEffectGilPlugin.hpp>
#include <tuttle/plugin/exceptions.hpp>

namespace tuttle
{
namespace plugin
{

struct SamplerProcessParams
{
    terry::sampler::EParamFilter _filter;
    double _filterSize;
    double _filterSigma;
    double _filterSharpen;
    double _paramB;
    double _paramC;

    terry::sampler::EParamFilterOutOfImage _outOfImageProcess;
};

class SamplerPlugin : public ImageEffectGilPlugin
{
public:
    SamplerPlugin(OfxImageEffectHandle handle);
    virtual ~SamplerPlugin() = 0;

public:
    virtual void changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName);

    virtual void fillProcessParams(SamplerProcessParams& samplerprocessParam) const;

    ::terry::sampler::EParamFilter getInterpolation() { return (terry::sampler::EParamFilter)_paramFilter->getValue(); }
public:
    OFX::ChoiceParam* _paramFilter;
    OFX::IntParam* _paramFilterSize;
    OFX::DoubleParam* _paramFilterSigma;
    OFX::DoubleParam* _paramFilterSharpen;
    OFX::DoubleParam* _paramB;
    OFX::DoubleParam* _paramC;
    OFX::ChoiceParam* _paramOutOfImage;
};
}
}

#endif
