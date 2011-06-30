#ifndef _TUTTLE_PLUGIN_HISTOGRAMKEYER_PROCESS_HPP_
#define _TUTTLE_PLUGIN_HISTOGRAMKEYER_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>

namespace tuttle {
namespace plugin {
namespace histogramKeyer {

/**
 * @brief HistogramKeyer process
 *
 */
    
/**
 *Functor which compute alpha for each pixel
 */
struct Compute_alpha_pixel
{ 
	bool _isOutputBW; // is output black & white (or alpha channel)
    HistogramKeyerProcessParams<float> _params;
	
    template< typename Pixel>
    Pixel operator()( const Pixel& p )
    {
        using namespace boost::gil;
        
        hsl32f_pixel_t hsl_pix;
        rgb32f_pixel_t convert_to_rgb;
        color_convert( p, convert_to_rgb ); // first step : p to rgb_pixel
        color_convert(convert_to_rgb,hsl_pix ); // second step : rgb_pixel to hsl_pixel
        
        double alpha = 1.0;

        //RGBA
        for( int v = 0; v < boost::gil::num_channels<Pixel>::type::value -1; ++v ) // RGB but not alpha (doesn't needed)
        {
			if(_params._boolRGB[v]->getValue()) //if current channel check-box is active
				alpha *=  _params._paramColorRGB->getValue( v, _params._time,p[v] );
        }
        //HSL
        for( int v = 0; v < boost::gil::num_channels<hsl32f_pixel_t>::type::value; ++v )
        {
			if(_params._boolHSL[v]->getValue()) //if current channel check-box is active
				alpha *=  _params._paramColorHSL->getValue( v, _params._time,hsl_pix[v] );
        }
		Pixel ret;
		if(_params._boolReverseMask->getValue()) //revert mask
			alpha = 1-alpha;
		
		if(_isOutputBW) // output is gray scale image
		{
			gray32f_pixel_t inter; // need a gray_pixel
			inter[0] = alpha;
			color_convert(inter,ret); // convert gray_pixel to rgba_pixel
		}
		else // output is alpha channel
		{
			for(unsigned int i=0; i<boost::gil::num_channels<Pixel>::type::value -1; ++i)
				ret[i] = p[i];
			ret[3] = alpha; //fill alpha channel up
		}
		return ret;
    }
};

template<class View>
class HistogramKeyerProcess : public ImageGilFilterProcessor<View>
{
public:
	typedef typename View::value_type Pixel;
	typedef typename boost::gil::channel_type<View>::type Channel;
	typedef float Scalar;
protected:
    HistogramKeyerPlugin&    _plugin;            ///< Rendering plugin
	HistogramKeyerProcessParams<Scalar> _params; ///< parameters

public:
    HistogramKeyerProcess( HistogramKeyerPlugin& effect );

	void setup( const OFX::RenderArguments& args );

    void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}

#include "HistogramKeyerProcess.tcc"

#endif
