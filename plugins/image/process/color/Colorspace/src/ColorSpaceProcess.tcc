#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <cstdlib>
#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <tuttle/plugin/image/gil/color.hpp>

namespace ttlc = tuttle::plugin::color;

namespace tuttle {
namespace plugin {
namespace colorspace {

using namespace boost::gil;

template<class View>
ColorSpaceProcess<View>::ColorSpaceProcess( ColorSpacePlugin& instance )
	: ImageGilFilterProcessor<View>( instance )
	, _plugin( instance )
{
}

template<class View>
void ColorSpaceProcess<View>::setup( const OFX::RenderArguments& args )
{
	//_gradationIn = _plugin.getGradationLawIn();
}


/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template<class View>
void ColorSpaceProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	OfxRectI procWindowOutput                      = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	rgba32f_pixel_t wpix;

	for( int y = procWindowOutput.y1;
	     y < procWindowOutput.y2;
	     ++y )
	{
		typename View::x_iterator src_it = this->_srcView.x_at( procWindowOutput.x1, y );
		typename View::x_iterator dst_it = this->_dstView.x_at( procWindowOutput.x1, y );
		for( int x = procWindowOutput.x1;
		     x < procWindowOutput.x2;
		     ++x, ++src_it, ++dst_it )
		{
			//x^a = e^aln(x)
			color_convert( *src_it, wpix );
			if( wpix[ 0 ] > 0.0 )
			{
				wpix[ 0 ] = exp( log( wpix[ 0 ] ) * 1.5 );
			}

			if( wpix[ 1 ] > 0.0 )
			{
				wpix[ 1 ] = exp( log( wpix[ 1 ] ) * 1.5 );
			}

			if( wpix[ 2 ] > 0.0 )
			{
				wpix[ 2 ] = exp( log( wpix[ 2 ] ) * 1.5 );
			}

			if( wpix[ 3 ] > 0.0 )
			{
				wpix[ 3 ] = exp( log( wpix[ 3 ] ) * 1.5 );
			}
			color_convert( wpix, *dst_it );
		}
		if( this->progressForward() )
			return;
	}
	/*
	   const OfxRectI procWindowSrc = this->translateRegion( procWindowRoW, this->_srcPixelRod );
	   OfxPointI procWindowSize = { procWindowRoW.x2 - procWindowRoW.x1,
				     procWindowRoW.y2 - procWindowRoW.y1 };
	   View src = subimage_view( this->_srcView, procWindowSrc.x1, procWindowSrc.y1,
				  procWindowSize.x,
				  procWindowSize.y );
	   View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
				  procWindowSize.x,
				  procWindowSize.y );
	   copy_pixels( src, dst );
	 */

}


/*
template<class View>
void ColorSpaceProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	OfxPointI procWindowSize  = {
		procWindowRoW.x2 - procWindowRoW.x1,
		procWindowRoW.y2 - procWindowRoW.y1
	};

	View src = subimage_view(	this->_srcView, procWindowOutput.x1, procWindowOutput.y1,
					procWindowSize.x,
					procWindowSize.y );
	View dst = subimage_view(	this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
					procWindowSize.x,
					procWindowSize.y );
	std::cout << "proc " << procWindowOutput.x1 << "+"<< procWindowOutput.x2 << " x " << procWindowOutput.y1 <<"+"<<procWindowOutput.y2 << std::endl;

	//colorspace_pixels_progress< ttlc::GradationLaw::linear, ttlc::Layout::RGB, ttlc::Primaries::TODO, ttlc::Premultiplication::Off, ttlc::GradationLaw::linear, ttlc::Layout::RGB, ttlc::Primaries::TODO, ttlc::Premultiplication::Off >( csAPI, this->_srcView, this->_dstView, procWindowRoW, this );

	switch( _gradationIn )
	{
		case ttlc::eParamLinear:
				colorspace_pixels_progress<	ttlc::ttlc_colorspace< ttlc::GradationLaw::linear >,
								ttlc::ttlc_colorspace< ttlc::GradationLaw::linear >
								>( &csAPI, src, dst, this );
				break;
		case ttlc::eParamsRGB:
				colorspace_pixels_progress<	ttlc::ttlc_colorspace< ttlc::GradationLaw::sRGB >,
								ttlc::ttlc_colorspace< ttlc::GradationLaw::linear >
								>( &csAPI, src, dst, this );
				break;
		case ttlc::eParamCineon:
				colorspace_pixels_progress<	ttlc::ttlc_colorspace< ttlc::GradationLaw::cineon >,
								ttlc::ttlc_colorspace< ttlc::GradationLaw::linear >
								>( &csAPI, src, dst, this );
				break;
		case ttlc::eParamGamma:
				colorspace_pixels_progress<	ttlc::ttlc_colorspace< ttlc::GradationLaw::gamma >,
								ttlc::ttlc_colorspace< ttlc::GradationLaw::linear >
								>( &csAPI, src, dst, this );
				break;
		case ttlc::eParamPanalog:
				colorspace_pixels_progress<	ttlc::ttlc_colorspace< ttlc::GradationLaw::panalog >,
								ttlc::ttlc_colorspace< ttlc::GradationLaw::linear >
								>( &csAPI, src, dst, this );
				break;
		case ttlc::eParamREDLog:
				colorspace_pixels_progress<	ttlc::ttlc_colorspace< ttlc::GradationLaw::redLog >,
								ttlc::ttlc_colorspace< ttlc::GradationLaw::linear >
								>( &csAPI, src, dst, this );
				break;
		case ttlc::eParamViperLog:
				colorspace_pixels_progress<	ttlc::ttlc_colorspace< ttlc::GradationLaw::viperLog >,
								ttlc::ttlc_colorspace< ttlc::GradationLaw::linear >
								>( &csAPI, src, dst, this );
				break;
		case ttlc::eParamREDSpace:
				colorspace_pixels_progress<	ttlc::ttlc_colorspace< ttlc::GradationLaw::redSpace >,
								ttlc::ttlc_colorspace< ttlc::GradationLaw::linear >
								>( &csAPI, src, dst, this );
				break;
		case ttlc::eParamAlexaLogC:
				colorspace_pixels_progress<	ttlc::ttlc_colorspace< ttlc::GradationLaw::alexaLogC >,
								ttlc::ttlc_colorspace< ttlc::GradationLaw::linear >
								>( &csAPI, src, dst, this );
				break;
	}

}*/

}
}
}
