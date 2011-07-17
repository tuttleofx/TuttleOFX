#include "HSLOverlay.hpp"

namespace tuttle {
namespace plugin {
namespace histogramKeyer {
	

/*Contructor when Nuke overlay works
HSLOverlay::HSLOverlay(OfxInteractHandle handle, OFX::ImageEffect* effect):OFX::OverlayInteract( handle ),_infos( effect )
{
	_plugin = static_cast<HistogramKeyerPlugin*>(_effect);
	_plugin->addRefOverlayData();
}*/

//temporary constructor
HSLOverlay::HSLOverlay(HistogramKeyerPlugin* plugin)
{
	_plugin = plugin;
	_plugin->addRefOverlayData();
}
//temporary
HSLOverlay::HSLOverlay(){_plugin = NULL;};

/*Destructor*/
HSLOverlay::~HSLOverlay()
{
	_plugin->releaseOverlayData();
}

/**
 * Display of the RGB histograms data
 */
bool HSLOverlay::draw(const OFX::DrawArgs& args)
{	
	//height of the window (image for test)
	//width of the window (image for test)
	OfxPointI size = _plugin->_clipSrc->getPixelRodSize(args.time);
    const double step = size.x / (double)(getData()._data._step -1);
        
    double heightH, heightS,heightL;
    heightH = heightS = heightL = size.y;
    if(!(_plugin->_paramDisplayTypeSelection->getValue() == 1))
    {
		//get max of the three channels
        const Number maxH = *(std::max_element(getData()._data._bufferHue.begin(),getData()._data._bufferHue.end()));
        const Number maxS = *(std::max_element(getData()._data._bufferSaturation.begin(),getData()._data._bufferSaturation.end()));
        const Number maxL = *(std::max_element(getData()._data._bufferLightness.begin(),getData()._data._bufferLightness.end()));
            
		//Adapt maximal value (3 cases)
        if(maxH > maxS && maxH > maxL)				//H is the max
        {
			heightS = maxS*heightH/maxH;
            heightL = maxL*heightH/maxH;
        }
        else if(maxS > maxH && maxS > maxL)			//S is the max
        {
            heightH = maxH*heightS/maxS;
            heightL = maxL*heightS/maxS;
        }
        else										//L is the max
        {
            heightH = maxH*heightL/maxL;
            heightS = maxS*heightL/maxL;
        }
    }
	float selectionMultiplier = (float)_plugin->_paramSelectionMultiplierSelection->getValue();
	//Display on screen if specified (HSL)
	if(_plugin->_paramOverlayHSelection->getValue())			//HUE CHANNEL
	{
		displayASpecificHistogram(getData()._data._bufferHue,getData()._selectionData._bufferHue,step,heightH,size.x,redHisto,selectionMultiplier);
		if(getOnlyChannelSelectedHSL()==eSelectedChannelH)
		{
			displaySelectionPoints(getData()._selectionData._bufferHue,step,size.x,redHisto);				//selection points
			displayAverageBar(getData()._averageData._averageHue,averageHisto,size.x,size.y,step);			//average bar
			displayHueIndicator(size,kPrecisionHueIndicator);												//indicator
		}
	}
	if(_plugin->_paramOverlaySSelection->getValue())			//SATURATION CHANNEL
	{
		displayASpecificHistogram(getData()._data._bufferSaturation,getData()._selectionData._bufferSaturation,step,heightS,size.x,greenHisto,selectionMultiplier);
		if(getOnlyChannelSelectedHSL()==eSelectedChannelS)
		{
			displaySelectionPoints(getData()._selectionData._bufferSaturation,step,size.x,greenHisto);		//selection points
			displayAverageBar(getData()._averageData._averageSaturation,averageHisto,size.x,size.y,step);	//average bar
			displaySaturationIndicator(size);																//indicator
		}
	}
	if(_plugin->_paramOverlayLSelection->getValue())			//LIGHTNESS CHANNEL
	{
		displayASpecificHistogram(getData()._data._bufferLightness,getData()._selectionData._bufferLightness,step,heightL,size.x,blueHisto,selectionMultiplier);
		if(getOnlyChannelSelectedHSL()==eSelectedChannelL)
		{
			displaySelectionPoints(getData()._selectionData._bufferLightness,step,size.x,blueHisto);		//selection points
			displayAverageBar(getData()._averageData._averageLightness,averageHisto,size.x,size.y,step);	//average bar
			displayLightnessIndicator(size);																//indicator
		}
	}
	//Display border (separate from histograms to eliminate blending)
	if(_plugin->_paramOverlayHSelection->getValue())
		displayASpecificHistogramBorder(getData()._data._bufferHue,step,heightH,size.x,redHisto);			//H
	if(_plugin->_paramOverlaySSelection->getValue())
		displayASpecificHistogramBorder(getData()._data._bufferSaturation,step,heightS,size.x,greenHisto);	//S
	if(_plugin->_paramOverlayLSelection->getValue())
		displayASpecificHistogramBorder(getData()._data._bufferLightness,step,heightL,size.x,blueHisto);	//L
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
OverlayData& HSLOverlay::getData()
{
	return _plugin->getOverlayData();
}

}
}
}
