#include "pinningAlgorithm.hpp"

#include <terry/globals.hpp>
#include <terry/sampler/sampler.hpp>
#include <terry/sampler/all.hpp>

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
	using namespace terry;
	using namespace terry::sampler;
	
	const OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );

	switch( _params._interpolation )
	{
		case eParamFilterNearest:
		{
			resample< ::terry::sampler::nearest_neighbor_sampler >( this->_srcView, this->_dstView, procWindowOutput );
			return;
		}
		case eParamFilterBilinear:
		{
			resample< ::terry::sampler::bilinear_sampler >( this->_srcView, this->_dstView, procWindowOutput );
			return;
		}
		case eParamFilterBC:
		{
			bc_sampler BCsampler;
			BCsampler.valB = _params._paramB;
			BCsampler.valC = _params._paramC;
			resample( this->_srcView, this->_dstView, procWindowOutput, BCsampler );
			return;
		}
		case eParamFilterBicubic:
		{
			resample<bicubic_sampler>( this->_srcView, this->_dstView, procWindowOutput );
			return;
		}
		case eParamFilterCatrom:
		{
			resample<catrom_sampler>( this->_srcView, this->_dstView, procWindowOutput );
			return;
		}
		case eParamFilterMitchell:
		{
			resample<mitchell_sampler>( this->_srcView, this->_dstView, procWindowOutput );
			return;
		}
		case eParamFilterParzen:
		{
			resample<parzen_sampler>( this->_srcView, this->_dstView, procWindowOutput );
			return;
		}
		case eParamFilterKeys:
		{
			resample<keys_sampler>( this->_srcView, this->_dstView, procWindowOutput );
			return;
		}
		case eParamFilterSimon:
		{
			resample<simon_sampler>( this->_srcView, this->_dstView, procWindowOutput );
			return;
		}
		case eParamFilterRifman:
		{
			resample<rifman_sampler>( this->_srcView, this->_dstView, procWindowOutput );
			return;
		}
		case eParamFilterLanczos:
		{
			lanczos_sampler lanczosSampler;
			lanczosSampler.size = _params._filterSize;
			resample( this->_srcView, this->_dstView, procWindowOutput, lanczosSampler );
			return;
		}
		case eParamFilterLanczos3:
		{
			resample<lanczos3_sampler>( this->_srcView, this->_dstView, procWindowOutput );
			return;
		}
		case eParamFilterLanczos4:
		{
			resample<lanczos4_sampler>( this->_srcView, this->_dstView, procWindowOutput );
			return;
		}
		case eParamFilterLanczos6:
		{
			resample<lanczos6_sampler>( this->_srcView, this->_dstView, procWindowOutput );
			return;
		}
		case eParamFilterLanczos12:
		{
			resample<lanczos12_sampler>( this->_srcView, this->_dstView, procWindowOutput );
			return;
		}
		case eParamFilterGaussian:
		{
			gaussian_sampler gaussianSampler;
			gaussianSampler.size  = _params._filterSize;
			gaussianSampler.sigma = _params._filterSigma;
			resample<gaussian_sampler>( this->_srcView, this->_dstView, procWindowOutput );
			return;
		}
	}
	BOOST_THROW_EXCEPTION( exception::Bug()
		<< exception::user( "Interpolation method not recognize." ) );
}

template<class View>
template<class Sampler>
void PinningProcess<View>::resample( View& srcView, View& dstView, const OfxRectI& procWindow, const Sampler& sampler )
{
	using namespace boost::gil;
	switch( _params._method )
	{
		case eParamMethodAffine:
		case eParamMethodPerspective:
			resample_pixels_progress<Sampler>( srcView, dstView, _params._perspective, procWindow, _params._outOfImageProcess, this, sampler );
			return;
		case eParamMethodBilinear:
			resample_pixels_progress<Sampler>( srcView, dstView, _params._bilinear, procWindow, _params._outOfImageProcess, this, sampler );
			return;
	}
}

}
}
}

