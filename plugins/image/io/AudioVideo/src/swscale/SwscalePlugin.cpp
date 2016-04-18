#include "SwscalePlugin.hpp"
#include "SwscaleProcess.hpp"
#include "SwscaleDefinitions.hpp"

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif
#include <libswscale/swscale.h>
}

#include <tuttle/plugin/ofxToGil/point.hpp>
#include <terry/point/operations.hpp>
#include <boost/numeric/conversion/cast.hpp>

namespace tuttle
{
namespace plugin
{
namespace swscale
{

using boost::numeric_cast;

int filterParamToSwscaleFlag(const EParamFilter paramFilter)
{
    switch(paramFilter)
    {
        case eParamFilterFastBilinear:
            return SWS_FAST_BILINEAR;
        case eParamFilterBilinear:
            return SWS_BILINEAR;
        case eParamFilterBicubic:
            return SWS_BICUBIC;
        case eParamFilterX:
            return SWS_X;
        case eParamFilterPoint:
            return SWS_POINT;
        case eParamFilterArea:
            return SWS_AREA;
        case eParamFilterBicublin:
            return SWS_BICUBLIN;
        case eParamFilterGauss:
            return SWS_GAUSS;
        case eParamFilterSinc:
            return SWS_SINC;
        case eParamFilterLanczos:
            return SWS_LANCZOS;
        case eParamFilterSpline:
            return SWS_SPLINE;
    }
    BOOST_ASSERT(false);
    return SWS_FAST_BILINEAR;
}

SwscalePlugin::SwscalePlugin(OfxImageEffectHandle handle)
    : ImageEffectGilPlugin(handle)
{
    _paramMode = fetchChoiceParam(kParamMode);

    _paramFormat = fetchChoiceParam(kParamFormat);

    _paramScale = fetchDouble2DParam(kParamModeScale);

    _paramSize = fetchInt2DParam(kParamSize);
    _paramSizeWidth = fetchIntParam(kParamSizeWidth);
    _paramSizeHeight = fetchIntParam(kParamSizeHeight);
    _paramSizeOrientation = fetchChoiceParam(kParamSizeOrientation);
    _paramSizeKeepRatio = fetchBooleanParam(kParamSizeKeepRatio);

    _paramFilter = fetchChoiceParam(kParamFilter);

    updateVisibleTools();
}

void SwscalePlugin::updateVisibleTools()
{
    OFX::InstanceChangedArgs args(this->timeLineGetTime());

    changedParam(args, kParamMode);
    changedParam(args, kParamFilter);
    changedParam(args, kParamSizeKeepRatio);
    changedParam(args, kParamSizeOrientation);
}

SwscaleProcessParams SwscalePlugin::getProcessParams(const OfxPointD& renderScale) const
{
    SwscaleProcessParams params;

    params._filter = static_cast<EParamFilter>(_paramFilter->getValue());
    params._sws_filter = filterParamToSwscaleFlag(params._filter);

    return params;
}

void SwscalePlugin::changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName)
{
    ImageEffectGilPlugin::changedParam(args, paramName);

    if(paramName == kParamMode)
    {
        switch(_paramMode->getValue())
        {
            case eParamModeFormat:
            {
                _paramScale->setIsSecretAndDisabled(true);
                _paramSize->setIsSecretAndDisabled(true);
                _paramSizeWidth->setIsSecretAndDisabled(true);
                _paramSizeHeight->setIsSecretAndDisabled(true);
                _paramSizeOrientation->setIsSecretAndDisabled(true);
                _paramSizeKeepRatio->setIsSecretAndDisabled(true);

                _paramFormat->setIsSecretAndDisabled(false);
                break;
            }
            case eParamModeSize:
            {
                const bool keepRatio = _paramSizeKeepRatio->getValue();
                const EParamSizeOrientation orientation =
                    static_cast<EParamSizeOrientation>(_paramSizeOrientation->getValue());

                _paramFormat->setIsSecretAndDisabled(true);
                _paramScale->setIsSecretAndDisabled(true);

                _paramSizeKeepRatio->setIsSecretAndDisabled(false);

                _paramSizeWidth->setIsSecretAndDisabled(!keepRatio || orientation != eParamSizeOrientationX);
                _paramSizeHeight->setIsSecretAndDisabled(!keepRatio || orientation != eParamSizeOrientationY);
                _paramSizeOrientation->setIsSecretAndDisabled(!keepRatio);
                _paramSize->setIsSecretAndDisabled(keepRatio);
                break;
            }
            case eParamModeScale:
            {
                _paramFormat->setIsSecretAndDisabled(true);
                _paramSize->setIsSecretAndDisabled(true);
                _paramSizeWidth->setIsSecretAndDisabled(true);
                _paramSizeHeight->setIsSecretAndDisabled(true);
                _paramSizeOrientation->setIsSecretAndDisabled(true);
                _paramSizeKeepRatio->setIsSecretAndDisabled(true);

                _paramScale->setIsSecretAndDisabled(false);
                break;
            }
        }
    }
    else if(paramName == kParamFormat && args.reason == OFX::eChangeUserEdit)
    {
        std::size_t width = 0;
        std::size_t height = 0;
        getFormatResolution(static_cast<EParamFormat>(_paramFormat->getValue()), width, height);

        _paramMode->setValue(eParamModeFormat);
        _paramSize->setValue(numeric_cast<int>(width), numeric_cast<int>(height));
        _paramSizeWidth->setValue(numeric_cast<int>(width));
        _paramSizeHeight->setValue(numeric_cast<int>(height));
    }
    else if(paramName == kParamSize && args.reason == OFX::eChangeUserEdit)
    {
        const OfxPointI s = _paramSize->getValue();

        _paramMode->setValue(eParamModeSize);
        _paramSizeWidth->setValue(s.x);
        _paramSizeHeight->setValue(s.y);
    }
    else if(paramName == kParamSizeKeepRatio && args.reason == OFX::eChangeUserEdit)
    {
        const bool keepRatio = _paramSizeKeepRatio->getValue();
        const EParamSizeOrientation orientation = static_cast<EParamSizeOrientation>(_paramSizeOrientation->getValue());

        _paramSizeWidth->setIsSecretAndDisabled(!keepRatio || orientation != eParamSizeOrientationX);
        _paramSizeHeight->setIsSecretAndDisabled(!keepRatio || orientation != eParamSizeOrientationY);
        _paramSizeOrientation->setIsSecretAndDisabled(!keepRatio);
        _paramSize->setIsSecretAndDisabled(keepRatio);
    }
    else if(paramName == kParamSizeOrientation && args.reason == OFX::eChangeUserEdit)
    {
        const bool keepRatio = _paramSizeKeepRatio->getValue();
        const EParamSizeOrientation orientation = static_cast<EParamSizeOrientation>(_paramSizeOrientation->getValue());

        _paramSizeWidth->setIsSecretAndDisabled(!keepRatio || orientation != eParamSizeOrientationX);
        _paramSizeHeight->setIsSecretAndDisabled(!keepRatio || orientation != eParamSizeOrientationY);
    }
    else if(paramName == kParamSizeWidth && args.reason == OFX::eChangeUserEdit)
    {
        _paramMode->setValue(eParamModeSize);
        _paramSizeKeepRatio->setValue(true);
        _paramSizeOrientation->setValue(eParamSizeOrientationX);

        _paramSize->setValue(_paramSizeWidth->getValue(), _paramSize->getValue().y);
    }
    else if(paramName == kParamSizeHeight && args.reason == OFX::eChangeUserEdit)
    {
        _paramMode->setValue(eParamModeSize);
        _paramSizeKeepRatio->setValue(true);
        _paramSizeOrientation->setValue(eParamSizeOrientationY);

        _paramSize->setValue(_paramSize->getValue().x, _paramSizeHeight->getValue());
    }
    else if(paramName == kParamScale && args.reason == OFX::eChangeUserEdit)
    {
        _paramMode->setValue(eParamModeScale);
    }
}

bool SwscalePlugin::getRegionOfDefinition(const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod)
{
    using namespace boost::gil;

    const OfxRectD srcRod = _clipSrc->getCanonicalRod(args.time);
    const Point2 srcRodSize(srcRod.x2 - srcRod.x1, srcRod.y2 - srcRod.y1);

    switch(_paramMode->getValue())
    {
        case eParamModeFormat:
        {
            std::size_t width = 0;
            std::size_t height = 0;
            getFormatResolution(static_cast<EParamFormat>(_paramFormat->getValue()), width, height);
            rod.x1 = 0;
            rod.y1 = 0;
            rod.x2 = width;
            rod.y2 = height;

            return true;
        }
        case eParamModeSize:
        {
            std::size_t sizex = 0;
            std::size_t sizey = 0;
            if(_paramSizeKeepRatio->getValue())
            {
                if(_paramSizeOrientation->getValue() == eParamSizeOrientationX)
                {
                    sizex = _paramSizeWidth->getValue();
                    sizey = srcRodSize.y / numeric_cast<double>(srcRodSize.x) * sizex;
                }
                else // direction == eParamSizeY
                {
                    sizey = _paramSizeHeight->getValue();
                    sizex = srcRodSize.x / numeric_cast<double>(srcRodSize.y) * sizey;
                }
            }
            else
            {
                const OfxPointI s = _paramSize->getValue();
                sizex = s.x;
                sizey = s.y;
            }

            rod.x1 = 0;
            rod.y1 = 0;
            rod.x2 = sizex;
            rod.y2 = sizey;

            return true;
        }
        case eParamModeScale:
        {
            const Point2 scale = ofxToGil(_paramScale->getValue());
            if(scale.x == 0.0 || scale.y == 0.0)
                return false;

            const Point2 dstSize(srcRodSize * scale);
            rod.x1 = 0;
            rod.y1 = 0;
            rod.x2 = dstSize.x;
            rod.y2 = dstSize.y;
            return true;
        }
    }

    return false;
}

// void SwscalePlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois )
//{
//	// TODO: to enable tiles...
//}

bool SwscalePlugin::isIdentity() const
{
    switch(_paramMode->getValue())
    {
        case eParamModeFormat:
        {
            ///@todo could we read the input rod at this step?
            return false;
        }
        case eParamModeSize:
        {
            ///@todo could we read the input rod at this step?
            return false;
        }
        case eParamModeScale:
        {
            const Point2 scale = ofxToGil(_paramScale->getValue());
            if(scale.x == 1.0 && scale.y == 1.0)
                return true;
            return false;
        }
    }
    return false;
}

bool SwscalePlugin::isIdentity(const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime)
{
    if(isIdentity())
    {
        identityClip = _clipSrc;
        identityTime = args.time;
        return true;
    }
    return false;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void SwscalePlugin::render(const OFX::RenderArguments& args)
{
    OFX::ImageEffectHostDescription* desc = OFX::getImageEffectHostDescription();

    if(desc->_supportedPixelDepths.size() == 1 && OFX::eBitDepthFloat == desc->_supportedPixelDepths.at(0))
    {
        BOOST_THROW_EXCEPTION(exception::BitDepthMismatch() << exception::user("SwScale: unsupported plugin on this host."));
    }

    SwscaleProcess procObj(*this);
    procObj.setupAndProcess(args);
}
}
}
}
