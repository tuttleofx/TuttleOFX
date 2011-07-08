#ifndef _TUTTLE_PLUGIN_HISTOGRAMKEYER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_HISTOGRAMKEYER_PLUGIN_HPP_

#include "OverlayData.hpp"

namespace tuttle {
namespace plugin {
namespace histogramKeyer {

template<typename Scalar>
/*
 * params needed by the process to compute final image (curves & time)
 */
struct HistogramKeyerProcessParams
{
    OFX::ParametricParam* _paramColorRGB;		//curve RGB
    OFX::ParametricParam* _paramColorHSL;		//curve HSL
    OfxTime _time;								//current time
	OFX::ChoiceParam* _paramOutputSetting;		//ouput display (BW/alpha)
	OFX::BooleanParam* _boolReverseMask;		//is mask revert
	std::vector<OFX::BooleanParam*> _boolRGB;	//check boxes RGB selection
	std::vector<OFX::BooleanParam*> _boolHSL;	//check boxes HSL selection
	
};

/**
 * @brief HistogramKeyer plugin
 */
class HistogramKeyerPlugin : public ImageEffectGilPlugin
{
public:	
	/*Class arguments*/
    typedef float Scalar;
	bool _isCleaned;								//ask overlay to clean scene
	bool _isNbStepChanged;							//do we have to recompute selection histograms (nbStep has changed)
	bool _isHistogramRefresh;						//do we have to recompute selection histograms (other reason)
	
	/*Plugin parameters*/	
	OFX::BooleanParam* _paramGlobalDisplaySelection;//global display 
	OFX::ParametricParam* _paramColorHSLSelection;	//curve HSL
    OFX::ParametricParam* _paramColorRGBSelection;	//curve	RGB
	
	OFX::BooleanParam* _paramOverlayRSelection;		//R
	OFX::BooleanParam* _paramOverlayGSelection;		//G
	OFX::BooleanParam* _paramOverlayBSelection;		//B
	OFX::PushButtonParam* _clearRGB;				//clean RGB button
	
	OFX::BooleanParam* _paramOverlayHSelection;		//H
	OFX::BooleanParam* _paramOverlaySSelection;		//S
	OFX::BooleanParam* _paramOverlayLSelection;		//L
	OFX::PushButtonParam* _clearHSL;				//clean HSL button
	
    OFX::ChoiceParam* _paramDisplayTypeSelection;	//display option list global/adapted to channel (Histogram overlay group)
	OFX::PushButtonParam* _clearAll;				//clear all button (Histogram overlay group)
	
	OFX::BooleanParam* _paramDisplaySelection;		//display selection on clip source (Selection group)
	
	OFX::IntParam* _nbStepSelection;				//step selection (Advanced group)
	OFX::DoubleParam* _selectionMultiplierSelection;//selection multiplier (Advanced group)
	OFX::PushButtonParam* _refreshOverlaySelection; //refresh overlay button (Advanced group)
	
	OFX::ChoiceParam* _paramOutputSettingSelection;	//output display list (BW/Alpha)
	OFX::BooleanParam* _paramReverseMaskSelection;	//revert mask check box
	
	/*Overlay data parameters*/
	boost::scoped_ptr<OverlayData> _overlayData;	//scoped pointer points the overlay data (or NULL)
	std::size_t _overlayDataCount;					//count (class calling scoped pointer)
	
	
	/*Creators*/
    HistogramKeyerPlugin( OfxImageEffectHandle handle );
	HistogramKeyerProcessParams<Scalar> getProcessParams( const OfxTime time, const OfxPointD& renderScale = OFX::kNoRenderScale ) const;
	
	/*Input function*/
    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );
    void changedClip( const OFX::InstanceChangedArgs& args, const std::string& clipName );
	
	/*Output function*/
	bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );
    void render( const OFX::RenderArguments &args );
	
	/*Overlay data functions*/
	void addRefOverlayData();					//add reference to overlay data
	void releaseOverlayData();					//release reference to overlay data
	bool hasOverlayData() const;				//is there overlay data ?
	OverlayData& getOverlayData();				//getter/setter
	const OverlayData& getOverlayData() const;	//const getter
};

}
}
}

#endif
