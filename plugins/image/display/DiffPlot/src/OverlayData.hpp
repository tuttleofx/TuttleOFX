#ifndef OVERLAYDATA_HPP
#define	OVERLAYDATA_HPP

#include "DiffPlotDefinitions.hpp"

#include <tuttle/plugin/memory/OfxAllocator.hpp>
#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

#include <boost/gil/extension/color/hsl.hpp>
#include <boost/multi_array.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/array.hpp>

namespace tuttle {
namespace plugin {
namespace diffPlot {

typedef long Number;
typedef boost::container::flat_set<Number> ValueSet;
typedef boost::container::flat_map<Number, ValueSet> DiffPlotMap;

struct DiffPlotBufferData
{
	//step
	int _step;								//nbStep (for computing and display)
	//RGB
	DiffPlotMap _bufferRed;			//R
	DiffPlotMap _bufferGreen;		//G
	DiffPlotMap _bufferBlue;		//B
	///HLS
	DiffPlotMap _bufferHue;			//H
	DiffPlotMap _bufferLightness;	//S
	DiffPlotMap _bufferSaturation;	//L
};

/*
 *functor to compute selection diffPlots
 */
typedef boost::multi_array<unsigned char,2, OfxAllocator<unsigned char> > bool_2d;


struct Pixel_compute_diffPlots
{
    DiffPlotBufferData& _data;		//DiffPlotBufferData to fill up
	bool_2d& _imgBool;				//bool selection img (pixels)
	std::ssize_t _height;			//height of src clip
	std::ssize_t _width;			//width of src clip
	std::ssize_t _y, _x;			//position of the current pixel (functor needs to know which pixel is it)
	bool _isSelectionMode;			//do we work on all of the pixels (normal diffPlots) or only on selection
	
	Pixel_compute_diffPlots( bool_2d& selection, DiffPlotBufferData& data, const bool isSelectionMode )
	: _data( data )
	, _imgBool( selection )
	, _height( _imgBool.shape()[0] )
	, _width( _imgBool.shape()[1] )
	, _y(0)
	, _x(0)
	, _isSelectionMode(isSelectionMode)
	{
	}
	
	//basic round function
    double round( const double x ) const
	{
		if( x >= 0.5 ) { return ceil(x); } else { return floor(x); }
	}
	
