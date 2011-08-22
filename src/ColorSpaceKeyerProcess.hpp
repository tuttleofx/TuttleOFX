#ifndef _TUTTLE_PLUGIN_COLORSPACEKEYER_PROCESS_HPP_
#define _TUTTLE_PLUGIN_COLORSPACEKEYER_PROCESS_HPP_

#include "SelectionAverage.hpp"
#include "GeodesicForm.hpp"
#include <tuttle/plugin/ImageGilFilterProcessor.hpp>

namespace tuttle {
namespace plugin {
namespace colorSpaceKeyer {

	
struct Compute_alpha_pixel
{ 
	bool _isOutputBW; // is output black & white (or alpha channel)
	GeodesicForm& _data; //Geodesic form
	
	Compute_alpha_pixel(bool isOutputBW, GeodesicForm& data):
	_isOutputBW(isOutputBW),
	_data(data)
	{		
	}
	
    template< typename Pixel>
    Pixel operator()( const Pixel& p )
    {
        using namespace boost::gil;
		
        double alpha = 0.0; //define current pixel alpha (1 by default)
		Ofx3DPointD testPoint; //initialize test point 
		testPoint.x = p[0]; //x == red
		testPoint.y = p[1]; //y == green
		testPoint.z = p[2]; //z == blue
		
		if(_data.isPointIntoGeodesicForm(testPoint)) //if current pixel is into the geodesic form
		{
			alpha = 1.0; //change alpha to 0
		}
		//else
			//std::cout << "passe aussi"<<std::endl;
		
		Pixel ret; //declare returned pixel
		if(_isOutputBW) // output is gray scale image
		{
			gray32f_pixel_t inter;		// need a gray_pixel
			inter[0] = alpha;			// recopy value (black or white)
			color_convert(inter,ret);	// convert gray_pixel to rgba_pixel
		}
		else // output is alpha channel
		{
			for(unsigned int i=0; i<boost::gil::num_channels<Pixel>::type::value -1; ++i) // All RGB channels (not alpha)
				ret[i] = p[i]; //recopy r,g and b channels
			ret[3] = alpha; //fill alpha channel up
		}
		return ret;
    }
};	
	
/**
 * @brief ColorSpaceKeyer process
 *
 */
template<class View>
class ColorSpaceKeyerProcess : public ImageGilFilterProcessor<View>
{
public:
	typedef typename View::value_type Pixel;
	typedef typename boost::gil::channel_type<View>::type Channel;
	typedef float Scalar;
protected:
    ColorSpaceKeyerPlugin&    _plugin;            ///< Rendering plugin
	ColorSpaceKeyerProcessParams<Scalar> _params; ///< parameters

public:
    ColorSpaceKeyerProcess( ColorSpaceKeyerPlugin& effect );

	void setup( const OFX::RenderArguments& args );

    void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}

#include "ColorSpaceKeyerProcess.tcc"

#endif
