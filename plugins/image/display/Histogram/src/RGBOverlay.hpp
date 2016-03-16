#ifndef RGBOVERLAY_HPP
#define RGBOVERLAY_HPP

#include "HistogramPlugin.hpp"
#include "HistogramDisplay.hpp"

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/interact/interact.hpp>
#include <tuttle/plugin/interact/InteractInfos.hpp>

#include <ofxsImageEffect.h>
#include <ofxsInteract.h>

namespace tuttle
{
namespace plugin
{
namespace histogram
{

/*
 *Enum used to know which RGB channel are selected
 */
enum ESelectedChannelRGB
{
    eSelectedChannelMoreRGB, // More than one RGB channel are selected
    eSelectedChannelR, // Only red is selected
    eSelectedChannelG, // Only green is selected
    eSelectedChannelB, // Only blue is selected
    eSelectedChannelNoneRGB // None channel is selected
};

/// @todo class RGBOverlay : public OFX::OverlayInteract (when Nuke overlay works)
class RGBOverlay
{
public:
    /*Class arguments*/
    HistogramPlugin* _plugin; // plugin reference
    // interact::InteractInfos _infos; (when Nuke overlay works)
    bool _isGriddisplay; // has grid been displayed yet

    /*Creator*/
    // RGBOverlay(OfxInteractHandle handle, OFX::ImageEffect* effect); (when Nuke overlay works)
    RGBOverlay(HistogramPlugin* plugin); // temporary

    /*Destructor*/
    ~RGBOverlay();

    /*draw main function*/
    bool draw(const OFX::DrawArgs& args);

    /*get selected channels*/
    ESelectedChannelRGB getOnlyChannelSelectedRGB() const;

    /*Display grid on screen*/
    void displayGrid(float height, float width);

    /*get overlay data*/
    OverlayData& getOverlayData();
};
}
}
}

#endif /* RGBOVERLAY_HPP */
