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
	
typedef boost::multi_array<unsigned char, 2> bool_2d; //for _imgBool (in functor or class)

/*
 *functor to compute selection histograms
 */
struct Pixel_compute_selection_histograms
{
	bool_2d _imgBool;				//bool selection img (pixels)
    HistogramBufferData _data;		//HistogramBufferData to fill up
	int _width;						//width of src clip
	int _height;					//height of src clip
	int _x, _y;						//position of the current pixel
	
	//basic round function
    double round(double x)
	{
		if(x>=0.5){return ceil(x);}else{return floor(x);}
	} 
    //operator ()
	template< typename Pixel>
    Pixel operator()( const Pixel& p )
    {
        using namespace boost::gil;
		if(_imgBool[_y][_x]) //if current pixel is selected
		{
			int indice;
			double val;
			hsl32f_pixel_t hsl_pix;			//needed to work in HSL (entry is RGBA)
			rgba32f_pixel_t pix;
			
			color_convert( p, pix );		//convert input to RGBA
			color_convert( pix, hsl_pix );	//convert RGBA tp HSL

			//RGBA
			for( int v = 0; v < boost::gil::num_channels<Pixel>::type::value; ++v )
			{   
				val = p[v];          
				if(val >= 0 && val <= 1)
				{    
					double inter = round(val*(_data._step-1));
					indice = inter;
					if(v == 0)
						_data._bufferRed.at(indice) += 1;			//increments red buffer 
					else if(v == 1)
						_data._bufferGreen.at(indice) += 1;			//increments green buffer 
					else if(v == 2)
						_data._bufferBlue.at(indice) += 1;			//increments blue buffer 
					else if(v == 3)
						_data._bufferAlpha.at(indice) += 1;			//increments alpha buffer 
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
						_data._bufferHue.at(indice) += 1;			//increments hue buffer
					else if(v == 2)
						_data._bufferLightness.at(indice) += 1;		//increments saturation buffer
					else if(v == 1)
						_data._bufferSaturation.at(indice) += 1;	//increments lightness buffer
				}
			}
		}
		//Check pixel position
		++_x;
		if(_x == _width){_y++;_x = 0;}
        return p;
    }
};


class HistogramKeyerOverlay : public OFX::OverlayInteract
{
	typedef double Scalar;
	typedef OfxRGBColourF Color;
	
public:	
	/*Class arguments*/
	HistogramKeyerPlugin* _plugin;							//plugin reference
	OfxPointI _size;										//source clip size
	interact::InteractInfos _infos;				
	HistogramKeyerHistogramDisplay _histogramDisplay;		//histogram display to delegate histogram display
	bool_2d _imgBool;										//unsigned char 2D (use for display texture on screen)
	
	bool _penDown;											//is mouse under selection
	bool _keyDown;											//is key under selection
	OfxPointI _origin;										//begin of the current selection
	OfxPointI _end;											//end of the current selection


	/*Creator*/
	HistogramKeyerOverlay( OfxInteractHandle handle, OFX::ImageEffect* effect );
	
	//Draw main function
	bool draw( const OFX::DrawArgs& args );
	//Mouse management
	bool penDown( const OFX::PenArgs& args );
	bool penUp( const OFX::PenArgs& args );
	bool penMotion( const OFX::PenArgs& args );
	//Keyboard management
	bool keyDown( const OFX::KeyArgs& args );
	bool keyUp( const OFX::KeyArgs& args );
	//Selection help (display)
	void displaySelectedAreas();
	void displaySelectionZone();
	
	//compute (refresh)
	void computeSelectionHistograms(const OfxTime time, const OfxPointD renderScale);
	void computeAverage();
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
