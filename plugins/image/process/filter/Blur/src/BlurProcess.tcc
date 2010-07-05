#include "BlurPlugin.hpp"
#include <tuttle/plugin/image/gil/gaussianKernel.hpp>

#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <boost/gil/extension/numeric/convolve.hpp>
//#include <boost/gil/extension/io/png_dynamic_io.hpp> /// @todo tuttle: to remove

#include <boost/units/pow.hpp>
#include <boost/lambda/core.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/algorithm.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <cmath>
#include <vector>
#include <algorithm>



namespace tuttle {
namespace plugin {
namespace blur {

template<class View>
BlurProcess<View>::BlurProcess( BlurPlugin &effect )
: ImageGilFilterProcessor<View>( effect )
, _plugin( effect )
{
}

template <class View>
void BlurProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args );
	_params = _plugin.getProcessParams();

//	COUT_X( 20, "_" );
//	COUT_VAR( _params._size );
//	COUT_VAR2( _params._gilKernelX.size(), _params._gilKernelY.size() );
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template<class View>
void BlurProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using boost::numeric_cast;
	using namespace boost::gil;
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	OfxPointI procWindowSize = { procWindowRoW.x2 - procWindowRoW.x1,
							     procWindowRoW.y2 - procWindowRoW.y1 };

	View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
							  procWindowSize.x, procWindowSize.y );
	
	Point proc_tl( procWindowRoW.x1-this->_srcPixelRod.x1, procWindowRoW.y1-this->_srcPixelRod.y1 );
	
	convolve_boundary_option option = convolve_option_extend_mirror;
	switch( _params._border )
	{
		case eBorderMirror:
			option = convolve_option_extend_mirror;
			break;
		case eBorderConstant:
			option = convolve_option_extend_constant;
			break;
		case eBorderBlack:
			option = convolve_option_extend_zero;
			break;
		case eBorderPadded:
			option = convolve_option_extend_padded;
			break;
	}

	correlate_rows_cols<Pixel>( this->_srcView, _params._gilKernelX, _params._gilKernelY, dst, proc_tl, option );
}

}
}
}
