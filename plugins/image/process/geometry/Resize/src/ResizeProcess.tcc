#include "ResizeAlgorithm.hpp"

//#include <boost/gil/gil_all.hpp>
//#include <boost/gil/extension/numeric/sampler.hpp>
//#include <boost/gil/extension/numeric/resample.hpp>

//#include <tuttle/plugin/image/ofxToGil.hpp>
#include <tuttle/plugin/image/gil/resample.hpp>


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

	_filter = _plugin.getFilter();
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template<class View>
void ResizeProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	
	switch( _filter )
	{
		case eParamFilterNearest	: resize_view( this->_srcView, this->_dstView, nearest_neighbor_sampler()	); break;
		case eParamFilterBilinear	: resize_view( this->_srcView, this->_dstView, bilinear_sampler()		); break;
		case eParamFilterBicubic	: resize_view( this->_srcView, this->_dstView, bicubic_sampler()		); break;
		case eParamFilterLanczos	: resize_view( this->_srcView, this->_dstView, lanczos_sampler()		); break;
	}
}

}
}
}
