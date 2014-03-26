#ifndef _TUTTLE_PLUGIN_COLORDATA_HPP
#define _TUTTLE_PLUGIN_COLORDATA_HPP

#include <tuttle/plugin/memory/OfxAllocator.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/array.hpp>
#include <boost/multi_array.hpp>
#include <boost/gil/extension/color/hsl.hpp>

namespace tuttle {
namespace plugin {
namespace diffPlot {

typedef long Number;

struct ColorValues
{
	boost::array<Number, 4> _rgba;
	bool operator<(const ColorValues & color) const
	{
		for (int i = 0; i < 4; ++i)
		{
			if( _rgba[i] == color._rgba[i])
				continue;
			return _rgba[i] < color._rgba[i];
		}
		return true; // They're equals
	}

	bool operator==(const ColorValues & color) const
	{
		for (int i = 0; i < 4; ++i)
		{
			if( _rgba[i] != color._rgba[i])
				return false;
		}
		return true;
	}
};

typedef boost::container::flat_set<ColorValues> ColorSet;
typedef boost::container::flat_map<ColorValues, ColorSet> ColorSetMap;

struct ColorData
{
	ColorSetMap _colorMapping;

	void clear()
	{
		_colorMapping.clear();
	}
};

typedef boost::multi_array<unsigned char,2, OfxAllocator<unsigned char> > bool_2d;
/*
 *functor to compute selection diffPlots
 */
struct ColorDiffPlotFunc
{
	ColorData& _data; //Color Data to fill up
	bool_2d& _imgBool; //bool selection img (pixels)
	std::ssize_t _height; //height of src clip
	std::ssize_t _width; //width of src clip
	std::ssize_t _y, _x; //position of the current pixel (functor needs to know which pixel is it)
	bool _isSelectionMode; //do we work on all of the pixels (normal diffPlots) or only on selection

	ColorDiffPlotFunc( bool_2d& selection, ColorData& data, const bool isSelectionMode )
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

			// TODO
//			rgba32f_pixel_t rgba_pixA;
//			color_convert( pixA, rgba_pixA );  //convert input to RGBA
//
//			rgba32f_pixel_t rgba_pixB;
//			color_convert( pixB, rgba_pixB );
//
//			// Convert to color
//			ColorValues colorA;
//			ColorValues colorB;
//			colorA._rgba = rgba_pixA;
//			colorB._rgba = rgba_pixB;
//
//			// step / round ???
//			_data._colorMapping[colorA].insert( colorB );
		}

		//Check pixel position
		++_x;
		if(_x == _width){++_y;_x=0;}
        return pixA;
    }
};

}
}
}

#endif
