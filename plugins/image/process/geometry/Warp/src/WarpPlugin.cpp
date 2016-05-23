#include "WarpPlugin.hpp"
#include "WarpProcess.hpp"
#include "WarpDefinitions.hpp"
#include "Bezier/bezier.hpp"

#include <tuttle/plugin/ofxToGil/image.hpp>
#include <tuttle/plugin/global.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/assign/list_of.hpp>

#include <cstddef>

namespace tuttle
{
namespace plugin
{
namespace warp
{

WarpPlugin::WarpPlugin(OfxImageEffectHandle handle)
    : ImageEffect(handle)
{
    _clipSrc = fetchClip(kOfxImageEffectSimpleSourceClipName);
    _clipSrcB = fetchClip(kClipSourceB);
    _clipDst = fetchClip(kOfxImageEffectOutputClipName);

    _paramOverlay = fetchBooleanParam(kParamOverlay);
    _paramInverse = fetchBooleanParam(kParamInverse);
    _paramReset = fetchPushButtonParam(kParamReset);
    _paramNextCurve = fetchPushButtonParam(kParamNextCurve);
    _paramSetKey = fetchPushButtonParam(kParamSetKey);

    _paramMethod = fetchChoiceParam(kParamMethod);
    _paramNbPoints = fetchIntParam(kParamNbPoints);
    _transition = fetchDoubleParam(kParamTransition);

    _paramRigiditeTPS = fetchDoubleParam(kParamRigiditeTPS);
    _paramNbPointsBezier = fetchIntParam(kParamNbPointsBezier);

    // Multi curve
    for(std::size_t cptCBegin = 0; cptCBegin < kMaxNbPoints; ++cptCBegin)
    {
        _paramCurveBegin[cptCBegin] = fetchBooleanParam(kParamCurveBegin + boost::lexical_cast<std::string>(cptCBegin));
        _paramCurveBegin[cptCBegin]->setDefault(false);
    }

    // Param IN
    _paramGroupIn = fetchGroupParam(kParamGroupIn);
    for(std::size_t cptIn = 0; cptIn < kMaxNbPoints; ++cptIn)
    {
        _paramPointIn[cptIn] = fetchDouble2DParam(kParamPointIn + boost::lexical_cast<std::string>(cptIn));
    }
    _paramOverlayIn = fetchBooleanParam(kParamOverlayIn);
    _paramOverlayInColor = fetchRGBParam(kParamOverlayInColor);

    // Param OUT
    _paramGroupOut = fetchGroupParam(kParamGroupIn);
    for(std::size_t cptOut = 0; cptOut < kMaxNbPoints; ++cptOut)
    {
        _paramPointOut[cptOut] = fetchDouble2DParam(kParamPointOut + boost::lexical_cast<std::string>(cptOut));
    }
    _paramOverlayOut = fetchBooleanParam(kParamOverlayOut);
    _paramOverlayOutColor = fetchRGBParam(kParamOverlayOutColor);

    // Param TGT IN
    _paramGroupTgtIn = fetchGroupParam(kParamGroupTgtIn);
    for(std::size_t cptTgtIn = 0; cptTgtIn < kMaxNbPoints; ++cptTgtIn)
    {
        _paramPointTgtIn[2 * cptTgtIn] =
            fetchDouble2DParam(kParamPointTgtIn + boost::lexical_cast<std::string>(2 * cptTgtIn));
        _paramPointTgtIn[2 * cptTgtIn + 1] =
            fetchDouble2DParam(kParamPointTgtIn + boost::lexical_cast<std::string>(2 * cptTgtIn + 1));
    }
    _paramOverlayTgtIn = fetchBooleanParam(kParamOverlayTgtIn);
    _paramOverlayTgtInColor = fetchRGBParam(kParamOverlayTgtInColor);

    // Param TGT Out
    _paramGroupTgtOut = fetchGroupParam(kParamGroupTgtOut);
    for(std::size_t cptTgtOut = 0; cptTgtOut < kMaxNbPoints; ++cptTgtOut)
    {
        _paramPointTgtOut[2 * cptTgtOut] =
            fetchDouble2DParam(kParamPointTgtOut + boost::lexical_cast<std::string>(2 * cptTgtOut));
        _paramPointTgtOut[2 * cptTgtOut + 1] =
            fetchDouble2DParam(kParamPointTgtOut + boost::lexical_cast<std::string>(2 * cptTgtOut + 1));
    }
    _paramOverlayTgtOut = fetchBooleanParam(kParamOverlayTgtOut);
    _paramOverlayTgtOutColor = fetchRGBParam(kParamOverlayTgtOutColor);

    // Param speciaux
    _instanceChangedArgs.time = 0;
    _instanceChangedArgs.renderScale.x = 1;
    _instanceChangedArgs.renderScale.y = 1;
    _instanceChangedArgs.reason = OFX::eChangePluginEdit;
    changedParam(_instanceChangedArgs, kParamNbPoints); // init IsSecret property for each pair of points parameter
}

WarpProcessParams<WarpPlugin::Scalar> WarpPlugin::getProcessParams(const OfxPointD& renderScale) const
{
    using namespace boost::assign;
    WarpProcessParams<Scalar> params;
    const std::size_t nbPoints = _paramNbPoints->getValue();
    params._nbPoints = nbPoints;

    params._rigiditeTPS = _paramRigiditeTPS->getValue();
    params._transition = _transition->getValue();
    params._method = static_cast<EParamMethod>(_paramMethod->getValue());

    if(nbPoints <= 1)
    {
        /// @todo: in this case it's just a translation...
        // TUTTLE_TCOUT_WITHINFOS( "TODO !" );
    }
    const std::size_t nbBezierPoints = _paramNbPointsBezier->getValue();
    // TUTTLE_TCOUT_VAR( nbBezierPoints );
    if(nbPoints == 0)
    {
        return params;
    }
    for(std::size_t c = 0; c < nbPoints - 1; ++c)
    {

        if(_paramCurveBegin[c + 1]->getValue())
            continue;
        // Creation des points et des ptTangente et recuperation des valeurs
        // points a relier
        Point2 pIn1 = ofxToGil(_paramPointIn[c]->getValue());
        params._inPoints.push_back(pIn1);
        Point2 pIn2 = ofxToGil(_paramPointIn[c + 1]->getValue());

        Point2 pOut1 = ofxToGil(_paramPointOut[c]->getValue());
        params._outPoints.push_back(pOut1);
        Point2 pOut2 = ofxToGil(_paramPointOut[c + 1]->getValue());

        // Points de la tangente
        Point2 tIn1 = ofxToGil(_paramPointTgtIn[(2 * c) + 1]->getValue());
        Point2 tIn2 = ofxToGil(_paramPointTgtIn[(2 * c) + 2]->getValue());
        params._tgtPointsIn.push_back(tIn1);
        params._tgtPointsIn.push_back(tIn2);

        Point2 tOut1 = ofxToGil(_paramPointTgtOut[(2 * c) + 1]->getValue());
        Point2 tOut2 = ofxToGil(_paramPointTgtOut[(2 * c) + 2]->getValue());

        params._tgtPointsOut.push_back(tOut1);
        params._tgtPointsOut.push_back(tOut2);

        // Creation et remplissage du tableau necessaire a Bezier
        // TUTTLE_TCOUT_INFOS;
        // TUTTLE_TCOUT_VAR( c );
        {
            std::vector<Point2> tabPtsIn;
            tabPtsIn.push_back(pIn1);
            tabPtsIn.push_back(tIn1);
            tabPtsIn.push_back(tIn2);
            tabPtsIn.push_back(pIn2);

            params._bezierIn.push_back(pIn1);
            bezier::bezierSubdivide(tabPtsIn, nbBezierPoints, params._bezierIn);
        }
        {
            std::vector<Point2> tabPtsOut;
            tabPtsOut.push_back(pOut1);
            tabPtsOut.push_back(tOut1);
            tabPtsOut.push_back(tOut2);
            tabPtsOut.push_back(pOut2);

            params._bezierOut.push_back(pOut1);
            bezier::bezierSubdivide(tabPtsOut, nbBezierPoints, params._bezierOut);
        }
        // TUTTLE_TCOUT_INFOS;
    }
    const std::size_t c = nbPoints - 1;
    Point2 pIn = ofxToGil(_paramPointIn[c]->getValue());
    params._inPoints.push_back(pIn);
    params._bezierIn.push_back(pIn);
    Point2 pOut = ofxToGil(_paramPointOut[c]->getValue());
    params._outPoints.push_back(pOut);
    params._bezierOut.push_back(pOut);

    if(_paramInverse->getValue())
    {
        std::swap(params._bezierIn, params._bezierOut);
    }
    return params;
}

void WarpPlugin::changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName)
{
    if(boost::starts_with(paramName, kParamPointIn) || boost::starts_with(paramName, kParamPointOut) ||
       boost::starts_with(paramName, kParamPointTgtIn) || boost::starts_with(paramName, kParamPointTgtOut) ||
       paramName == kParamNbPoints)
    {
        switch(static_cast<EParamMethod>(_paramMethod->getValue()))
        {
            case eParamMethodCreation:
            {
                std::size_t size = _paramNbPoints->getValue();
                std::size_t i = 0;
                for(; i < size; ++i)
                {
                    _paramPointIn[i]->setIsSecretAndDisabled(false);
                    _paramPointOut[i]->setIsSecretAndDisabled(false);
                    _paramPointTgtIn[2 * i]->setIsSecretAndDisabled(false);
                    _paramPointTgtIn[2 * i + 1]->setIsSecretAndDisabled(false);
                    _paramPointTgtOut[2 * i]->setIsSecretAndDisabled(false);
                    _paramPointTgtOut[2 * i + 1]->setIsSecretAndDisabled(false);
                }
                for(; i < kMaxNbPoints; ++i)
                {
                    _paramPointIn[i]->setIsSecretAndDisabled(true);
                    _paramPointOut[i]->setIsSecretAndDisabled(true);
                    _paramPointTgtIn[2 * i]->setIsSecretAndDisabled(true);
                    _paramPointTgtIn[2 * i + 1]->setIsSecretAndDisabled(true);
                    _paramPointTgtOut[2 * i]->setIsSecretAndDisabled(true);
                    _paramPointTgtOut[2 * i + 1]->setIsSecretAndDisabled(true);
                }
                break;
            }
            case eParamMethodDelete:
            {
                break;
            }
            case eParamMethodMove:
            {
                break;
            }
        }
    }
    // Si le mode est Reset
    else if(paramName == kParamReset)
    {
        for(std::size_t i = 0; i < kMaxNbPoints; ++i)
        {
            _paramPointIn[i]->setIsSecretAndDisabled(true);
            _paramPointIn[i]->setValue(positionOrigine, positionOrigine);
            _paramPointOut[i]->setIsSecretAndDisabled(true);
            _paramPointOut[i]->setValue(positionOrigine, positionOrigine);

            _paramPointTgtIn[2 * i]->setIsSecretAndDisabled(true);
            _paramPointTgtIn[2 * i]->setValue(positionOrigine, positionOrigine);
            _paramPointTgtIn[(2 * i) + 1]->setIsSecretAndDisabled(true);
            _paramPointTgtIn[(2 * i) + 1]->setValue(positionOrigine, positionOrigine);

            _paramPointTgtOut[2 * i]->setIsSecretAndDisabled(true);
            _paramPointTgtOut[(2 * i) + 1]->setIsSecretAndDisabled(true);

            _paramNbPoints->setValue(0);
        }
    }
    // Si le mode est MultiCurve
    else if(paramName == kParamNextCurve)
    {
        TUTTLE_LOG_DEBUG(_paramNbPoints->getValue());
        _paramCurveBegin[(_paramNbPoints->getValue())]->setValue(true);
        TUTTLE_LOG_DEBUG(_paramCurveBegin[(_paramNbPoints->getValue())]->getValue());
    }
    else if(paramName == kParamSetKey)
    {
        // @todo adrien: c'est peut-etre mieux de le faire uniquement sur les points cree ? _paramNbPoints->getValue()
        for(std::size_t i = 0; i < kMaxNbPoints; ++i)
        {
            _paramPointIn[i]->setValueAtTime(args.time, _paramPointIn[i]->getValue());
            _paramPointOut[i]->setValueAtTime(args.time, _paramPointOut[i]->getValue());

            _paramPointTgtIn[2 * i]->setValueAtTime(args.time, _paramPointTgtIn[2 * i]->getValue());
            _paramPointTgtIn[(2 * i) + 1]->setValueAtTime(args.time, _paramPointTgtIn[(2 * i) + 1]->getValue());

            _paramPointTgtOut[2 * i]->setValueAtTime(args.time, _paramPointTgtOut[2 * i]->getValue());
            _paramPointTgtOut[(2 * i) + 1]->setValueAtTime(args.time, _paramPointTgtOut[(2 * i) + 1]->getValue());
        }
    }
}

bool WarpPlugin::isIdentity(const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime)
{
    WarpProcessParams<Scalar> params = getProcessParams();
    if(params._nbPoints == 0)
    {
        identityClip = _clipSrc;
        identityTime = args.time;
        return true;
    }
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
void WarpPlugin::render(const OFX::RenderArguments& args)
{
    using namespace boost::gil;
    // instantiate the render code based on the pixel depth of the dst clip
    OFX::EBitDepth dstBitDepth = _clipDst->getPixelDepth();
    OFX::EPixelComponent dstComponents = _clipDst->getPixelComponents();

    // do the rendering
    if(dstComponents == OFX::ePixelComponentRGBA)
    {
        switch(dstBitDepth)
        {
            case OFX::eBitDepthUByte:
            {
                WarpProcess<rgba8_view_t> p(*this);
                p.setupAndProcess(args);
                break;
            }
            case OFX::eBitDepthUShort:
            {
                WarpProcess<rgba16_view_t> p(*this);
                p.setupAndProcess(args);
                break;
            }
            case OFX::eBitDepthFloat:
            {
                WarpProcess<rgba32f_view_t> p(*this);
                p.setupAndProcess(args);
                break;
            }
            default:
            {
                TUTTLE_LOG_ERROR("Bit depth (" << mapBitDepthEnumToString(dstBitDepth) << ") not recognized by the plugin.");

                break;
            }
        }
    }
    else if(dstComponents == OFX::ePixelComponentAlpha)
    {
        switch(dstBitDepth)
        {
            case OFX::eBitDepthUByte:
            {
                WarpProcess<gray8_view_t> p(*this);
                p.setupAndProcess(args);
                break;
            }
            case OFX::eBitDepthUShort:
            {
                WarpProcess<gray16_view_t> p(*this);
                p.setupAndProcess(args);
                break;
            }
            case OFX::eBitDepthFloat:
            {
                WarpProcess<gray32f_view_t> p(*this);
                p.setupAndProcess(args);
                break;
            }
            default:
            {

                TUTTLE_LOG_ERROR("Bit depth (" << mapBitDepthEnumToString(dstBitDepth) << ") not recognized by the plugin.");

                break;
            }
        }
    }
    else
    {
        TUTTLE_LOG_ERROR("Pixel components (" << mapPixelComponentEnumToString(dstComponents)
                                              << ") not supported by the plugin.");
    }
}
}
}
}
