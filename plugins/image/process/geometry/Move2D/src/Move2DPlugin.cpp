#include "Move2DPlugin.hpp"
#include "Move2DProcess.hpp"
#include "Move2DDefinitions.hpp"

#include <boost/gil/gil_all.hpp>
#include <boost/gil/utilities.hpp>

namespace tuttle
{
namespace plugin
{
namespace move2D
{

Move2DPlugin::Move2DPlugin(OfxImageEffectHandle handle)
    : ImageEffectGilPlugin(handle)
{
    //    _clipSrcMatte = fetchClip( kClipMatte );
    _paramTranslation = fetchDouble2DParam(kParamTranslation);
}

Move2DProcessParams<Move2DPlugin::Scalar> Move2DPlugin::getProcessParams(const OfxPointD& renderScale) const
{
    Move2DProcessParams<Scalar> params;

    OfxPointD translation = _paramTranslation->getValue();
    params._translation.x = translation.x;
    params._translation.y = translation.y;

    return params;
}

void Move2DPlugin::changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName)
{
}

bool Move2DPlugin::getRegionOfDefinition(const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod)
{
    const Move2DProcessParams<Scalar> params = getProcessParams();
    OfxRectD srcRod = _clipSrc->getCanonicalRod(args.time);

    //	switch( params._border )
    //	{
    //		case eParamBorderPadded:
    rod.x1 = srcRod.x1 + params._translation.x;
    rod.y1 = srcRod.y1 + params._translation.y;
    rod.x2 = srcRod.x2 + params._translation.x;
    rod.y2 = srcRod.y2 + params._translation.y;
    return true;
    //		default:
    //			break;
    //	}
    //	return false;
}

void Move2DPlugin::getRegionsOfInterest(const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois)
{
    const Move2DProcessParams<Scalar> params = getProcessParams();
    OfxRectD srcRoi;

    //	switch( params._border )
    //	{
    //		case eParamBorderPadded:
    srcRoi.x1 = args.regionOfInterest.x1 - params._translation.x;
    srcRoi.y1 = args.regionOfInterest.y1 - params._translation.y;
    srcRoi.x2 = args.regionOfInterest.x2 - params._translation.x;
    srcRoi.y2 = args.regionOfInterest.y2 - params._translation.y;
    //		default:
    //			break;
    //	}

    rois.setRegionOfInterest(*this->_clipSrc, srcRoi);
}

bool Move2DPlugin::isIdentity(const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime)
{
    const Move2DProcessParams<Scalar> params = getProcessParams();

    static const boost::gil::point2<Scalar> zero(0, 0);
    if(params._translation == zero)
    {
        identityClip = this->_clipSrc;
        identityTime = args.time;
        return true;
    }
    return false;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void Move2DPlugin::render(const OFX::RenderArguments& args)
{
    doGilRender<Move2DProcess>(*this, args);
}
}
}
}
