#ifndef _TUTTLE_PLUGIN_HISTOGRAMKEYEROVERLAY_HPP_
#define _TUTTLE_PLUGIN_HISTOGRAMKEYEROVERLAY_HPP_

#include "HistogramKeyerPlugin.hpp"
#include "HistogramKeyerHistogramDisplay.hpp"


#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/interact/interact.hpp>
#include <tuttle/plugin/interact/InteractInfos.hpp>
#include "boost/multi_array.hpp"

#include <ofxsImageEffect.h>
#include <ofxsInteract.h>

namespace tuttle {
namespace plugin {
namespace histogramKeyer {
	
typedef boost::multi_array<bool, 2> bool_2d;

/*
 *Functor to compute selection histograms
 * 
 */
struct Pixel_compute_selection_histograms
{
	bool_2d _imgBool;				//bool selection img (pixels)
    HistogramBufferData _data;		//HistogramBufferData to fill up
	int _width;						//width of src clip
	int _height;					//height of src clip
	int _x, _y;						//position of the current pixel
	
    double round(double x)
	{
		if(x>=0.5){return ceil(x);}else{return floor(x);}
	} 
    
	template< typename Pixel>
    Pixel operator()( const Pixel& p )
    {
        using namespace boost::gil;
		
		int revert_y = (_height-1)-_y; // GIL and OFX don't have same repere (inverse Y)
		
		if(_imgBool[revert_y][_x]) // if current pixel is selected
		{
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
		}
		//Check pixel position
		++_x;
		if(_x == _width)
		{
			_y++;
			_x = 0;
		}
        return p;
    }
};


class HistogramKeyerOverlay : public OFX::OverlayInteract
{
typedef double Scalar;
typedef OfxRGBColourF Color;


HistogramKeyerPlugin* _plugin;
interact::InteractInfos _infos;
HistogramKeyerHistogramDisplay _histogramDisplay;
bool _penDown;
bool _keyDown;
OfxPointI _size;
OfxPointI _origin;
OfxPointI _end;
bool_2d _imgBool;


public:
	HistogramKeyerOverlay( OfxInteractHandle handle, OFX::ImageEffect* effect );

	bool draw( const OFX::DrawArgs& args );
	bool penDown( const OFX::PenArgs& args );
	bool penUp( const OFX::PenArgs& args );
	bool penMotion( const OFX::PenArgs& args );
	
	//key up
	bool keyDown( const OFX::KeyArgs& args );
	bool keyUp( const OFX::KeyArgs& args );
	//test
	void gainFocus( const OFX::FocusArgs& args );
	void loseFocus( const OFX::FocusArgs& args );
	
	void displaySelectedAreas(OfxPointD pixelScale);
	void displaySelectionZone();
	void computeSelectionHistograms(const OfxTime time, const OfxPointD renderScale);
};

class HistogramKeyerParamOverlayDescriptor : public OFX::ParamInteractDescriptor
{
public:
	OFX::Interact* createInstance( OfxInteractHandle handle, OFX::ImageEffect* effect )
	{
		return new HistogramKeyerOverlay( handle, effect );
	}
};

class HistogramKeyerOverlayDescriptor : public OFX::EffectOverlayDescriptor
{
public:
	OFX::Interact* createInstance( OfxInteractHandle handle, OFX::ImageEffect* effect )
	{
		return new HistogramKeyerOverlay( handle, effect );
	}
};

}
}
}

#endif
