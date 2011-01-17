#include "ResizeAlgorithm.hpp"

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/numeric/sampler.hpp>
#include <boost/gil/extension/numeric/resample.hpp>

namespace tuttle {
namespace plugin {
namespace resize {

template<class View>
ResizeProcess<View>::ResizeProcess( ResizePlugin &effect )
: ImageGilFilterProcessor<View>( effect )
, _plugin( effect )
{
	this->setNoMultiThreading();
}

template<class View>
void ResizeProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args );
	_params = _plugin.getProcessParams( args.renderScale );

}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template<class View>
void ResizeProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
//	const OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
//	const OfxRectI procWindowSrc = translateRegion( procWindowRoW, this->_srcPixelRod );
//	const OfxPointI procWindowSize = { procWindowRoW.x2 - procWindowRoW.x1,
//							     procWindowRoW.y2 - procWindowRoW.y1 };
//	View src = subimage_view( this->_srcView, procWindowSrc.x1, procWindowSrc.y1,
//							                  procWindowSize.x, procWindowSize.y );
//	View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
//							                  procWindowSize.x, procWindowSize.y );
//
//	resize_view( src, dst, bilinear_sampler() );
	
	resize_view( this->_srcView, this->_dstView, bilinear_sampler() );
}

}
}
}
