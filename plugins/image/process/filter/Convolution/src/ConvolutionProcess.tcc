#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <boost/gil/extension/numeric/kernel.hpp>
#include <boost/gil/extension/numeric/convolve.hpp>

#include "ConvolutionPlugin.hpp"

namespace tuttle {
namespace plugin {
namespace convolution {

template<class View>
ConvolutionProcess<View>::ConvolutionProcess( ConvolutionPlugin& instance )
	: ImageGilFilterProcessor<View>( instance )
	, _plugin( instance )
{}

template <class View>
void ConvolutionProcess<View>::setup( const OFX::RenderArguments& args )
{
	TUTTLE_COUT_INFOS;
	ImageGilFilterProcessor<View>::setup( args );
	_params = _plugin.getProcessParams();

	TUTTLE_COUT_VAR( _params._size );
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template<class View>
void ConvolutionProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	OfxPointI procWindowSize  = {
		procWindowRoW.x2 - procWindowRoW.x1,
		procWindowRoW.y2 - procWindowRoW.y1
	};

	View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
	                          procWindowSize.x, procWindowSize.y );

	Point proc_tl( procWindowRoW.x1 - this->_srcPixelRod.x1, procWindowRoW.y1 - this->_srcPixelRod.y1 );
	/*
	switch( _params._convType )
	{
		case eConv1D:
		{
			if( _params._size.x == 0 )
				correlate_cols<Pixel>( this->_srcView, _params._gilKernelY, dst, proc_tl, _params._boundary_option );
			else if( _params._size.y == 0 )
				correlate_rows<Pixel>( this->_srcView, _params._gilKernelX, dst, proc_tl, _params._boundary_option );
			else
				correlate_rows_cols<Pixel>( this->_srcView, _params._gilKernelX, _params._gilKernelY, dst, proc_tl, _params._boundary_option );
			break;
		}
		case eConv2D:
		{
//			_params._convMatrix
			break;
		}
	}
	*/
}

}
}
}
