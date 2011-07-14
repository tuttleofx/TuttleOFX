#include "pinningAlgorithm.hpp"

#include <terry/globals.hpp>
#include <tuttle/plugin/image/resample.hpp>
#include <tuttle/plugin/exceptions.hpp>

namespace tuttle {
namespace plugin {
namespace pinning {

template<class View>
PinningProcess<View>::PinningProcess( PinningPlugin& effect )
: ImageGilFilterProcessor<View>( effect )
, _plugin( effect )
{
}

template<class View>
void PinningProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args );

	_params = _plugin.getProcessParams( args.time, args.renderScale );
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template<class View>
void PinningProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );

	switch( _params._interpolation )
	{
		case eParamInterpolationNearest:
		{
			resample<terry::sampler::nearest_neighbor_sampler>( this->_srcView, this->_dstView, procWindowOutput );
			return;
		}
		case eParamInterpolationBilinear:
		{
			resample<terry::sampler::bilinear_sampler>( this->_srcView, this->_dstView, procWindowOutput );
			return;
		}
		case eParamInterpolationBicubic:
		{
			resample<terry::sampler::bicubic_sampler>( this->_srcView, this->_dstView, procWindowOutput );
			return;
		}
		case eParamInterpolationCatmul:
		{
			resample<terry::sampler::catmul_sampler>( this->_srcView, this->_dstView, procWindowOutput );
			return;
		}
		case eParamInterpolationMitchell:
		{
			resample<terry::sampler::mitchell_sampler>( this->_srcView, this->_dstView, procWindowOutput );
			return;
		}
		case eParamInterpolationParzen:
		{
			resample<terry::sampler::parzen_sampler>( this->_srcView, this->_dstView, procWindowOutput );
			return;
		}
		case eParamInterpolationKeys:
		{
			resample<terry::sampler::keys_sampler>( this->_srcView, this->_dstView, procWindowOutput );
			return;
		}
		case eParamInterpolationSimon:
		{
			resample<terry::sampler::simon_sampler>( this->_srcView, this->_dstView, procWindowOutput );
			return;
		}
		case eParamInterpolationRifman:
		{
			resample<terry::sampler::rifman_sampler>( this->_srcView, this->_dstView, procWindowOutput );
			return;
		}


//		case eParamInterpolationLanczos3:
//		{
//			resample<terry::sampler::lanczos3_sampler>( this->_srcView, this->_dstView, procWindowOutput );
//			return;
//		}
//		case eParamInterpolationLanczos4:
//		{
//			resample<terry::sampler::lanczos4_sampler>( this->_srcView, this->_dstView, procWindowOutput );
//			return;
//		}
//		case eParamInterpolationLanczos6:
//		{
//			resample<terry::sampler::lanczos6_sampler>( this->_srcView, this->_dstView, procWindowOutput );
//			return;
//		}
//		case eParamInterpolationLanczos12:
//		{
//			resample<terry::sampler::lanczos12_sampler>( this->_srcView, this->_dstView, procWindowOutput );
//			return;
//		}
//		case eParamInterpolationGaussian:
//		{
//			resample<terry::sampler::gaussian_sampler>( this->_srcView, this->_dstView, procWindowOutput );
//			return;
//		}
	}
	BOOST_THROW_EXCEPTION( exception::Bug()
		<< exception::user( "Interpolation method not recognize." ) );
}

template<class View>
template<class Sampler>
void PinningProcess<View>::resample( View& srcView, View& dstView, const OfxRectI& procWindow )
{
	using namespace boost::gil;
	switch( _params._method )
	{
		case eParamMethodAffine:
		case eParamMethodPerspective:
			resample_pixels_progress<Sampler>( srcView, dstView, _params._perspective, procWindow, this );
			return;
		case eParamMethodBilinear:
			resample_pixels_progress<Sampler>( srcView, dstView, _params._bilinear, procWindow, this );
			return;
	}
}

}
}
}

