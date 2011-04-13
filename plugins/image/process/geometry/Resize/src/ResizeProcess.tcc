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

	double src_width  = std::max<double>(this->_srcView.width () -1,1);
	double src_height = std::max<double>(this->_srcView.height() -1,1);
	double dst_width  = std::max<double>(this->_dstView.width () -1,1);
	double dst_height = std::max<double>(this->_dstView.height() -1,1);

	matrix3x2<double> mat =
		matrix3x2<double>::get_translate(-dst_width/2.0, -dst_height/2.0) *
		matrix3x2<double>::get_scale( (src_width+1) / dst_width, (src_height +1)/ dst_height) *
		//matrix3x2<double>::get_scale( (src_width+1) / src_width, (src_height+1)/src_height) *
		matrix3x2<double>::get_translate( src_width/2.0 ,  src_height/2.0 );

	//resample_pixels(src,dst,mat,sampler);

	switch( _filter )
	{
		case eParamFilterNearest	: resample_pixels( this->_srcView, this->_dstView, mat, ttl_nearest_neighbor_sampler()	); break;
		case eParamFilterBilinear	: resample_pixels( this->_srcView, this->_dstView, mat, ttl_bilinear_sampler()		); break;
		case eParamFilterBicubic	: resample_pixels( this->_srcView, this->_dstView, mat, ttl_bicubic_sampler()		); break;
		case eParamFilterKeys		: resample_pixels( this->_srcView, this->_dstView, mat, ttl_keys_sampler()			); break;
		case eParamFilterSimon		: resample_pixels( this->_srcView, this->_dstView, mat, ttl_simon_sampler()			); break;
		case eParamFilterRifman		: resample_pixels( this->_srcView, this->_dstView, mat, ttl_rifman_sampler()		); break;
		case eParamFilterLanczos	: resample_pixels( this->_srcView, this->_dstView, mat, ttl_lanczos_sampler()		); break;
	}
}

}
}
}
