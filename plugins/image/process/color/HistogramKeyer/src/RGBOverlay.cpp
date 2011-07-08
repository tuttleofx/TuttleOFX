#include "RGBOverlay.hpp"

namespace tuttle {
namespace plugin {
namespace histogramKeyer {
	

/*Contructor (when Nuke works)
RGBOverlay::RGBOverlay(OfxInteractHandle handle, OFX::ImageEffect* effect):OFX::OverlayInteract( handle ),_infos( effect )
{
	_plugin = static_cast<HistogramKeyerPlugin*>(_effect);
	_plugin->addRefOverlayData();
}*/

//tomporary constructor
RGBOverlay::RGBOverlay(HistogramKeyerPlugin* plugin)
{
	_plugin = plugin;
	_plugin->addRefOverlayData();
}
//temporary
RGBOverlay::RGBOverlay(){_plugin = NULL;};

/*Destructor*/
RGBOverlay::~RGBOverlay()
{
	_plugin->releaseOverlayData();
}

/**
 * Display of the RGB histograms data
 */
bool RGBOverlay::draw(const OFX::DrawArgs& args)
{	
	//height of the window (image for test)
	//width of the window (image for test)
	OfxPointI size = _plugin->_clipSrc->getPixelRodSize(args.time);
    const double step = size.x / (double)(getData()._data._step -1);
	double heightR, heightG,heightB;
    heightR = heightG = heightB = size.y;
    if(!(_plugin->_paramDisplayTypeSelection->getValue() == 1))
    {
		//get max of the three channels
		Number maxR = *(std::max_element(getData()._data._bufferRed.begin(),getData()._data._bufferRed.end()));
        Number maxG = *(std::max_element(getData()._data._bufferGreen.begin(),getData()._data._bufferGreen.end()));
        Number maxB = *(std::max_element(getData()._data._bufferBlue.begin(),getData()._data._bufferBlue.end()));
		
        if(maxR > maxG && maxR > maxB)				//R is the max
        {
			heightG = maxG*heightR/maxR;
            heightB = maxB*heightR/maxR;
        }
        else if(maxG > maxR && maxG > maxB)			//G is the max
        {
            heightR = maxR*heightG/maxG;
            heightB = maxB*heightG/maxG;
        }
        else										//B is the max
        {	
            heightR = maxR*heightB/maxB;
            heightG = maxG*heightB/maxB;
        }
    }
	float selectionMultiplier = (float)_plugin->_selectionMultiplierSelection->getValue(); //get selection multiplier from plugin
	//display on screen if specified (RGB)
	
	if(_plugin->_paramOverlayRSelection->getValue())		//RED CHANNEL
	{
		displayASpecificHistogram(getData()._data._bufferRed,getData()._selectionData._bufferRed,step,heightR,size.x,redHisto,selectionMultiplier);
		if(getOnlyChannelSelectedRGB()==eSelectedChannelR)
		{
			displaySelectionPoints(getData()._selectionData._bufferRed,step,size.x,redHisto);					//selection points
			displayAverageBar(getData()._averageData._averageRed,averageHisto,size.x,size.y,step);		//average bar
			displayRedIndicator(size);																	//indicator
		}
	}
	if(_plugin->_paramOverlayGSelection->getValue())		//GREEN CHANNEL
	{
		displayASpecificHistogram(getData()._data._bufferGreen,getData()._selectionData._bufferGreen,step,heightG,size.x,greenHisto,selectionMultiplier);
		if(getOnlyChannelSelectedRGB()==eSelectedChannelG)
		{
			displaySelectionPoints(getData()._selectionData._bufferGreen,step,size.x,greenHisto);				//selection points
			displayAverageBar(getData()._averageData._averageGreen,averageHisto,size.x,size.y,step);	//average bar
			displayGreenIndicator(size);																//indicator
		}
	}
	if(_plugin->_paramOverlayBSelection->getValue())		//BLUE CHANNEL
	{
		displayASpecificHistogram(getData()._data._bufferBlue,getData()._selectionData._bufferBlue,step,heightB,size.x,blueHisto,selectionMultiplier);
		if(getOnlyChannelSelectedRGB()==eSelectedChannelB)
		{
			displaySelectionPoints(getData()._selectionData._bufferBlue,step,size.x,blueHisto);			//selection points
			displayAverageBar(getData()._averageData._averageBlue,averageHisto,size.x,size.y,step);				//average bar
			displayBlueIndicator(size);																	//indicator
		}
	}
	//Display border (separate from histograms to eliminate blending)
	if(_plugin->_paramOverlayRSelection->getValue())
		displayASpecificHistogramBorder(getData()._data._bufferRed,step,heightR,size.x,redHisto);		//R
	if(_plugin->_paramOverlayGSelection->getValue())
		displayASpecificHistogramBorder(getData()._data._bufferGreen,step,heightG,size.x,greenHisto);	//G
	if(_plugin->_paramOverlayBSelection->getValue())
		displayASpecificHistogramBorder(getData()._data._bufferBlue,step,heightB,size.x,blueHisto);		//B
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
OverlayData& RGBOverlay::getData()
{
	return _plugin->getOverlayData();
}

}
}
}