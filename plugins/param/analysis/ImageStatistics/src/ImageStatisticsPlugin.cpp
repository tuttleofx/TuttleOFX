#include "ImageStatisticsPlugin.hpp"
#include "ImageStatisticsProcess.hpp"
#include "ImageStatisticsDefinitions.hpp"

#include <boost/numeric/conversion/cast.hpp>

#include <tuttle/plugin/interact/InteractInfos.hpp>
#include <tuttle/plugin/numeric/rectOp.hpp>

#include <boost/gil/gil_all.hpp>

namespace tuttle
{
namespace plugin
{
namespace imageStatistics
{

ImageStatisticsPlugin::ImageStatisticsPlugin(OfxImageEffectHandle handle)
    : ImageEffectGilPlugin(handle)
{
    _clipMask = fetchClip(kClipMask);

    _paramRestrictToRegion = fetchBooleanParam(kParamRestrictToRegion);

    _paramCoordinateSystem = fetchChoiceParam(kParamCoordinateSystem);
    _paramRectCenter = fetchDouble2DParam(kParamRectCenter);
    _paramRectSize = fetchDouble2DParam(kParamRectSize);
    _paramChooseOutput = fetchChoiceParam(kParamChooseOutput);

    _paramOutputNbPixels = fetchIntParam(kParamOutputNbPixels);

    _paramOutputAverage = fetchRGBAParam(kParamOutputAverage);
    _paramOutputVariance = fetchRGBAParam(kParamOutputVariance);
    _paramOutputChannelMin = fetchRGBAParam(kParamOutputChannelMin);
    _paramOutputChannelMax = fetchRGBAParam(kParamOutputChannelMax);
    _paramOutputLuminosityMin = fetchRGBAParam(kParamOutputLuminosityMin);
    _paramOutputLuminosityMax = fetchRGBAParam(kParamOutputLuminosityMax);
    _paramOutputKurtosis = fetchRGBAParam(kParamOutputKurtosis);
    _paramOutputSkewness = fetchRGBAParam(kParamOutputSkewness);

    _paramOutputAverageHSL = fetchDouble3DParam(kParamOutputAverageHSL);
    _paramOutputVarianceHSL = fetchDouble3DParam(kParamOutputVarianceHSL);
    _paramOutputChannelMinHSL = fetchDouble3DParam(kParamOutputChannelMinHSL);
    _paramOutputChannelMaxHSL = fetchDouble3DParam(kParamOutputChannelMaxHSL);
    _paramOutputLuminosityMinHSL = fetchDouble3DParam(kParamOutputLuminosityMinHSL);
    _paramOutputLuminosityMaxHSL = fetchDouble3DParam(kParamOutputLuminosityMaxHSL);
    _paramOutputKurtosisHSL = fetchDouble3DParam(kParamOutputKurtosisHSL);
    _paramOutputSkewnessHSL = fetchDouble3DParam(kParamOutputSkewnessHSL);

    OFX::InstanceChangedArgs args(this->timeLineGetTime());
    changedParam(args, kParamRestrictToRegion);
}

ImageStatisticsProcessParams ImageStatisticsPlugin::getProcessParams(const OfxTime time, const OfxPointD& renderScale) const
{
    ImageStatisticsProcessParams params;
    OfxPointD rectCenter = _paramRectCenter->getValue();
    OfxPointD rectSize = _paramRectSize->getValue();

    rectSize.x = std::abs(rectSize.x);
    rectSize.y = std::abs(rectSize.y);
    if(_paramCoordinateSystem->getValue() == eParamCoordinateSystemNormalized)
    {
        OfxPointD projectSize = this->getProjectSize();
        rectCenter *= projectSize;
        rectSize *= projectSize;
    }

    // Region is input image RoD
    params._rect = _clipSrc->getPixelRod(time);
    if(_paramRestrictToRegion->getValue())
    {
        // Intersection with user custom region
        OfxRectI userRect;
        userRect.x1 = boost::numeric_cast<int>(rectCenter.x - rectSize.x);
        userRect.y1 = boost::numeric_cast<int>(rectCenter.y - rectSize.y);
        userRect.x2 = boost::numeric_cast<int>(std::ceil(rectCenter.x + rectSize.x));
        userRect.y2 = boost::numeric_cast<int>(std::ceil(rectCenter.y + rectSize.y));

        params._rect = rectanglesIntersection(params._rect, userRect);
    }
    if(_clipMask->isConnected())
    {
        // Intersection with input mask RoD
        params._rect = rectanglesIntersection(params._rect, _clipMask->getPixelRod(time));
    }

    params._rect *= renderScale;

    params._chooseOutput = static_cast<EParamChooseOutput>(_paramChooseOutput->getValue());

    return params;
}

void ImageStatisticsPlugin::getRegionsOfInterest(const OFX::RegionsOfInterestArguments& args,
                                                 OFX::RegionOfInterestSetter& rois)
{
    ImageStatisticsProcessParams params = getProcessParams(args.time, args.renderScale);
    if(params._chooseOutput == eParamChooseOutputSource)
    {
        // In that case we need the full image input...
        // Not to compute statistics, but to copy the image.
        return;
    }
    OfxRectI inputRoi = params._rect; // we need the selected rectangle
    OfxRectD inputRoi_d = rectIntToDouble(inputRoi);

    rois.setRegionOfInterest(*_clipSrc, inputRoi_d);
    rois.setRegionOfInterest(*_clipMask, inputRoi_d);
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void ImageStatisticsPlugin::render(const OFX::RenderArguments& args)
{
    // instantiate the render code based on the pixel depth of the dst clip
    OFX::EBitDepth bitDepth = _clipDst->getPixelDepth();
    OFX::EPixelComponent components = _clipDst->getPixelComponents();

    switch(components)
    {
        case OFX::ePixelComponentRGBA:
        {
            switch(bitDepth)
            {
                case OFX::eBitDepthFloat:
                {
                    doGilRender<ImageStatisticsProcess, false, boost::gil::rgba_layout_t, boost::gil::bits32f>(*this, args);
                    return;
                }
                case OFX::eBitDepthUByte:
                case OFX::eBitDepthUShort:
                case OFX::eBitDepthNone:
                case OFX::eBitDepthCustom:
                {
                    BOOST_THROW_EXCEPTION(exception::Unsupported() << exception::user() + "Bit depth (" +
                                                                          mapBitDepthEnumToString(bitDepth) +
                                                                          ") not recognized by the plugin.");
                }
            }
            break;
        }
        case OFX::ePixelComponentRGB:
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

void ImageStatisticsPlugin::changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName)
{
    if(paramName == kParamCoordinateSystem)
    {
        OfxPointD projectSize = this->getProjectSize();
        OfxPointD rectCenter = _paramRectCenter->getValue();
        OfxPointD rectSize = _paramRectSize->getValue();
        switch(_paramCoordinateSystem->getValue())
        {
            case eParamCoordinateSystemCanonical:
            {
                rectCenter *= projectSize;
                rectSize *= projectSize;
                break;
            }
            case eParamCoordinateSystemNormalized:
            {
                rectCenter /= projectSize;
                rectSize /= projectSize;
                break;
            }
            default:
                BOOST_THROW_EXCEPTION(exception::Value() << exception::user("Unrecognized coordinate system option."));
        }
        _paramRectCenter->setValue(rectCenter);
        _paramRectSize->setValue(rectSize);
    }
    else if(paramName == kParamRestrictToRegion)
    {
        const bool useRegion = _paramRestrictToRegion->getValue();

        _paramCoordinateSystem->setEnabled(useRegion);
        _paramRectCenter->setEnabled(useRegion);
        _paramRectSize->setEnabled(useRegion);
    }

    if(paramName == kParamCoordinateSystem || paramName == kParamRectCenter || paramName == kParamRectSize)
    {
        _paramRestrictToRegion->setValue(true);
    }
}
}
}
}
