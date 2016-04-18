#ifndef _TUTTLE_PLUGIN_HISTOGRAM_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_HISTOGRAM_PLUGIN_HPP_

#include "OverlayData.hpp"

#include <tuttle/common/atomic.hpp>

namespace tuttle
{
namespace plugin
{
namespace histogram
{

/*
 * params needed by the process to compute final image (curves & time)
 */
template <typename Scalar>
struct HistogramProcessParams
{
    OfxTime _time;                                // current time
    boost::array<OFX::BooleanParam*, 3> _boolRGB; // check boxes RGB selection
    boost::array<OFX::BooleanParam*, 3> _boolHSL; // check boxes HSL selection
};

/**
 * @brief Histogram plugin
 */
class HistogramPlugin : public ImageEffectGilPlugin
{
public:
    /*Class arguments*/
    typedef float Scalar;
    bool _isCleaned;          // ask overlay to Reset scene
    bool _isNbStepChanged;    // do we have to recompute selection histograms (nbStep has changed)
    bool _isHistogramRefresh; // do we have to recompute selection histograms (other reason)

    /*Plugin parameters*/
    OFX::BooleanParam* _paramGlobalDisplaySelection; // global display

    OFX::BooleanParam* _paramOverlayRSelection; // R
    OFX::DoubleParam* _paramMutliplierR;        // multiplier R

    OFX::BooleanParam* _paramOverlayGSelection; // G
    OFX::DoubleParam* _paramMutliplierG;        // multiplier G

    OFX::BooleanParam* _paramOverlayBSelection; // B
    OFX::DoubleParam* _paramMutliplierB;        // multiplier B

    OFX::BooleanParam* _paramOverlayHSelection; // H
    OFX::DoubleParam* _paramMutliplierH;        // multiplier H

    OFX::BooleanParam* _paramOverlaySSelection; // S
    OFX::DoubleParam* _paramMutliplierS;        // multiplier S

    OFX::BooleanParam* _paramOverlayLSelection; // L
    OFX::DoubleParam* _paramMutliplierL;        // multiplier L

    OFX::ChoiceParam* _paramDisplayTypeSelection; // display option list global/adapted to channel (Histogram overlay group)

    OFX::BooleanParam* _paramDisplaySelection; // display selection on clip source (Selection group)
    OFX::ChoiceParam* _paramSelectionMode;     // selection mode unique/additive/subtractive (Selection group)

    OFX::IntParam* _paramNbStepSelection;                 // step selection (Advanced group)
    OFX::DoubleParam* _paramSelectionMultiplierSelection; // selection multiplier (Advanced group)
    OFX::PushButtonParam* _paramRefreshOverlaySelection;  // refresh overlay button (Advanced group)

    /*Overlay data parameters*/
    boost::scoped_ptr<OverlayData> _overlayData; // scoped pointer points the overlay data (or NULL)
    std::size_t _overlayDataCount;               // count (class calling scoped pointer)

    inline bool getIsRendering() const { return _isRendering.load(boost::memory_order_relaxed); }
    inline void setIsRendering(const bool v) { _isRendering.store(v, boost::memory_order_relaxed); }

private:
    boost::atomic_bool _isRendering; // is plugin rendering ? (if rendering don't modify data)

public:
    HistogramPlugin(OfxImageEffectHandle handle);
    HistogramProcessParams<Scalar> getProcessParams(const OfxTime time,
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
