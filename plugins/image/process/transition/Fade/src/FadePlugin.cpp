#include "FadePlugin.hpp"
#include "FadeProcess.hpp"
#include "FadeDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle
{
namespace plugin
{
namespace fade
{

FadePlugin::FadePlugin(OfxImageEffectHandle handle)
    : OFX::ImageEffect(handle)
{
    _clipSrcFrom = fetchClip(kOfxImageEffectTransitionSourceFromClipName);
    _clipSrcTo = fetchClip(kOfxImageEffectTransitionSourceToClipName);
    _clipDst = fetchClip(kOfxImageEffectOutputClipName);

    _paramTransition = fetchDoubleParam(kOfxImageEffectTransitionParamName);
    _paramRod = fetchChoiceParam(kParamRod);
    _paramColor = fetchRGBAParam(kParamColor);
}

FadeProcessParams FadePlugin::getProcessParams() const
{
    FadeProcessParams params;
    params._transition = _paramTransition->getValue();
    params._rod = static_cast<EParamRod>(_paramRod->getValue());
    params._color = ofxToGil(_paramColor->getValue());
    return params;
}

void FadePlugin::changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName)
{
    //    if( paramName == kParamHelpButton )
    //    {
    //        sendMessage( OFX::Message::eMessageMessage,
    //                     "", // No XML resources
    //                     kParamHelpString );
    //    }
}

// bool FadePlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
//{
//	FadeProcessParams<Scalar> params = getProcessParams();
//	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
//
//	switch( params._border )
//	{
//		case eParamBorderPadded:
//			rod.x1 = srcRod.x1 + 1;
//			rod.y1 = srcRod.y1 + 1;
//			rod.x2 = srcRod.x2 - 1;
//			rod.y2 = srcRod.y2 - 1;
//			return true;
//		default:
//			break;
//	}
//	return false;
//}

bool FadePlugin::isIdentity(const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime)
{
    FadeProcessParams params = getProcessParams();
    if(params._transition == 0.0 && _clipSrcFrom->isConnected())
    {
        identityClip = _clipSrcFrom;
        identityTime = args.time;
        return true;
    }
    else if(params._transition == 1.0 && _clipSrcTo->isConnected())
    {
        identityClip = _clipSrcTo;
        identityTime = args.time;
        return true;
    }
    return false;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void FadePlugin::render(const OFX::RenderArguments& args)
{
    doGilRender<FadeProcess>(*this, args);
}
}
}
}
