#include "ChannelShufflePlugin.hpp"
#include "ChannelShuffleProcess.hpp"
#include "ChannelShuffleDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle
{
namespace plugin
{
namespace channelShuffle
{

ChannelShufflePlugin::ChannelShufflePlugin(OfxImageEffectHandle handle)
    : ImageEffectGilPlugin(handle)
{
    _paramRed = fetchChoiceParam(kParamRed);
    _paramGreen = fetchChoiceParam(kParamGreen);
    _paramBlue = fetchChoiceParam(kParamBlue);
    _paramAlpha = fetchChoiceParam(kParamAlpha);
}

ChannelShuffleProcessParams ChannelShufflePlugin::getProcessParams() const
{
    ChannelShuffleProcessParams params;
    params.mapping[0] = _paramRed->getValue();
    params.mapping[1] = _paramGreen->getValue();
    params.mapping[2] = _paramBlue->getValue();
    params.mapping[3] = _paramAlpha->getValue();
    return params;
}

void ChannelShufflePlugin::changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName)
{
    //    if( paramName == kParamHelpButton )
    //    {
    //        sendMessage( OFX::Message::eMessageMessage,
    //                     "", // No XML resources
    //                     kParamHelpString );
    //    }
}

// bool ChannelShufflePlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
//{
//	ChannelShuffleProcessParams<Scalar> params = getProcessParams();
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
//
// void ChannelShufflePlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter&
// rois )
//{
//	ChannelShuffleProcessParams<Scalar> params = getProcessParams();
//	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
//
//	OfxRectD srcRoi;
//	srcRoi.x1 = srcRod.x1 - 1;
//	srcRoi.y1 = srcRod.y1 - 1;
//	srcRoi.x2 = srcRod.x2 + 1;
//	srcRoi.y2 = srcRod.y2 + 1;
//	rois.setRegionOfInterest( *_clipSrc, srcRoi );
//}

bool ChannelShufflePlugin::isIdentity(const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime)
{
    //	ChannelShuffleProcessParams<Scalar> params = getProcessParams();
    //	if( params._in == params._out )
    //	{
    //		identityClip = _clipSrc;
    //		identityTime = args.time;
    //		return true;
    //	}
    return false;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void ChannelShufflePlugin::render(const OFX::RenderArguments& args)
{
    doGilRender<ChannelShuffleProcess>(*this, args);
}
}
}
}
