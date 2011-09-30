#include "LensDistortPlugin.hpp"

#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/image/resample.hpp>
#include <tuttle/plugin/numeric/rectOp.hpp>

#include <terry/sampler/sampler.hpp>

namespace tuttle {
namespace plugin {
namespace lens {

template<class View>
LensDistortProcess<View>::LensDistortProcess( LensDistortPlugin& instance )
	: ImageGilFilterProcessor<View>( instance )
	, _plugin( instance )
{}

template<class View>
void LensDistortProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args );

	// recovery parameters values
	_lensType      = _plugin.getLensType();
	_interpolation = _plugin.getInterpolation();
	_centerType    = _plugin.getCenterType();

	OfxRectD srcRod = rectIntToDouble( this->_srcPixelRod );
	OfxRectD dstRod = rectIntToDouble( this->_dstPixelRod );
	if( _plugin._srcRefClip->isConnected() )
	{
		OfxRectD srcRefRod = rectIntToDouble( _plugin._srcRefClip->getPixelRod( args.time, args.renderScale ) );
		_p = _plugin.getProcessParams( srcRod, dstRod, srcRefRod, this->_clipDst->getPixelAspectRatio() );
	}
	else
	{
		_p = _plugin.getProcessParams( srcRod, dstRod, this->_clipDst->getPixelAspectRatio() );
	}
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in]   procWindowRoW     Processing window
 */
template<class View>
void LensDistortProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace bgil;
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );

	switch( _interpolation )
	{
		case eParamInterpolationNearest:
		{
			lensDistort<terry::sampler::nearest_neighbor_sampler>( this->_srcView, this->_dstView, procWindowOutput );
			return;
		}
		case eParamInterpolationBilinear:
		{
			lensDistort<terry::sampler::bilinear_sampler>( this->_srcView, this->_dstView, procWindowOutput );
			return;
		}
		case eParamInterpolationBicubic:
		{
			lensDistort<terry::sampler::bicubic_sampler>( this->_srcView, this->_dstView, procWindowOutput );
			return;
		}
		//case eParamInterpolationCatmul:
		//{
		//	lensDistort<terry::sampler::catmul_sampler>( this->_srcView, this->_dstView, procWindowOutput );
		//	return;
		//}
		case eParamInterpolationMitchell:
		{
			lensDistort<terry::sampler::mitchell_sampler>( this->_srcView, this->_dstView, procWindowOutput );
			return;
		}
		case eParamInterpolationParzen:
		{
			lensDistort<terry::sampler::parzen_sampler>( this->_srcView, this->_dstView, procWindowOutput );
			return;
		}
		case eParamInterpolationKeys:
		{
			lensDistort<terry::sampler::keys_sampler>( this->_srcView, this->_dstView, procWindowOutput );
			return;
		}
		case eParamInterpolationSimon:
		{
			lensDistort<terry::sampler::simon_sampler>( this->_srcView, this->_dstView, procWindowOutput );
			return;
		}
		case eParamInterpolationRifman:
		{
			lensDistort<terry::sampler::rifman_sampler>( this->_srcView, this->_dstView, procWindowOutput );
			return;
		}


//		case eParamInterpolationLanczos3:
//		{
//			lensDistort<terry::sampler::lanczos3_sampler>( this->_srcView, this->_dstView, procWindowOutput );
//			return;
//		}
//		case eParamInterpolationLanczos4:
//		{
//			lensDistort<terry::sampler::lanczos4_sampler>( this->_srcView, this->_dstView, procWindowOutput );
//			return;
//		}
//		case eParamInterpolationLanczos6:
//		{
//			lensDistort<terry::sampler::lanczos6_sampler>( this->_srcView, this->_dstView, procWindowOutput );
//			return;
//		}
//		case eParamInterpolationLanczos12:
//		{
//			lensDistort<terry::sampler::lanczos12_sampler>( this->_srcView, this->_dstView, procWindowOutput );
//			return;
//		}
//		case eParamInterpolationGaussian:
//		{
//			lensDistort<terry::sampler::gaussian_sampler>( this->_srcView, this->_dstView, procWindowOutput );
//			return;
//		}
	}
	BOOST_THROW_EXCEPTION( exception::Bug()
		<< exception::user( "Interpolation method not recognize." ) );
}

template<class View>
template<class Sampler>
void LensDistortProcess<View>::lensDistort( View& srcView, View& dstView, const OfxRectI& procWindow )
{
	switch( _lensType )
	{
		case eParamLensTypeStandard:
		{
			if( _p._distort )
			{
				resample_pixels_progress<Sampler>( srcView, dstView, static_cast<NormalLensDistortParams<double>&>( _p ), procWindow, this );
			}
			else
			{
				resample_pixels_progress<Sampler>( srcView, dstView, static_cast<NormalLensUndistortParams<double>&>( _p ), procWindow, this );
			}
			return;
		}
		case eParamLensTypeFisheye:
		{
			if( _p._distort )
				resample_pixels_progress<Sampler>( srcView, dstView, static_cast<FisheyeLensDistortParams<double>&>( _p ), procWindow, this );
			else
				resample_pixels_progress<Sampler>( srcView, dstView, static_cast<FisheyeLensUndistortParams<double>&>( _p ), procWindow, this );
			return;
		}
		case eParamLensTypeAdvanced:
		{
			if( _p._distort )
				resample_pixels_progress<Sampler>( srcView, dstView, static_cast<AdvancedLensDistortParams<double>&>( _p ), procWindow, this );
			else
				resample_pixels_progress<Sampler>( srcView, dstView, static_cast<AdvancedLensUndistortParams<double>&>( _p ), procWindow, this );
			return;
		}
	}
	BOOST_THROW_EXCEPTION( exception::Bug()
		<< exception::user( "Lens type not recognize." ) );
}

}
}
}


