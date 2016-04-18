#include "HistogramPlugin.hpp"

#include <boost/gil/gil_all.hpp>

#include <boost/smart_ptr/scoped_array.hpp>

namespace tuttle
{
namespace plugin
{
namespace histogram
{

/*
 * fecth plugin IHM parameters with plugin parameters pointer
 */
HistogramPlugin::HistogramPlugin(OfxImageEffectHandle handle)
    : ImageEffectGilPlugin(handle)
{
    _paramGlobalDisplaySelection = fetchBooleanParam(kGlobalDisplay); // global display

    _paramOverlayRSelection = fetchBooleanParam(kBoolRed);   // R (is channel selected?)
    _paramMutliplierR = fetchDoubleParam(kMultiplierRed);    // R multiplier
    _paramOverlayGSelection = fetchBooleanParam(kBoolGreen); // G (is channel selected?)
    _paramMutliplierG = fetchDoubleParam(kMultiplierGreen);  // G multiplier

    _paramOverlayBSelection = fetchBooleanParam(kBoolBlue); // B (is channel selected?)
    _paramMutliplierB = fetchDoubleParam(kMultiplierBlue);  // B multiplier

    _paramOverlayHSelection = fetchBooleanParam(kBoolHue);        // H (is channel selected?)
    _paramMutliplierH = fetchDoubleParam(kMultiplierHue);         // H multiplier
    _paramOverlaySSelection = fetchBooleanParam(kBoolSaturation); // S (is channel selected?)
    _paramMutliplierS = fetchDoubleParam(kMultiplierSaturation);  // S multiplier

    _paramOverlayLSelection = fetchBooleanParam(kBoolLightness); // L (is channel selected?)
    _paramMutliplierL = fetchDoubleParam(kMultiplierLightness);  // L multiplier

    _paramDisplayTypeSelection =
        fetchChoiceParam(kHistoDisplayListParamLabel); // histogram display list (Histogram overlay group)

    _paramDisplaySelection = fetchBooleanParam(kBoolSelection); // display selection on source clip (Selection group)
    _paramSelectionMode = fetchChoiceParam(kSelectionModeListParamLabel); // selection mode (Selection group)

    _paramSelectionMultiplierSelection = fetchDoubleParam(kselectionMultiplier); // selection multiplier (Advanced group)
    _paramRefreshOverlaySelection = fetchPushButtonParam(kButtonRefreshOverlay); // refresh overlay (Advanced group)
    _paramNbStepSelection = fetchIntParam(knbStepRange);                         // nb step range (Advanced group)

    // Reset param booleans
    _isCleaned = false;
    _isNbStepChanged = false;
    _isHistogramRefresh = false;

    // Initialize scoped pointer
    _overlayDataCount = 0;

    /// @todo: HACK to display curves default position
    OFX::InstanceChangedArgs changed(this->timeLineGetTime());
    changedClip(changed, kOfxImageEffectSimpleSourceClipName);
}

/*
 * create and return HistogramProcessParams structure for process
 */
HistogramProcessParams<HistogramPlugin::Scalar> HistogramPlugin::getProcessParams(const OfxTime time,
                                                                                  const OfxPointD& renderScale) const
{
    HistogramProcessParams<Scalar> params;

    params._time = time;
    params._boolRGB[0] = _paramOverlayRSelection; // R (is channel selected?)
    params._boolRGB[1] = _paramOverlayGSelection; // G (is channel selected?)
    params._boolRGB[2] = _paramOverlayBSelection; // B (is channel selected?)

    params._boolHSL[0] = _paramOverlayHSelection; // H (is channel selected?)
    params._boolHSL[1] = _paramOverlaySSelection; // S (is channel selected?)
    params._boolHSL[2] = _paramOverlayLSelection; // L (is channel selected?)

    return params;
}

/*
 * a plugin parameter as been changed
 */
void HistogramPlugin::changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName)
{
    // refresh histogram overlay
    if(paramName == kButtonRefreshOverlay)
    {
        // Draw forced
        OFX::InstanceChangedArgs changed(args.time, args.renderScale);
        this->changedClip(changed, this->_clipSrc->name());
    }
    // nbStep changed
    else if(paramName == knbStepRange)
    {
        if(this->hasOverlayData()) // if there is overlay value
        {
            getOverlayData().setNbStep(_paramNbStepSelection->getValue()); // change nbStep value
            // getOverlayData( ).computeFullData( this->_clipSrc, args.time, args.renderScale ); //reset buffer and compute
            // them
            getOverlayData()._isDataInvalid = true;
        }
    }
    // Clear user selection
    else if(paramName == kButtonResetSelection)
    {
        if(this->hasOverlayData()) // if there is overlay value
        {
            this->getOverlayData().clearSelection(); // clear selection
        }
    }
}

/*
 * the source clip has been changed
 */
void HistogramPlugin::changedClip(const OFX::InstanceChangedArgs& args, const std::string& clipName)
{
    if(clipName == kOfxImageEffectSimpleSourceClipName)
    {
        if(this->hasOverlayData())
        {
            this->getOverlayData()._isDataInvalid = true;
            this->redrawOverlays();
        }
    }
}

bool HistogramPlugin::isIdentity(const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime)
{
    /// @todo HACK: nuke doesn't call changedClip when the time is modified.
    // OFX::InstanceChangedArgs changed( args.time, args.renderScale );
    // this->changedClip(changed,this->_clipSrc->name());
    //	HistogramProcessParams<Scalar> params = getProcessParams();

    //	identityClip = this->_clipSrc;
    //	identityTime = args.time;
    //	return true;
    return false;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void HistogramPlugin::render(const OFX::RenderArguments& args)
{
    if(!_clipSrc->isConnected())
        BOOST_THROW_EXCEPTION(exception::ImageNotConnected());

    boost::scoped_ptr<OFX::Image> src(_clipSrc->fetchImage(args.time));
    if(!src.get())
        BOOST_THROW_EXCEPTION(exception::ImageNotReady() << exception::dev() + "Error on clip " + quotes(_clipSrc->name())
                                                         << exception::time(args.time));
    if(src->getRowDistanceBytes() == 0)
        BOOST_THROW_EXCEPTION(exception::WrongRowBytes() << exception::dev() + "Error on clip " + quotes(_clipSrc->name())
                                                         << exception::time(args.time));

    boost::scoped_ptr<OFX::Image> dst(_clipDst->fetchImage(args.time));
    if(!dst.get())
        BOOST_THROW_EXCEPTION(exception::ImageNotReady() << exception::dev() + "Error on clip " + quotes(_clipDst->name())
                                                         << exception::time(args.time));
    if(dst->getRowDistanceBytes() == 0)
        BOOST_THROW_EXCEPTION(exception::WrongRowBytes() << exception::dev() + "Error on clip " + quotes(_clipDst->name())
                                                         << exception::time(args.time));

    setIsRendering(true); // plugin is rendering

    // Copy buffer
    const OfxRectI bounds = dst->getBounds();
    if(src->isLinearBuffer() && dst->isLinearBuffer())
    {
        const std::size_t imageDataBytes = dst->getBoundsImageDataBytes();
        if(imageDataBytes)
        {
            void* dataSrcPtr = src->getPixelAddress(bounds.x1, bounds.y1);
            void* dataDstPtr = dst->getPixelAddress(bounds.x1, bounds.y1);
            memcpy(dataDstPtr, dataSrcPtr, imageDataBytes);
        }
    }
    else
    {
        const std::size_t rowBytesToCopy = dst->getBoundsRowDataBytes();
        for(int y = bounds.y1; y < bounds.y2; ++y)
        {
            void* dataSrcPtr = src->getPixelAddress(bounds.x1, y);
            void* dataDstPtr = dst->getPixelAddress(bounds.x1, y);
            memcpy(dataDstPtr, dataSrcPtr, rowBytesToCopy);
        }
    }

    setIsRendering(false); // plugin is not rendering anymore

    if(this->hasOverlayData())
    {
        this->getOverlayData()._isDataInvalid = true;
        this->redrawOverlays();
    }

    if(OFX::getImageEffectHostDescription()->hostName ==
       "uk.co.thefoundry.nuke") /// @todo: HACK Nuke doesn't call changeClip function when time is changed
    {
        if(getOverlayData().isCurrentTimeModified(args.time)) // if time is changed
        {
            this->redrawOverlays(); // redraw scene
        }
    }
}

/// @brief Overlay data
/// @{

/*Overlay data management*/
void HistogramPlugin::addRefOverlayData()
{
    if(_overlayDataCount == 0)
    {
        const OfxPointI imgSize = this->_clipSrc->getPixelRodSize(0); ///@todo set the correct time !
        _overlayData.reset(new OverlayData(imgSize, this->_paramNbStepSelection->getValue()));
    }
    ++_overlayDataCount;
}

void HistogramPlugin::releaseOverlayData()
{
    --_overlayDataCount;
    if(_overlayDataCount == 0)
    {
        _overlayData.reset(NULL);
    }
}

bool HistogramPlugin::hasOverlayData() const
{
    return _overlayDataCount != 0;
}

OverlayData& HistogramPlugin::getOverlayData()
{
    return *_overlayData.get();
}

const OverlayData& HistogramPlugin::getOverlayData() const
{
    return *_overlayData.get();
}
/// @}
}
}
}
