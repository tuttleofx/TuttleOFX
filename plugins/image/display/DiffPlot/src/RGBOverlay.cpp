#include "RGBOverlay.hpp"

namespace tuttle {
namespace plugin {
namespace diffPlot {
	

/*Contructor (when Nuke works)
RGBOverlay::RGBOverlay(OfxInteractHandle handle, OFX::ImageEffect* effect):OFX::OverlayInteract( handle ),_infos( effect )
{
	_plugin = static_cast<DiffPlotPlugin*>(_effect);
	_plugin->addRefOverlayData();
}*/

RGBOverlay::RGBOverlay(DiffPlotPlugin* plugin)
{
	_plugin = plugin;
	_plugin->addRefOverlayData();
}

RGBOverlay::~RGBOverlay()
{
	_plugin->releaseOverlayData();
}

/**
 * Display of the RGB diffPlots data
 */
bool RGBOverlay::draw(const OFX::DrawArgs& args)
{
	bool hasGridBeenDisplayed = false;
	//height of the window (image for test)
	//width of the window (image for test)
	const OfxPointI size = _plugin->_clipSrc->getPixelRodSize(args.time);
	const double step = size.x / (double)(getOverlayData()._data._step -1);
	
	float selectionMultiplier = (float)_plugin->_paramSelectionMultiplierSelection->getValue(); //get selection multiplier from plugin
	//display on screen if specified (RGB)
	
	if(_plugin->_paramOverlayRSelection->getValue())		//RED CHANNEL
	{
		if(!hasGridBeenDisplayed)		//if grid has not been already displayed
		{	
			displayGrid(size.y,size.x);	//display grid
			hasGridBeenDisplayed = true;		//set display grid to true
		}
		
		plotChannelMapping( getOverlayData()._data._bufferRed, getOverlayData()._selectionData._bufferRed, step, size.y, size.x, selectionMultiplier );
	}
	if(_plugin->_paramOverlayGSelection->getValue())		//GREEN CHANNEL
	{
		if(!hasGridBeenDisplayed)		//if grid has not been already displayed
		{	
			displayGrid(size.y,size.x);	//display grid
			hasGridBeenDisplayed = true;		//set display grid to true
		}
		
		plotChannelMapping( getOverlayData()._data._bufferGreen, getOverlayData()._selectionData._bufferGreen, step, size.y, size.x, selectionMultiplier );
	}
	if(_plugin->_paramOverlayBSelection->getValue())		//BLUE CHANNEL
	{
		if(!hasGridBeenDisplayed)		//if grid has not been already displayed
		{	
			displayGrid(size.y,size.x);	//display grid
			hasGridBeenDisplayed = true;		//set display grid to true
		}
		
		plotChannelMapping( getOverlayData()._data._bufferBlue, getOverlayData()._selectionData._bufferBlue, step, size.y, size.x, selectionMultiplier );
	}
	return true;
}

/**
 * Permits to know if there is only one RGB channel selected
 * @return "R", "G", "B" if there is only one channel selected,  "none" or "more" else
 */
ESelectedChannelRGB RGBOverlay::getOnlyChannelSelectedRGB() const
{
	//get all of the checkbox states RGB
	bool R = _plugin->_paramOverlayRSelection->getValue();	//check box R value
	bool G = _plugin->_paramOverlayGSelection->getValue();	//check box G value
	bool B = _plugin->_paramOverlayBSelection->getValue();	//check box B value
	
	if(R||G||B)
	{
		if(R && !G && !B)
			return eSelectedChannelR;			//Only red is selected
		if(!R && G && !B)
			return eSelectedChannelG;			//Only green is selected
		if(!R && !G && B)
			return eSelectedChannelB;			//Only blue is selected
		return eSelectedChannelMoreRGB;			//More than one RGB channel are selected
	}
	return eSelectedChannelNoneRGB;				//None channel is selected
}

/**
 * Get overlay data from plugin
 * @return 
 */
OverlayData& RGBOverlay::getOverlayData()
{
	return _plugin->getOverlayData();
}

}
}
}
