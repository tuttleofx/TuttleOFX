#ifndef _TUTTLE_PLUGIN_HISTOGRAMKEYER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_HISTOGRAMKEYER_PLUGIN_HPP_

#include "HistogramKeyerDefinitions.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

#include <boost/gil/extension/color/hsl.hpp>
#include <boost/array.hpp>

namespace tuttle {
namespace plugin {
namespace histogramKeyer {

//typedef std::map<double, double> Curve;

template<typename Scalar>

/*
 * Params needed by the process to compute final image (curves & time)
 */
struct HistogramKeyerProcessParams
{
    OFX::ParametricParam* _paramColorRGB;
    OFX::ParametricParam* _paramColorHSL;
    OfxTime _time;
	OFX::ChoiceParam* _paramOutputSetting;
	OFX::BooleanParam* _boolReverseMask;
	OFX::BooleanParam** _boolRGB; //checkboxes RGB selection
	OFX::BooleanParam** _boolHSL; //checkboxes HSL selection 
	
};

/*
 * structure of 7 buffers (contains histogram data)
 */
struct HistogramBufferData
{
	//step
	int _step;
	//RGB
	std::vector<Number> _bufferRed;
	std::vector<Number> _bufferGreen;
	std::vector<Number> _bufferBlue;
	///HLS
	std::vector<Number> _bufferHue;
	std::vector<Number> _bufferLightness;
	std::vector<Number> _bufferSaturation;
	//Alpha
	std::vector<Number> _bufferAlpha;
};

/*
 *Functor to create histograms
 * 
 * for each pixel, process on RGB first and HSL then.
 * for each channel value the relative buffer is incremented on the right id, 
 */
struct Pixel_increment_histogramData
{
    HistogramBufferData _data; 
    double round(double x)
	{
		if(x>=0.5){return ceil(x);}else{return floor(x);}
	} 
    template< typename Pixel>
    Pixel operator()( const Pixel& p )
    {
        using namespace boost::gil;
        
        hsl32f_pixel_t hsl_pix; //needed to work in HSL (entry is RGBA)
		rgba32f_pixel_t pix;
        color_convert( p, pix );
		color_convert( pix, hsl_pix );
        int indice;
        double val;
        //RGBA
        for( int v = 0; v < boost::gil::num_channels<Pixel>::type::value; ++v )
        {   
          val = p[v];          
          if(val >= 0 && val <= 1)
          {    
                double inter = round(val*(_data._step-1));
				indice = inter;
                if(v == 0)
                    _data._bufferRed.at(indice) += 1;
                else if(v == 1)
                    _data._bufferGreen.at(indice) += 1;
                else if(v == 2)
                    _data._bufferBlue.at(indice) += 1;
                else if(v == 3)
                    _data._bufferAlpha.at(indice) += 1;
          }
        }
        //HLS
        for(int v = 0; v < boost::gil::num_channels<hsl32f_pixel_t>::type::value; ++v )
        {
          val = hsl_pix[v];          
          if(val >= 0 && val <= 1)
          {    
                double inter = round(val*(_data._step-1));
				indice = inter;
                if(v == 0)
                    _data._bufferHue.at(indice) += 1;
                else if(v == 1)
                    _data._bufferLightness.at(indice) += 1;
                else if(v == 2)
                    _data._bufferSaturation.at(indice) += 1;
          }
        }
        return p;
    }
};


/**
 * @brief HistogramKeyer plugin
 */
class HistogramKeyerPlugin : public ImageEffectGilPlugin
{
	
public:
    typedef float Scalar;
	HistogramBufferData _data; // histogram data
	HistogramBufferData _selectionData; //selection histogram data

	bool _isCleaned;      // 
	bool _isNbStepChanged;// do we have to recompute selection histograms
	boost::gil::rgba32f_view_t _srcView;
	
public:
    HistogramKeyerPlugin( OfxImageEffectHandle handle );

public:
	HistogramKeyerProcessParams<Scalar> getProcessParams( const OfxTime time, const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

	void beginChanged( OFX::InstanceChangeReason reason );
    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );
    void changedClip( const OFX::InstanceChangedArgs& args, const std::string& clipName );

//	void getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );
	bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );

    void render( const OFX::RenderArguments &args );
	
public:
	//curves
	OFX::ParametricParam* _paramColorHSLSelection;
    OFX::ParametricParam* _paramColorRGBSelection;
	//list
    OFX::ChoiceParam* _paramDisplayTypeSelection;
	//checkboxes
	OFX::BooleanParam* _paramOverlayRSelection;		//R
	OFX::BooleanParam* _paramOverlayGSelection;		//G
	OFX::BooleanParam* _paramOverlayBSelection;		//B
	OFX::BooleanParam* _paramOverlayHSelection;		//H
	OFX::BooleanParam* _paramOverlaySSelection;		//S
	OFX::BooleanParam* _paramOverlayLSelection;		//L
	OFX::BooleanParam* _paramDisplaySelection;		// display selection on clip source
	OFX::BooleanParam* _paramReverseMaskSelection;	// reverse mask
	//groups
	OFX::GroupParam* _groupRGBSelection;
	OFX::GroupParam* _groupHSLSelection;
	//buttons
	OFX::PushButtonParam* _clearRGB;
	OFX::PushButtonParam* _clearHSL;
	OFX::PushButtonParam* _clearAll;
	OFX::PushButtonParam* _refreshOverlaySelection;
	//int range
	OFX::IntParam* _nbStepSelection;
	//double range
	OFX::DoubleParam* _selectionMultiplierSelection;
	//Output
	OFX::ChoiceParam* _paramOutputSettingSelection;
	
	void resetVectortoZero( std::vector<long>& v, const unsigned int size ) const;
	void resetHistogramBufferData( HistogramBufferData& toReset ) const;
	void correctHistogramBufferData(HistogramBufferData& toCorrect) const;
	void correctVector(std::vector<Number>& v) const;
};

}
}
}

#endif
