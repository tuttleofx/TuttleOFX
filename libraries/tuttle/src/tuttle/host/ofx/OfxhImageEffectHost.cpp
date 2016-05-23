#include "OfxhImageEffectHost.hpp"

#include "OfxhImageEffectSuite.hpp"
#include "OfxhParameterSuite.hpp"
#include "OfxhMessageSuite.hpp"
#include "OfxhInteractSuite.hpp"
#include "OfxhProgressSuite.hpp"
#include "OfxhTimelineSuite.hpp"
#include "OfxhMultiThreadSuite.hpp"

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace imageEffect
{

/// properties for the image effect host
static property::OfxhPropSpec hostStuffs[] = {
    {kOfxImageEffectHostPropIsBackground, property::ePropTypeInt, 1, true, "0"},
    {kOfxImageEffectPropSupportsOverlays, property::ePropTypeInt, 1, true, "1"},
    {kOfxImageEffectPropSupportsMultiResolution, property::ePropTypeInt, 1, true, "1"},
    {kOfxImageEffectPropSupportsTiles, property::ePropTypeInt, 1, true, "1"},
    {kOfxImageEffectPropTemporalClipAccess, property::ePropTypeInt, 1, true, "1"},

    /// xxx this needs defaulting manually
    {kOfxImageEffectPropSupportedComponents, property::ePropTypeString, 0, true, ""},
    /// xxx this needs defaulting manually

    {kOfxImageEffectPropSupportedPixelDepths, property::ePropTypeString, 0, true, ""},

    /// xxx this needs defaulting manually

    {kOfxImageEffectPropSupportedContexts, property::ePropTypeString, 0, true, ""},
    /// xxx this needs defaulting manually

    {kOfxImageEffectPropSupportsMultipleClipDepths, property::ePropTypeInt, 1, true, "1"},
    {kOfxImageEffectPropSupportsMultipleClipPARs, property::ePropTypeInt, 1, true, "0"},
    {kOfxImageEffectPropSetableFrameRate, property::ePropTypeInt, 1, true, "0"},
    {kOfxImageEffectPropSetableFielding, property::ePropTypeInt, 1, true, "0"},
    {kOfxParamHostPropSupportsCustomInteract, property::ePropTypeInt, 1, true, "0"},
    {kOfxParamHostPropSupportsStringAnimation, property::ePropTypeInt, 1, true, "0"},
    {kOfxParamHostPropSupportsChoiceAnimation, property::ePropTypeInt, 1, true, "0"},
    {kOfxParamHostPropSupportsBooleanAnimation, property::ePropTypeInt, 1, true, "0"},
    {kOfxParamHostPropSupportsCustomAnimation, property::ePropTypeInt, 1, true, "0"},
    {kOfxParamHostPropMaxParameters, property::ePropTypeInt, 1, true, "-1"},
    {kOfxParamHostPropMaxPages, property::ePropTypeInt, 1, true, "0"},
    {kOfxParamHostPropPageRowColumnCount, property::ePropTypeInt, 2, true, "0"},
    {0},
};

/// ctor

OfxhImageEffectHost::OfxhImageEffectHost()
{
    /// add the properties for an image effect host, derived classs to set most of them
    _properties.addProperties(hostStuffs);
}

OfxhImageEffectHost::~OfxhImageEffectHost()
{
}

/**
 * optionally over-ridden function to register the creation of a new descriptor in the host app
 */
void OfxhImageEffectHost::initDescriptor(OfxhImageEffectNodeDescriptor& desc) const
{
}

/**
 * Use this in any dialogue etc... showing progress
 */
void OfxhImageEffectHost::loadingStatus(const std::string&)
{
}

bool OfxhImageEffectHost::pluginSupported(const OfxhImageEffectPlugin& plugin, std::string& reason) const
{
    return true;
}

/**
 * our suite fetcher
 */
void* OfxhImageEffectHost::fetchSuite(const char* suiteName, const int suiteVersion)
{
    if(strcmp(suiteName, kOfxImageEffectSuite) == 0)
    {
        return getImageEffectSuite(suiteVersion);
    }
    else if(strcmp(suiteName, kOfxParameterSuite) == 0)
    {
        return attribute::getParameterSuite(suiteVersion);
    }
    else if(strcmp(suiteName, kOfxMessageSuite) == 0)
    {
        return getMessageSuite(suiteVersion);
    }
    else if(strcmp(suiteName, kOfxInteractSuite) == 0)
    {
        return interact::getInteractSuite(suiteVersion);
    }
    else if(strcmp(suiteName, kOfxProgressSuite) == 0)
    {
        return getProgressSuite(suiteVersion);
    }
    else if(strcmp(suiteName, kOfxTimeLineSuite) == 0)
    {
        return getTimelineSuite(suiteVersion);
    }
    else if(strcmp(suiteName, kOfxMultiThreadSuite) == 0)
    {
        return getMultithreadSuite(suiteVersion);
    }
    else /// otherwise just grab the base class one, which is props and memory
        return tuttle::host::ofx::OfxhHost::fetchSuite(suiteName, suiteVersion);
}
}
}
}
}
