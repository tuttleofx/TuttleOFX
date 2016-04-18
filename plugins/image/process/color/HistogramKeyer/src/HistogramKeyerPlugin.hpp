#ifndef _TUTTLE_PLUGIN_HISTOGRAMKEYER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_HISTOGRAMKEYER_PLUGIN_HPP_

#include "OverlayData.hpp"

namespace tuttle
{
namespace plugin
{
namespace histogramKeyer
{

/*
 * params needed by the process to compute final image (curves & time)
 */
template <typename Scalar>
struct HistogramKeyerProcessParams
{
    OFX::ParametricParam* _paramColorRGB;              // curve RGB
    OFX::ParametricParam* _paramColorHSL;              // curve HSL
    OfxTime _time;                                     // current time
    OFX::ChoiceParam* _paramOutputSetting;             // ouput display (BW/alpha)
    OFX::BooleanParam* _boolReverseMask;               // is mask revert
    boost::array<OFX::BooleanParam*, 3> _boolRGB;      // check boxes RGB selection
    boost::array<OFX::BooleanParam*, 3> _boolHSL;      // check boxes HSL selection
    boost::array<OFX::DoubleParam*, 3> _multiplierRGB; // multiplier RGB channels
    boost::array<OFX::DoubleParam*, 3> _multiplierHSL; // multiplier HSL channels
    OFX::BooleanParam* _boolClampCurveValues;          // clamp curve values (Advanced group)
};

/**
 * @brief HistogramKeyer plugin
 */
class HistogramKeyerPlugin : public ImageEffectGilPlugin
{
public:
    /*Class arguments*/
    typedef float Scalar;
    bool _isCleaned;          // ask overlay to Reset scene
    bool _isNbStepChanged;    // do we have to recompute selection histograms (nbStep has changed)
    bool _isHistogramRefresh; // do we have to recompute selection histograms (other reason)

    /*Plugin parameters*/
    OFX::BooleanParam* _paramGlobalDisplaySelection; // global display
    OFX::ParametricParam* _paramColorHSLSelection;   // curve HSL
    OFX::ParametricParam* _paramColorRGBSelection;   // curve	RGB

    OFX::BooleanParam* _paramOverlayRSelection; // R
    OFX::DoubleParam* _paramMutliplierR;        // multiplier R

    OFX::BooleanParam* _paramOverlayGSelection; // G
    OFX::DoubleParam* _paramMutliplierG;        // multiplier G

    OFX::BooleanParam* _paramOverlayBSelection; // B
    OFX::DoubleParam* _paramMutliplierB;        // multiplier B

    OFX::PushButtonParam* _resetRGB; // Reset RGB button

    OFX::BooleanParam* _paramOverlayHSelection; // H
    OFX::DoubleParam* _paramMutliplierH;        // multiplier H

    OFX::BooleanParam* _paramOverlaySSelection; // S
    OFX::DoubleParam* _paramMutliplierS;        // multiplier S

    OFX::BooleanParam* _paramOverlayLSelection; // L
    OFX::DoubleParam* _paramMutliplierL;        // multiplier L

    OFX::PushButtonParam* _resetHSL; // Reset HSL button

    OFX::ChoiceParam* _paramDisplayTypeSelection; // display option list global/adapted to channel (Histogram overlay group)
    OFX::PushButtonParam* _paramResetAll;         // clear all button (Histogram overlay group)

    OFX::BooleanParam* _paramDisplaySelection; // display selection on clip source (Selection group)
    OFX::IntParam* _paramSelectionFromCurve;   // precision of selection to curve (Selection group)
    OFX::ChoiceParam* _paramSelectionMode;     // selection mode unique/additive/subtractive (Selection group)

    OFX::IntParam* _paramNbStepSelection;                 // step selection (Advanced group)
    OFX::DoubleParam* _paramSelectionMultiplierSelection; // selection multiplier (Advanced group)
    OFX::PushButtonParam* _paramRefreshOverlaySelection;  // refresh overlay button (Advanced group)
    OFX::BooleanParam* _paramClampCurveValues;            // clamp curve values (Advanced group)

    OFX::ChoiceParam* _paramOutputSettingSelection; // output display list (BW/Alpha)
    OFX::BooleanParam* _paramReverseMaskSelection;  // revert mask check box

    OFX::PushButtonParam* _paramButtonAppendToSelectionHSL; // append to selection HSL - Push button
    OFX::PushButtonParam* _paramButtonAppendToSelectionRGB; // append to selection RGB - Push button

    bool _isRendering; // is plugin rendering ? (if rendering don't modify data)

    /*Overlay data parameters*/
    boost::scoped_ptr<OverlayData> _overlayData; // scoped pointer points the overlay data (or NULL)
    std::size_t _overlayDataCount;               // count (class calling scoped pointer)

    /*Creators*/
    HistogramKeyerPlugin(OfxImageEffectHandle handle);
    HistogramKeyerProcessParams<Scalar> getProcessParams(const OfxTime time,
                                                         const OfxPointD& renderScale = OFX::kNoRenderScale) const;

    /*Input function*/
    void changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName);
    void changedClip(const OFX::InstanceChangedArgs& args, const std::string& clipName);

    /*Output function*/
    bool isIdentity(const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime);
    void render(const OFX::RenderArguments& args);

    /*Overlay data functions*/
    void addRefOverlayData();                  // add reference to overlay data
    void releaseOverlayData();                 // release reference to overlay data
    bool hasOverlayData() const;               // is there overlay data ?
    OverlayData& getOverlayData();             // getter/setter
    const OverlayData& getOverlayData() const; // const getter
};
}
}
}

#endif
