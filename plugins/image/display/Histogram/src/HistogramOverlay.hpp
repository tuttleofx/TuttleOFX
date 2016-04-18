#ifndef _TUTTLE_PLUGIN_HISTOGRAMOVERLAY_HPP_
#define _TUTTLE_PLUGIN_HISTOGRAMOVERLAY_HPP_

#include "HistogramPlugin.hpp"
#include "RGBOverlay.hpp"
#include "HSLOverlay.hpp"

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/interact/interact.hpp>
#include <tuttle/plugin/interact/InteractInfos.hpp>
#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

#include <ofxsImageEffect.h>
#include <ofxsInteract.h>

namespace tuttle
{
namespace plugin
{
namespace histogram
{

class HistogramOverlay : public OFX::OverlayInteract
{
    typedef double Scalar;
    typedef OfxRGBColourF Color;

public:
    /*Class arguments*/
    HistogramPlugin* _plugin; // plugin reference
    interact::InteractInfos _infos;

    bool _penDown;          // is mouse under selection
    bool _keyDown;          // is key under selection
    OfxPointI _origin;      // begin of the current selection
    OfxPointI _end;         // end of the current selection
    OfxPointI _squareBegin; // begin of square selection
    OfxPointI _squareEnd;   // end of square selection

    bool _isFirstTime; // temporary

    HSLOverlay _hslParam;
    RGBOverlay _rgbParam;

    /*Creator*/
    HistogramOverlay(OfxInteractHandle handle, OFX::ImageEffect* effect);
    ~HistogramOverlay();

    bool draw(const OFX::DrawArgs& args); // draw all of the component (main display)

    /*Mouse management*/
    bool penDown(const OFX::PenArgs& args);   // mouse/pen is clicking down
    bool penUp(const OFX::PenArgs& args);     // mouse/pen is releasing
    bool penMotion(const OFX::PenArgs& args); // mouse/pen is under motion

    /*Keyboard management*/
    bool keyDown(const OFX::KeyArgs& args); // Ctrl key is pressing down
    bool keyUp(const OFX::KeyArgs& args);   // Ctrl key is releasing

    /*Selection help (display)*/
    void displaySelectedAreas(const OfxPointI& fullImgSize, const OfxPointI& imgSize,
                              const OfxRectI& pixelRoD); // display the selected areas (texture)
    void displaySelectionZone();                         // display the current selection zone (white square)

    /*Get overlay data*/
    OverlayData& getOverlayData();

private:
    void drawWarning(const Ofx3DPointD& centerPoint, const double ratio); // draw a warning signal
};

class RGBParamOverlayDescriptor : public OFX::ParamInteractDescriptor
{
public:
    OFX::Interact* createInstance(OfxInteractHandle handle, OFX::ImageEffect* effect)
    {
        // return new RGBOverlay( handle, effect ); (when Nuke overlay works)
        return NULL; // temporary
    }
};

class HSLParamOverlayDescriptor : public OFX::ParamInteractDescriptor
{
public:
    OFX::Interact* createInstance(OfxInteractHandle handle, OFX::ImageEffect* effect)
    {
        // return new HSLOverlay( handle, effect ); (when Nuke overlay works)
        return NULL; // temporary
    }
};

class HistogramOverlayDescriptor : public OFX::EffectOverlayDescriptor
{
public:
    OFX::Interact* createInstance(OfxInteractHandle handle, OFX::ImageEffect* effect)
    {
        return new HistogramOverlay(handle, effect);
    }
};
}
}
}

#endif
