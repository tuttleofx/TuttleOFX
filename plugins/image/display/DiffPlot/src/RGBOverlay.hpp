#ifndef RGBOVERLAY_HPP
#define	RGBOVERLAY_HPP

#include "DiffPlotPlugin.hpp"
#include "DiffPlotDisplay.hpp"

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/interact/interact.hpp>
#include <tuttle/plugin/interact/InteractInfos.hpp>

#include <ofxsImageEffect.h>
#include <ofxsInteract.h>

namespace tuttle {
namespace plugin {
namespace diffPlot {
	
/*
 *Enum used to know which RGB channel are selected
 */
enum ESelectedChannelRGB
{
	eSelectedChannelMoreRGB,		//More than one RGB channel are selected
	eSelectedChannelR,				//Only red is selected
	eSelectedChannelG,				//Only green is selected
	eSelectedChannelB,				//Only blue is selected
	eSelectedChannelNoneRGB			//None channel is selected
};

/// @todo class RGBOverlay : public OFX::OverlayInteract (when Nuke overlay works)
class RGBOverlay
{
public:
	/*Class arguments*/
	DiffPlotPlugin* _plugin;	//plugin reference
	// interact::InteractInfos _infos; (when Nuke overlay works)
		
	/*Creator*/
	//RGBOverlay(OfxInteractHandle handle, OFX::ImageEffect* effect); (when Nuke overlay works)
	RGBOverlay(DiffPlotPlugin* plugin);	//temporary
	
	/*Destructor*/
	~RGBOverlay();
	
	/*draw main function*/
	bool draw(const OFX::DrawArgs& args);
	bool drawByChannel(const OFX::DrawArgs& args);
	bool drawByColor(const OFX::DrawArgs& args);
	
	/*get selected channels*/
	ESelectedChannelRGB getOnlyChannelSelectedRGB()const;
	
	/*get overlay data*/
	OverlayData& getOverlayData();
};

}
}
}

#endif	/* RGBOVERLAY_HPP */

