#include "ResizePlugin.hpp"
#include "ResizeProcess.hpp"
#include "ResizeDefinitions.hpp"

#include <tuttle/plugin/ofxToGil/point.hpp>

#include <terry/sampler/sampler.hpp>
#include <terry/point/operations.hpp>

#include <boost/gil/gil_all.hpp>

#include <boost/numeric/conversion/cast.hpp>

namespace tuttle
{
namespace plugin
{
namespace resize
{

using namespace ::terry::sampler;
using boost::numeric_cast;

ResizePlugin::ResizePlugin(OfxImageEffectHandle handle)
    : SamplerPlugin(handle)
{
    _paramMode = fetchChoiceParam(kParamMode);

    _paramFormat = fetchChoiceParam(kParamFormat);

    _paramScale = fetchDouble2DParam(kParamModeScale);

    _paramSize = fetchInt2DParam(kParamSize);
    _paramSizeWidth = fetchIntParam(kParamSizeWidth);
    _paramSizeHeight = fetchIntParam(kParamSizeHeight);
    _paramSizeOrientation = fetchChoiceParam(kParamSizeOrientation);
    _paramSizeKeepRatio = fetchBooleanParam(kParamSizeKeepRatio);

#if(TUTTLE_EXPERIMENTAL)
    _paramCenter = fetchBooleanParam(kParamCenter);
    _paramCenterPoint = fetchDouble2DParam(kParamCenterPoint);
#endif

    updateVisibleTools();
}

void ResizePlugin::updateVisibleTools()
{
    OFX::InstanceChangedArgs args(this->timeLineGetTime());
    changedParam(args, kParamMode);
#if(TUTTLE_EXPERIMENTAL)
    changedParam(args, kParamCenter);
#endif
    changedParam(args, kParamFilter);
    changedParam(args, kParamSizeKeepRatio);
    changedParam(args, kParamSizeOrientation);
}

ResizeProcessParams<ResizePlugin::Scalar> ResizePlugin::getProcessParams(const OfxPointD& renderScale) const
{
    ResizeProcessParams<Scalar> params;
#if(TUTTLE_EXPERIMENTAL)
    OfxPointD centerPoint = _paramCenterPoint->getValue();

    params._centerPoint.x = centerPoint.x;
    params._centerPoint.y = centerPoint.y;

    params._changeCenter = _paramCenter->getValue();
#endif
    SamplerPlugin::fillProcessParams(params._samplerProcessParams);

    return params;
}

void ResizePlugin::changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName)
{
    SamplerPlugin::changedParam(args, paramName);

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
#if(TUTTLE_EXPERIMENTAL)
    else if(paramName == kParamCenter)
    {
        if(_paramCenter->getValue())
        {
            _paramCenterPoint->setIsSecretAndDisabled(false);
        }
        else
        {
            _paramCenterPoint->setIsSecretAndDisabled(true);
        }
    }
#endif
}

bool ResizePlugin::getRegionOfDefinition(const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod)
{
    using namespace boost::gil;

    const OfxRectD srcRod = _clipSrc->getCanonicalRod(args.time);
    const Point2 srcRodSize(srcRod.x2 - srcRod.x1, srcRod.y2 - srcRod.y1);
    //	const OfxRectD srcRodInDstFrame = { 0, 0, srcRodSize.x, srcRodSize.y };

    // OfxPointD centerPoint = _paramCenterPoint->getValue();

    // TUTTLE_LOG_INFO( centerPoint.x << " x " << centerPoint.y );

    ResizeProcessParams<Scalar> params = getProcessParams();

    //	TUTTLE_LOG_INFO( rod.x1 << ", " << rod.y1 << " || " << rod.x2 << ", " << rod.y2 );

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
            /*
            const Point2 inA( srcRodInDstFrame.x1, srcRodInDstFrame.y1 ); // top left corner
            const Point2 inB( srcRodInDstFrame.x2, srcRodInDstFrame.y2 ); // down right corner
            const Point2 center( srcRodSize * 0.5 );
            */

            const Point2 scale = ofxToGil(_paramScale->getValue());
            if(scale.x == 0.0 || scale.y == 0.0)
                return false;

            const Point2 dstSize(srcRodSize * scale);
            rod.x1 = 0;
            rod.y1 = 0;
            rod.x2 = dstSize.x;
            rod.y2 = dstSize.y;
            /*
            const Point2 outA( center + ((inA - center) * scale) );
            const Point2 outB( center + ((inB - center) * scale) );
            rod.x1 = outA.x;
            rod.y1 = outA.y;
            rod.x2 = outB.x;
            rod.y2 = outB.y;
            */

            return true;
        }
    }

    //	TUTTLE_LOG_INFO( rod.x1 << ", " << rod.y1 << " || " << rod.x2 << ", " << rod.y2 );
    return false;
}

void ResizePlugin::getRegionsOfInterest(const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois)
{
    /*OfxRectD srcRoi;
    const OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
    srcRoi.x1 = srcRod.x1 - 1;
    srcRoi.y1 = srcRod.y1 - 1;
    srcRoi.x2 = srcRod.x2 + 1;
    srcRoi.y2 = srcRod.y2 + 1;
    rois.setRegionOfInterest( *_clipSrc, srcRoi );
*/
    /*ResizeProcessParams<Scalar> params = getProcessParams();
    OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
//
    OfxRectD srcRoi;
    srcRoi.x1 = srcRod.x1 - 1;
    srcRoi.y1 = srcRod.y1 - 1;
    srcwRoi.x2 = srcRod.x2 + 1;
    srcRoi.y2 = srcRod.y2 + 1;
    rois.setRegionOfInterest( *_clipSrc, srcRoi );*/
    /*
    OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
    OfxRectD dstRod = _clipDst->getCanonicalRod( args.time );

    ResizeProcessParams<Scalar> params = getProcessParams();

    OfxRectD outputRoi = args.regionOfInterest;
    outputRoi.x1 -= dstRod.x1; // to dest rod coordinates
    outputRoi.y1 -= dstRod.y1;
    outputRoi.x2 -= dstRod.x1;
    outputRoi.y2 -= dstRod.y1;
    OfxRectD srcRoi = ;
    srcRoi.x1    += srcRod.x1; // to RoW coordinates
    srcRoi.y1    += srcRod.y1;
    srcRoi.x2    += srcRod.x1;
    srcRoi.y2    += srcRod.y1;
    outputRoi.x1 += dstRod.x1; // to RoW coordinates
    outputRoi.y1 += dstRod.y1;
    outputRoi.x2 += dstRod.x1;
    outputRoi.y2 += dstRod.y1;
    //    srcRoi.x1 += 2; // if we remove 2 pixels to the needed RoI the plugin crash, because it tries to access to this
    pixels
    //    srcRoi.y1 += 2; // so the calcul of the RoI has a precision of one pixel
    //    srcRoi.x2 -= 2;
    //    srcRoi.y2 -= 2;
    OfxRectD srcRealRoi = rectanglesIntersection( srcRoi, srcRod );
    srcRealRoi = srcRod;*/

    // rois.setRegionOfInterest( *_clipSrc, outputRoi );
    /*
            if( _debugDisplayRoi->getValue() )
            {
                    _srcRealRoi = srcRealRoi;
                    _srcRoi     = srcRoi;
                    _dstRoi     = outputRoi;
            }*/
}

bool ResizePlugin::isIdentity(const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime)
{
    //	ResizeProcessParams<Scalar> params = getProcessParams();
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
void ResizePlugin::render(const OFX::RenderArguments& args)
{
    doGilRender<ResizeProcess>(*this, args);
}
}
}
}
