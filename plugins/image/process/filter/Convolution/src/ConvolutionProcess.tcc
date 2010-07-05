#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <boost/gil/extension/numeric/kernel.hpp>
#include <boost/gil/extension/numeric/convolve.hpp>

#include "ConvolutionPlugin.hpp"

namespace tuttle {
namespace plugin {
namespace convolution {

template<class View>
ConvolutionProcess<View>::ConvolutionProcess( ConvolutionPlugin &instance )
: ImageGilFilterProcessor<View>( instance )
, _plugin( instance )
{
}

template <class View>
void ConvolutionProcess<View>::setup( const OFX::RenderArguments& args )
{
	COUT_INFOS;
	ImageGilFilterProcessor<View>::setup( args );
	_params = _plugin.getProcessParams();

	COUT_VAR( _params._size );
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template<class View>
void ConvolutionProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	COUT_INFOS;
	using namespace boost::gil;
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	OfxPointI procWindowSize = { procWindowRoW.x2 - procWindowRoW.x1,
							     procWindowRoW.y2 - procWindowRoW.y1 };

	View src = subimage_view( this->_srcView, procWindowOutput.x1, procWindowOutput.y1,
							  procWindowSize.x, procWindowSize.y );
	View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
							  procWindowSize.x, procWindowSize.y );

	/// @todo tuttle: not correct...
    kernel_1d<double> kernel( &(_params._coef[0]), _params._coef.size(), _params._size.x );

    convolve_rows<Pixel>( src, kernel, dst );
    convolve_cols<Pixel>( src, kernel, dst );

	/// @todo tuttle: add convolve_option
//	switch( _params._outputType )
//	{
//		case :
//			convolve_rows<Pixel>( src, kernel, dst, convolve_option_output_ignore );
//			convolve_cols<Pixel>( src, kernel, dst, convolve_option_output_ignore );
//			break;
//		case :
//			convolve_rows<Pixel>( src, kernel, dst, convolve_option_output_zero );
//			convolve_cols<Pixel>( src, kernel, dst, convolve_option_output_zero );
//			break;
//		case :
//			convolve_rows<Pixel>( src, kernel, dst, convolve_option_extend_constant );
//			convolve_cols<Pixel>( src, kernel, dst, convolve_option_extend_constant );
//			break;
//		case :
//			convolve_rows<Pixel>( src, kernel, dst, convolve_option_extend_padded );
//			convolve_cols<Pixel>( src, kernel, dst, convolve_option_extend_padded );
//			break;
//		case :
//			convolve_rows<Pixel>( src, kernel, dst, convolve_option_extend_zero );
//			convolve_cols<Pixel>( src, kernel, dst, convolve_option_extend_zero );
//			break;
//	}
}

}
}
}