	template< typename Pixel>
    Pixel operator()( const Pixel& pixA, const Pixel& pixB )
    {
        using namespace boost::gil;
		
		BOOST_ASSERT( _y >= 0 );
		BOOST_ASSERT( _x >= 0 );
		BOOST_ASSERT( std::ssize_t(_imgBool.shape()[0]) > _y );
		BOOST_ASSERT( std::ssize_t(_imgBool.shape()[1]) > _x );
		
		bool ok = false;
		if(_isSelectionMode == false)
			ok = true;
		else if(_imgBool[_y][_x] && _isSelectionMode)
			ok = true;

		if(ok) //if current pixel is selected
		{
			// RGBA //
			rgba32f_pixel_t rgba_pixA;
			color_convert( pixA, rgba_pixA );  //convert input to RGBA

			rgba32f_pixel_t rgba_pixB;
			color_convert( pixB, rgba_pixB );
		
			for( int v = 0; v < boost::gil::num_channels<Pixel>::type::value; ++v )
			{
				const double channelA = rgba_pixA[v];          
				const double channelB = rgba_pixB[v];          
				if(channelA >= 0 && channelA <= 1 &&
				   channelB >= 0 && channelB <= 1)
				{
					const int channelA_step = int(round(channelA*(_data._step-1)));
					const int channelB_step = int(round(channelB*(_data._step-1)));
					
					// Add mapping entry: channelA_step -> channelB_step
					if( v == 0 )
						_data._bufferRed[channelA_step].insert(channelB_step);
					else if(v == 1)
						_data._bufferGreen[channelA_step].insert(channelB_step);
					else if(v == 2)
						_data._bufferBlue[channelA_step].insert(channelB_step);
				}
			}
			
			// HLS //
			hsl32f_pixel_t hsl_pixA;  //needed to work in HSL (entry is RGBA)
			color_convert( rgba_pixA, hsl_pixA );  //convert RGBA tp HSL

			hsl32f_pixel_t hsl_pixB;
			color_convert( rgba_pixB, hsl_pixB );
			for(int v = 0; v < boost::gil::num_channels<hsl32f_pixel_t>::type::value; ++v )
			{
				const double channelA = hsl_pixA[v];          
				const double channelB = hsl_pixB[v];          
				if(channelA >= 0 && channelA <= 1 &&
				   channelB >= 0 && channelB <= 1)         
				if(channelA >= 0 && channelA <= 1)
				{
					const int channelA_step = int(round(channelA*(_data._step-1)));
					const int channelB_step = int(round(channelB*(_data._step-1)));
					
					// Add mapping entry: channelA_step -> channelB_step
					if( v == 0 )
						_data._bufferHue[channelA_step].insert(channelB_step);
					else if(v == 1)
						_data._bufferLightness[channelA_step].insert(channelB_step);
					else if(v == 2)
						_data._bufferSaturation[channelA_step].insert(channelB_step);
				}
			}
		}
		
		//Check pixel position
		++_x;
		if(_x == _width){++_y;_x=0;}
        return pixA;
    }
};

class OverlayData 
{
public:
	typedef boost::gil::rgba32f_view_t SView; // declare current view type
	
public:
	OverlayData( const OfxPointI& size, const int nbSteps);
	
	/** 
	 * reset selection data (button clear selection)
	 */
	void clearSelection()
	{
		resetDiffPlotSelectionData();
		removeSelection();
	}
	void clearAll( const OfxPointI& size )
	{
		_size = size;
		resetDiffPlotData();
		resetDiffPlotSelectionData();
		removeSelection();
	}

	/**
	 * Image size checker
	 * @warning HACK changeClip method doesn't work in nuke when time of source clip is changed so we have to check size of imgBool all the time
	 */
	bool isImageSizeModified( const OfxPointI& size ) const;
	
	/**
	 * DiffPlot computing
	 */
	void computeFullData( OFX::Clip* clipSrcA, OFX::Clip* clipSrcB, const OfxTime time, const OfxPointD& renderScale, const bool selectionOnly = false );			//compute full data (average/selection/diffPlots)
	void setNbStep( const std::size_t nbStep ) { _vNbStep = nbStep; }
	
	/**
	 * Current time checker
     */
	bool isCurrentTimeModified(const OfxTime time) const;
	
private:
	/*DiffPlot management*/
	void computeDiffPlotBufferData( DiffPlotBufferData& data, SView& srcAView, SView& srcBView, const OfxTime time, const bool isSelection);

	void resetDiffPlotBufferData( DiffPlotBufferData& toReset ) const;		//reset a complete DiffPlotBufferData

	/*Average management*/
	void computeAverages();		//compute average of each channel

	/*Reset data*/
	void resetDiffPlotData(); //reset data (if size change for example)
	
	void resetDiffPlotSelectionData();	//reset selection data
	void removeSelection();
	
	void resetVectortoZero( DiffPlotMap& v, const std::size_t size ) const;	//reset a specific channel buffer
	
public:
	DiffPlotBufferData _data;				//diffPlot data
	DiffPlotBufferData _selectionData;		//selection diffPlot data

	bool_2d _imgBool;						//unsigned char 2D (use for display texture on screen)
	OfxTime _currentTime;					//time of the current frame
	std::size_t _vNbStep;					//nbStep for buffers
	bool _isComputing;
	
	bool _isDataInvalid;
	
private:
	OfxPointI _size;						//source clip size
	
};

}
}
}

#endif

