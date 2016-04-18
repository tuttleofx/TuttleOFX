#include "ColorTransferPlugin.hpp"
#include "ColorTransferProcess.hpp"
#include "ColorTransferDefinitions.hpp"

#include <boost/gil/gil_all.hpp>
#include <terry/numeric/operations.hpp>
#include <terry/numeric/assign.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorTransfer
{

ColorTransferPlugin::ColorTransferPlugin(OfxImageEffectHandle handle)
    : ImageEffectGilPlugin(handle)
{
    _clipSrcRef = this->fetchClip(kClipSrcRef);
    _clipDstRef = this->fetchClip(kClipDstRef);

    _paramColospace = this->fetchChoiceParam(kParamColorspace);
    _paramAverageCoef = this->fetchDoubleParam(kParamAverageCoef);
    _paramDynamicCoef = this->fetchDoubleParam(kParamDynamicCoef);

    //	_paramRegionA = this->fetchDouble2DParam( kParamRegionA );
    //	_paramRegionB = this->fetchDouble2DParam( kParamRegionB );
    //	_paramSameRegion = this->fetchBooleanParam( kParamSameRegion );
    //	_paramInputRegionA = this->fetchDouble2DParam( kParamInputRegionA );
    //	_paramInputRegionB = this->fetchDouble2DParam( kParamInputRegionB );
    //	const OFX::InstanceChangedArgs args( this->timeLineGetTime() );
    //	changedParam( args, kParamSameRegion );
}

ColorTransferProcessParams<ColorTransferPlugin::Scalar>
ColorTransferPlugin::getProcessParams(const OfxPointD& renderScale) const
{
    ColorTransferProcessParams<Scalar> params;
    params._colorspace = static_cast<EColorspace>(_paramColospace->getValue());
    params._averageCoef = _paramAverageCoef->getValue();
    params._dynamicCoef = _paramDynamicCoef->getValue();
    return params;
}

void ColorTransferPlugin::changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName)
{
    //	if( paramName == kParamSameRegion )
    //	{
    //		const bool status = _paramSameRegion->getValue();
    //		_paramInputRegionA->setIsSecretAndDisabled( status );
    //		_paramInputRegionB->setIsSecretAndDisabled( status );
    //	}
}

void ColorTransferPlugin::getRegionsOfInterest(const OFX::RegionsOfInterestArguments& args,
                                               OFX::RegionOfInterestSetter& rois)
{
    if(_clipSrcRef->isConnected())
    {
        // set roi of source
        OfxRectD srcRod = args.regionOfInterest;
        rois.setRegionOfInterest(*this->_clipSrc, srcRod);

        // set roi of source ref
        OfxRectD srcRefRod = _clipSrcRef->getCanonicalRod(args.time);
        rois.setRegionOfInterest(*_clipSrcRef, srcRefRod);
    }
    else
    {
        // set roi of source
        OfxRectD srcRod = _clipSrc->getCanonicalRod(args.time);
        rois.setRegionOfInterest(*this->_clipSrc, srcRod);
    }

    // set roi of destination ref
    OfxRectD dstRefRod = _clipDstRef->getCanonicalRod(args.time);
    rois.setRegionOfInterest(*_clipDstRef, dstRefRod);
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void ColorTransferPlugin::render(const OFX::RenderArguments& args)
{
    //	doGilRender<ColorTransferProcess>( *this, args );

    // instantiate the render code based on the pixel depth of the dst clip
    OFX::EBitDepth bitDepth = _clipDst->getPixelDepth();
    OFX::EPixelComponent components = _clipDst->getPixelComponents();

    switch(components)
    {
        case OFX::ePixelComponentRGBA:
        {
            doGilRender<ColorTransferProcess, false, boost::gil::rgba_layout_t>(*this, args, bitDepth);
            return;
        }
        case OFX::ePixelComponentRGB:
        {
            doGilRender<ColorTransferProcess, false, boost::gil::rgb_layout_t>(*this, args, bitDepth);
            return;
        }
        case OFX::ePixelComponentAlpha:
        case OFX::ePixelComponentCustom:
        case OFX::ePixelComponentNone:
        {
            BOOST_THROW_EXCEPTION(exception::Unsupported() << exception::user() + "Pixel components (" +
                                                                  mapPixelComponentEnumToString(components) +
                                                                  ") not supported by the plugin.");
        }
    }
    BOOST_THROW_EXCEPTION(exception::Unknown());
}
}
}
}
