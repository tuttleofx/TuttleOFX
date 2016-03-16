#include "FlipDefinitions.hpp"
#include "FlipPlugin.hpp"
#include "FlipProcess.hpp"

#include <tuttle/plugin/ofxToGil/point.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/math/special_functions/round.hpp>

namespace tuttle
{
namespace plugin
{
namespace flip
{

using namespace boost::math;
using namespace boost::gil;

FlipPlugin::FlipPlugin(OfxImageEffectHandle handle)
    : ImageEffectGilPlugin(handle)
{
    _paramFlip = fetchBooleanParam(kParamFlip);
    _paramFlop = fetchBooleanParam(kParamFlop);
}

OfxRectI FlipPlugin::computeFlipRegion(const OfxTime time, const bool fromRatio) const
{
    const OfxRectI sRod = _clipSrc->getPixelRod(time);
    return sRod;
}

FlipProcessParams FlipPlugin::getProcessParams(const OfxTime time, const OfxPointD& renderScale) const
{
    FlipProcessParams params;

    params.flip = _paramFlip->getValue();
    params.flop = _paramFlop->getValue();

    return params;
}

void FlipPlugin::getRegionsOfInterest(const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois)
{
    //	FlipProcessParams params = getProcessParams( args.time, args.renderScale );
    //	if( params.flip )
    //	{
    //		/// @todo needs to transform args.regionOfInterest, like in the compute function
    //	}
    //	if( params.flop )
    //	{
    //		/// @todo needs to transform args.regionOfInterest, like in the compute function
    //	}
    OfxRectD srcRod = _clipSrc->getCanonicalRod(args.time);
    rois.setRegionOfInterest(*this->_clipSrc, srcRod);
}

bool FlipPlugin::isIdentity(const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime)
{
    FlipProcessParams params = getProcessParams(args.time, args.renderScale);
    if(params.flip || params.flop)
        return false;

    identityClip = _clipSrc;
    identityTime = args.time;
    return true;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void FlipPlugin::render(const OFX::RenderArguments& args)
{
    doGilRender<FlipProcess>(*this, args);
}
}
}
}
