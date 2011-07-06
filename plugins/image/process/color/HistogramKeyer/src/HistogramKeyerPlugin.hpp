#ifndef _TUTTLE_PLUGIN_HISTOGRAMKEYER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_HISTOGRAMKEYER_PLUGIN_HPP_

#include "HistogramKeyerDefinitions.hpp"
#include <tuttle/plugin/ImageEffectGilPlugin.hpp>
#include <boost/gil/extension/color/hsl.hpp>
#include <boost/array.hpp>

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

/*
 * structure of 7 buffers (contains histogram data)
 */
struct HistogramBufferData
{
	//step
	int _step;								//nbStep (for computing and display)
	//RGB
	std::vector<Number> _bufferRed;			//R
	std::vector<Number> _bufferGreen;		//G
	std::vector<Number> _bufferBlue;		//B
	///HLS
	std::vector<Number> _bufferHue;			//H
	std::vector<Number> _bufferLightness;	//S
	std::vector<Number> _bufferSaturation;	//L
	//Alpha
	std::vector<Number> _bufferAlpha;		//alpha
};

/*
 * structure which contains selection average for each channel to display average bar
 */
struct AverageBarData
{
	//RGB
	int _averageRed;				//R
	int _averageGreen;				//G
	int _averageBlue;				//B
	//HSL
	int _averageHue;				//H
	int _averageSaturation;			//S
	int _averageLightness;			//L
};

/*
 * functor to create histograms
 * for each pixel, process on RGB first and HSL then.
 * for each channel value the relative buffer is incremented on the right id, 
 */
struct Pixel_increment_histogramData
{
	//functor data
    HistogramBufferData _data; 
	
	//basic function to round a double
    double round(double x)
	{
		if(x>=0.5){return ceil(x);}else{return floor(x);}
	}
	//operator called
    template< typename Pixel>
    Pixel operator()( const Pixel& p )
    {
        using namespace boost::gil;      
        hsl32f_pixel_t hsl_pix;										//needed to work in HSL (entry is RGBA)
		rgba32f_pixel_t pix;
		int indice;
        double val;
		
        color_convert( p, pix );									//convert input to RGBA
		color_convert( pix, hsl_pix );								//convert RGBA to HSL
        //RGBA
        for( int v = 0; v < boost::gil::num_channels<Pixel>::type::value; ++v )
        {   
          val = p[v];          
          if(val >= 0 && val <= 1)
          {    
                double inter = round(val*(_data._step-1));
				indice = inter;										//double to int 
                if(v == 0)
                    _data._bufferRed.at(indice) += 1;				//increments red buffer
                else if(v == 1)
                    _data._bufferGreen.at(indice) += 1;				//increments green buffer
                else if(v == 2)
                    _data._bufferBlue.at(indice) += 1;				//increments blue buffer
                else if(v == 3)
                    _data._bufferAlpha.at(indice) += 1;				//increments alpha buffer
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
                    _data._bufferHue.at(indice) += 1;				//increments hue buffer
                else if(v == 1)
                    _data._bufferLightness.at(indice) += 1;			//increments saturation buffer
                else if(v == 2)
                    _data._bufferSaturation.at(indice) += 1;		//increments lightness buffer
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
	/*Class arguments*/
    typedef float Scalar;
	HistogramBufferData _data;						//histogram data
	HistogramBufferData _selectionData;				//selection histogram data
	AverageBarData _averageData;					//average bar data used to display average bars
	bool _isCleaned;								//ask overlay to clean scene
	bool _isNbStepChanged;							//do we have to recompute selection histograms
	
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
	
    OFX::ChoiceParam* _paramDisplayTypeSelection;	//display option list global/adatped to channel (Histogram overlay group)
	OFX::PushButtonParam* _clearAll;				//clear all button (Histogram overlay group)
	
	OFX::BooleanParam* _paramDisplaySelection;		//display selection on clip source (Selection group)
	
	OFX::IntParam* _nbStepSelection;				//step selection (Advanced group)
	OFX::DoubleParam* _selectionMultiplierSelection;//selection multiplier (Advanced group)
	OFX::PushButtonParam* _refreshOverlaySelection; //refresh overlay button (Advanced group)
	
	OFX::ChoiceParam* _paramOutputSettingSelection;	//output display list (BW/Alpha)
	OFX::BooleanParam* _paramReverseMaskSelection;	//revert mask check box
	
	
	/*Creators*/
    HistogramKeyerPlugin( OfxImageEffectHandle handle );
	HistogramKeyerProcessParams<Scalar> getProcessParams( const OfxTime time, const OfxPointD& renderScale = OFX::kNoRenderScale ) const;
	/*Input function*/
    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );
    void changedClip( const OFX::InstanceChangedArgs& args, const std::string& clipName );
	/*Output function*/
	bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );
    void render( const OFX::RenderArguments &args );
	
	/*Histogram management functions*/
	void resetVectortoZero( std::vector<long>& v, const unsigned int size ) const;
	void resetHistogramBufferData( HistogramBufferData& toReset ) const;
	void correctHistogramBufferData(HistogramBufferData& toCorrect) const;
	void correctVector(std::vector<Number>& v) const;
};

}
}
}

#endif
