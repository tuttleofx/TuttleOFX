#ifndef _TUTTLE_PLUGIN_WARP_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_WARP_PLUGIN_HPP_

#include "WarpDefinitions.hpp"

#include <tuttle/plugin/global.hpp>

#include <ofxsImageEffect.h>

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/gil/gil_all.hpp>
#include <boost/array.hpp>

namespace tuttle
{
namespace plugin
{
namespace warp
{

using namespace boost::gil;

template <typename Scalar>
struct WarpProcessParams
{

    std::vector<point2<Scalar> > _inPoints;
    std::vector<point2<Scalar> > _outPoints;

    std::vector<point2<Scalar> > _tgtPointsIn;
    std::vector<point2<Scalar> > _tgtPointsOut;

    std::vector<point2<Scalar> > _buildPoints;

    std::vector<point2<Scalar> > _bezierIn;
    std::vector<point2<Scalar> > _bezierOut;

    bool _activateWarp;
    double _rigiditeTPS;
    std::size_t _nbPoints;
    double _transition;

    EParamMethod _method;
};

/**
 * @brief Warp plugin
 */
class WarpPlugin : public OFX::ImageEffect
{

public:
    typedef double Scalar;
    typedef boost::gil::point2<Scalar> Point2;

public:
    WarpPlugin(OfxImageEffectHandle handle);

public:
    WarpProcessParams<Scalar> getProcessParams(const OfxPointD& renderScale = OFX::kNoRenderScale) const;

    void changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName);

    //	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
    //	void getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );
    bool isIdentity(const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime);

    void render(const OFX::RenderArguments& args);

public:
    OFX::Clip* _clipSrc;  ///< Source image clip
    OFX::Clip* _clipSrcB; ///< Source image clip
    OFX::Clip* _clipDst;  ///< Destination image clip

    OFX::BooleanParam* _paramOverlay;
    OFX::BooleanParam* _paramInverse;
    OFX::PushButtonParam* _paramReset;
    OFX::PushButtonParam* _paramNextCurve;
    OFX::PushButtonParam* _paramSetKey;
    OFX::ChoiceParam* _paramMethod;

    OFX::IntParam* _paramNbPoints;
    OFX::DoubleParam* _transition;

    OFX::GroupParam* _paramGroupSettings;
    OFX::DoubleParam* _paramRigiditeTPS;
    OFX::IntParam* _paramNbPointsBezier;

    // In
    OFX::GroupParam* _paramGroupIn;
    boost::array<OFX::Double2DParam*, kMaxNbPoints> _paramPointIn;

    OFX::BooleanParam* _paramOverlayIn;
    OFX::RGBParam* _paramOverlayInColor;

    // Out
    OFX::GroupParam* _paramGroupOut;
    boost::array<OFX::Double2DParam*, kMaxNbPoints> _paramPointOut;

    OFX::BooleanParam* _paramOverlayOut;
    OFX::RGBParam* _paramOverlayOutColor;

    // Tgt In
    OFX::GroupParam* _paramGroupTgtIn;
    boost::array<OFX::Double2DParam*, 2 * kMaxNbPoints> _paramPointTgtIn;

    OFX::BooleanParam* _paramOverlayTgtIn;
    OFX::RGBParam* _paramOverlayTgtInColor;

    // Tgt Out
    OFX::GroupParam* _paramGroupTgtOut;
    boost::array<OFX::Double2DParam*, 2 * kMaxNbPoints> _paramPointTgtOut;

    OFX::BooleanParam* _paramOverlayTgtOut;
    OFX::RGBParam* _paramOverlayTgtOutColor;

    OFX::GroupParam* _paramGroupCurveBegin;
    boost::array<OFX::BooleanParam*, kMaxNbPoints> _paramCurveBegin;

private:
    OFX::InstanceChangedArgs _instanceChangedArgs;
};
}
}
}

#endif
