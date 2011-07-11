#ifndef OVERLAYDATA_HPP
#define	OVERLAYDATA_HPP
#include "HistogramKeyerDefinitions.hpp"
#include "boost/multi_array.hpp"
#include <boost/array.hpp>

namespace tuttle {
namespace plugin {
namespace histogramKeyer {
	
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
 *functor to compute selection histograms
 */
typedef boost::multi_array<unsigned char,2> bool_2d;


struct Pixel_compute_histograms
{
    HistogramBufferData _data;		//HistogramBufferData to fill up
	std::ssize_t _width;						//width of src clip
	std::ssize_t _height;					//height of src clip
	std::ssize_t _x, _y;						//position of the current pixel (functor needs to know which pixel is it)
	bool _isSelectionMode;			//do we work on all of the pixels (normal histograms)	
	const bool_2d& _imgBool;				//bool selection img (pixels)
	
	Pixel_compute_histograms( const bool_2d& selection )
	: _imgBool( selection )
	{}
	
	//basic round function
    double round(double x)
	{
		if(x>=0.5){return ceil(x);}else{return floor(x);}
	}
	
	template< typename Pixel>
    Pixel operator()( const Pixel& p )
    {
        using namespace boost::gil;
		bool ok = false;
		if(_isSelectionMode == false)
			ok = true;
		else if(_imgBool[_y][_x] && _isSelectionMode)
			ok = true;
		
		if(ok) //if current pixel is selected
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
		else
		{
			//std::cout << "passe pas !!" << std::endl;
		}
		//Check pixel position
		++_x;
		if(_x == _width){_y++;_x = 0;}
        return p;
    }
};

class OverlayData 
{
public:
	/*Class arguments*/
	HistogramBufferData _data;				//histogram data
	HistogramBufferData _selectionData;		//selection histogram data
	AverageBarData _averageData;			//average bar data used to display average bars
	OfxPointI _size;						//source clip size
	bool_2d _imgBool;						//unsigned char 2D (use for display texture on screen)
	std::size_t vNbStep;					//nbStep for buffers
	
	/*Creators*/
	OverlayData(const OfxPointI& size);
	
	/*Reset data*/
	void resetData( const OfxPointI& size );				//reset data (if size change for example)
	void resetSelectionData( const OfxPointI& size );	//reset selection data (button clear selection)
	
	/*Histogram computing*/
	void computeFullData(OFX::Clip* clipSrc,const OfxTime time, const OfxPointD renderScale);			//compute full data (average/selection/histograms)
	void computeHistogramBufferData(HistogramBufferData& data,OFX::Clip* clipSrc,const OfxTime time, const OfxPointD renderScale,bool isSelection=false);	//compute a HisogramBufferData
	
	/*Histogram management*/
	void resetHistogramBufferData( HistogramBufferData& toReset ) const;		//reset a complete HistogramBufferData
	void correctHistogramBufferData(HistogramBufferData& toCorrect) const;		//correct a complete HistogramBufferData
	
	/*Average management*/
	void computeAverages();		//compute average of each channel
	void resetAverages();		//rest all of the averages	
	
private:
	void correctVector(std::vector<Number>& v) const;								//correct a specific channel
	void resetVectortoZero(std::vector<long>& v, const unsigned int size) const;	//reset a specific channel buffer
	int computeAnAverage(std::vector<Number> selection_v)const;						//compute average of a specific channel
};

}
}
}

#endif

