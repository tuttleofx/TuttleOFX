#include "HSLOverlay.hpp"

namespace tuttle {
namespace plugin {
namespace diffPlot {
	

/*Contructor when Nuke overlay works
HSLOverlay::HSLOverlay(OfxInteractHandle handle, OFX::ImageEffect* effect):OFX::OverlayInteract( handle ),_infos( effect )
{
	_plugin = static_cast<DiffPlotPlugin*>(_effect);
	_plugin->addRefOverlayData();
}*/

//temporary constructor
HSLOverlay::HSLOverlay(DiffPlotPlugin* plugin)
{
	_plugin = plugin;
	_plugin->addRefOverlayData();
}

/*Destructor*/
HSLOverlay::~HSLOverlay()
{
	_plugin->releaseOverlayData();
}

/**
 * Display of the RGB diffPlots data
 */
bool HSLOverlay::draw(const OFX::DrawArgs& args)
{	
	bool hasGridBeenDisplayed = false;	//grid has not been displayed yet
	
	//height of the window (image for test)
	//width of the window (image for test)
	OfxPointI size = _plugin->_clipSrc->getPixelRodSize(args.time);
    const double step = size.x / (double)(getOverlayData()._channelData._step -1);

	float selectionMultiplier = (float)_plugin->_paramSelectionMultiplierSelection->getValue();
	//Display on screen if specified (HSL)
	if(_plugin->_paramOverlayHSelection->getValue())			//HUE CHANNEL
	{
		if(!hasGridBeenDisplayed)	//if grid has not been displayed yet
		{
			displayGrid(size.y, size.x); //display grid on screen
			hasGridBeenDisplayed = true; //set grid has already been displayed to true
		}
		plotChannelMapping( getOverlayData()._channelData._bufferHue, getOverlayData()._selectionData._bufferHue, step, size.y, size.x, selectionMultiplier );
	}
	if(_plugin->_paramOverlaySSelection->getValue())			//SATURATION CHANNEL
	{
		if(!hasGridBeenDisplayed)	//if grid has not been displayed yet
		{
			displayGrid(size.y, size.x); //display grid on screen
			hasGridBeenDisplayed = true; //set grid has already been displayed to true
		}
		
		plotChannelMapping( getOverlayData()._channelData._bufferSaturation, getOverlayData()._selectionData._bufferSaturation, step, size.y, size.x, selectionMultiplier );
	}
	if(_plugin->_paramOverlayLSelection->getValue())			//LIGHTNESS CHANNEL
	{
		if(!hasGridBeenDisplayed)	//if grid has not been displayed yet
		{
			displayGrid(size.y, size.x); //display grid on screen
			hasGridBeenDisplayed = true; //set grid has already been displayed to true
		}

		plotChannelMapping( getOverlayData()._channelData._bufferLightness, getOverlayData()._selectionData._bufferLightness, step, size.y, size.x, selectionMultiplier );
	}
	return true;
}

/**
 * Permits to know if there is only one RGB channel selected
 * @return "R", "G", "B" if there is only one channel selected,  "none" or "more" else
 */
ESelectedChannelHSL HSLOverlay::getOnlyChannelSelectedHSL() const
{
	//get all of the checkbox states HSL
	bool H = _plugin->_paramOverlayHSelection->getValue();	//check box H value
	bool S = _plugin->_paramOverlaySSelection->getValue();	//check box S value
	bool L = _plugin->_paramOverlayLSelection->getValue();	//check box L value
	
	if(H||S||L)
	{
		if(H && !S && !L)
			return eSelectedChannelH;			//Only hue is selected
		if(!H && S && !L)
			return eSelectedChannelS;			//Only saturation is selected
		if(!H && !S && L)
			return eSelectedChannelL;			//Only lightness is selected
		return eSelectedChannelMoreHSL;			//More than one HSL channel are selected
	}
	return eSelectedChannelNoneHSL;				//None HSL channel is selected
}

/**
 * Get overlay data from plugin
 * @return 
 */
OverlayData& HSLOverlay::getOverlayData()
{
	return _plugin->getOverlayData();
}

}
}
}
