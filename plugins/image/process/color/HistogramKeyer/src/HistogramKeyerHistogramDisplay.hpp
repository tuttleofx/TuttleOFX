#ifndef HISTOGRAMKEYERHISTOGRAMDISPLAY_HPP
#define	HISTOGRAMKEYERHISTOGRAMDISPLAY_HPP

//#include <boost/gil/color_convert.hpp>
#include "HistogramKeyerDefinitions.hpp"
#include "HistogramKeyerPlugin.hpp"


namespace tuttle {
namespace plugin {
namespace histogramKeyer {
	
typedef OfxRGBColourF Color;

//Histograms color (struct)
struct HistogramColor
{
	 OfxRGBColourF _colorBorder; // border color
	 OfxRGBColourF _colorFill;   // filling color
};

//Histograms colors
static const OfxRGBColourF redFilling={1.0f,0.2f,0.2f};
static const OfxRGBColourF greenFilling={0.2f,1.0f,0.2f};
static const OfxRGBColourF blueFilling={0.2f,0.2f,1.0f};

static const OfxRGBColourF redBorder = {0.8f,0.0f,0.0f};
static const OfxRGBColourF greenBorder = {0.0f,0.8f,0.0f};
static const OfxRGBColourF blueBorder = {0.0f,0.0f,0.8f};

static const HistogramColor redHisto = {redBorder,redFilling};
static const HistogramColor greenHisto = {greenBorder,greenFilling};
static const HistogramColor blueHisto = {blueBorder,blueFilling};


class HistogramKeyerHistogramDisplay {
public:
	/***************************/
	bool _translateHSL; // temp for HSL histograms translation
	/***************************/
	
	HistogramKeyerPlugin* _plugin;
	bool _isAdaptedToHeight;

public:
	HistogramKeyerHistogramDisplay(HistogramKeyerPlugin* plugin);
	inline HistogramKeyerHistogramDisplay():_plugin(NULL),_isAdaptedToHeight(false){};
	void displayHistogramOnScreenRGB( const HistogramBufferData& histogramBuffer, const HistogramBufferData& histogramSelectionBuffer, const OfxTime time) const;
    void displayHistogramOnScreenHSL( const HistogramBufferData& histogramBuffer, const HistogramBufferData& histogramSelectionBuffer, const OfxTime time) const;
	void displayASpecificHistogram( const std::vector<Number>& v, const std::vector<Number>& selection_v, const double step, const double height, const double width, const HistogramColor color) const;
	void displayASpecificHistogramBorder(const std::vector<Number>& v, const double step, const double height, const double width, const HistogramColor color) const;
	void displaySelectionPoints(const std::vector<Number>& selection_v, const double step, const double width, const HistogramColor color) const;
	
private:
	void displayRedIndicator(const OfxPointI size) const;
	void displayGreenIndicator(const OfxPointI size) const;
	void displayBlueIndicator(const OfxPointI size) const;
	void displayLightnessIndicator(const OfxPointI size) const;
	void displaySaturationIndicator(const OfxPointI size) const;
	void displayHueIndicator(const OfxPointI size) const;
	
	std::string getOnlyChannelSelectedRGB()const;
	std::string getOnlyChannelSelectedHSL()const;
};
}
}
}
#endif	/* HISTOGRAMKEYERHISTOGRAMDISPLAY_HPP */