#include "BlurPlugin.hpp"
#include <tuttle/plugin/image/gil/gaussianKernel.hpp>
#include <boost/gil/extension/numeric/convolve.hpp>

//#include <boost/lambda/lambda.hpp>

namespace tuttle {
namespace plugin {
namespace blur {

template<class View>
BlurProcess<View>::BlurProcess( BlurPlugin& effect )
	: ImageGilFilterProcessor<View>( effect )
	, _plugin( effect )
{}

template <class View>
void BlurProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args );
	_params = _plugin.getProcessParams( args.renderScale );

	//	TUTTLE_COUT_X( 20, "_" );
	//	TUTTLE_COUT_VAR( _params._size );
	//	TUTTLE_COUT_VAR2( _params._gilKernelX.size(), _params._gilKernelY.size() );
	//	std::cout << "x [";
	//	std::for_each(_params._gilKernelX.begin(), _params._gilKernelX.end(), std::cout << boost::lambda::_1 << ',');
	//	std::cout << "]" << std::endl;
	//	std::cout << "y [";
	//	std::for_each(_params._gilKernelY.begin(), _params._gilKernelY.end(), std::cout << boost::lambda::_1 << ',');
	//	std::cout << "]" << std::endl;
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template<class View>
void BlurProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
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

	if( _params._size.x == 0 )
		correlate_cols<Pixel>( this->_srcView, _params._gilKernelY, dst, proc_tl, _params._boundary_option );
	else if( _params._size.y == 0 )
		correlate_rows<Pixel>( this->_srcView, _params._gilKernelX, dst, proc_tl, _params._boundary_option );
	else
		correlate_rows_cols<Pixel>( this->_srcView, _params._gilKernelX, _params._gilKernelY, dst, proc_tl, _params._boundary_option );
}

}
}
}
