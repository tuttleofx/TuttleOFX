#include <tuttle/plugin/image/resample.hpp>
#include <terry/sampler/bc.hpp>
#include <terry/sampler/bilinear.hpp>
#include <terry/sampler/gaussian.hpp>
#include <terry/sampler/lanczos.hpp>
#include <terry/sampler/nearestNeighbor.hpp>

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
void ResizeProcess<View>::multiThreadProcessImages( const OfxRectI& procWindow )
{
	using namespace terry;
	using namespace terry::sampler;

	const double src_width  = std::max<double>(this->_srcView.width () -1,1);
	const double src_height = std::max<double>(this->_srcView.height() -1,1);
	const double dst_width  = std::max<double>(this->_dstView.width () -1,1);
	const double dst_height = std::max<double>(this->_dstView.height() -1,1);

	//TUTTLE_COUT("\E[1;31mResize Position = " << -( _params._centerPoint.x - dst_width * 0.5) << "x" << -( _params._centerPoint.y - dst_height * 0.5) << "\E[0;0m");

	const EParamFilterOutOfImage outOfImageProcess = static_cast<EParamFilterOutOfImage>(_params._outOfImageProcess);

	matrix3x2<double> mat;

	if( _params._changeCenter )
	{
		mat =	matrix3x2<double>::get_translate( -( _params._centerPoint.x - dst_width * 0.5) , -( _params._centerPoint.y - dst_height * 0.5) ) *
			matrix3x2<double>::get_translate( - dst_width * 0.5, - dst_height * 0.5 ) *
			matrix3x2<double>::get_scale    ( (src_width + 1) / (dst_width + 1 ), (src_height + 1) / (dst_height + 1) ) *
			matrix3x2<double>::get_translate( src_width * 0.5 , src_height * 0.5 )
			;
	}
	else
	{
		mat =	matrix3x2<double>::get_translate( - dst_width * 0.5, - dst_height * 0.5 ) *
			matrix3x2<double>::get_scale    ( (src_width + 1) / (dst_width + 1 ), (src_height + 1) / (dst_height + 1) ) *
			matrix3x2<double>::get_translate( src_width * 0.5 , src_height * 0.5 )
			;
	}


	switch( _params._filter )
	{
		case eParamFilterNearest	: resample_pixels_progress< ::terry::sampler::nearest_neighbor_sampler	>( this->_srcView, this->_dstView, mat, procWindow, outOfImageProcess, this	); break;
		case eParamFilterBilinear	: resample_pixels_progress< ::terry::sampler::bilinear_sampler		>( this->_srcView, this->_dstView, mat, procWindow, outOfImageProcess, this	); break;

		case eParamFilterBC :
		{
			bc_sampler BCsampler;
			BCsampler.valB = _params._paramB;
			BCsampler.valC = _params._paramC;
			resample_pixels_progress( this->_srcView, this->_dstView, mat, procWindow, outOfImageProcess, this, BCsampler );
			break;
		}

		case eParamFilterBicubic  : resample_pixels_progress< bicubic_sampler	>( this->_srcView, this->_dstView, mat, procWindow, outOfImageProcess, this ); break;
		case eParamFilterCatrom   : resample_pixels_progress< catrom_sampler	>( this->_srcView, this->_dstView, mat, procWindow, outOfImageProcess, this ); break;
		case eParamFilterKeys     : resample_pixels_progress< keys_sampler	>( this->_srcView, this->_dstView, mat, procWindow, outOfImageProcess, this ); break;
		case eParamFilterSimon    : resample_pixels_progress< simon_sampler	>( this->_srcView, this->_dstView, mat, procWindow, outOfImageProcess, this ); break;
		case eParamFilterRifman   : resample_pixels_progress< rifman_sampler	>( this->_srcView, this->_dstView, mat, procWindow, outOfImageProcess, this ); break;

		case eParamFilterMitchell : resample_pixels_progress< mitchell_sampler	>( this->_srcView, this->_dstView, mat, procWindow, outOfImageProcess, this ); break;
		case eParamFilterParzen   : resample_pixels_progress< parzen_sampler	>( this->_srcView, this->_dstView, mat, procWindow, outOfImageProcess, this ); break;

		case eParamFilterGaussian :
		{
			gaussian_sampler gaussianSampler;
			gaussianSampler.size  = _params._filterSize;
			gaussianSampler.sigma = _params._filterSigma;
			resample_pixels_progress( this->_srcView, this->_dstView, mat, procWindow, outOfImageProcess, this, gaussianSampler );
			break;
		}

		case eParamFilterLanczos  :
		{
			lanczos_sampler lanczosSampler;
			lanczosSampler.size = _params._filterSize;
			resample_pixels_progress( this->_srcView, this->_dstView, mat, procWindow, outOfImageProcess, this, lanczosSampler );
			break;
		}

		case eParamFilterLanczos3	: resample_pixels_progress< lanczos3_sampler		>( this->_srcView, this->_dstView, mat, procWindow, outOfImageProcess, this	); break;
		case eParamFilterLanczos4	: resample_pixels_progress< lanczos4_sampler		>( this->_srcView, this->_dstView, mat, procWindow, outOfImageProcess, this	); break;
		case eParamFilterLanczos6	: resample_pixels_progress< lanczos6_sampler		>( this->_srcView, this->_dstView, mat, procWindow, outOfImageProcess, this	); break;
		case eParamFilterLanczos12	: resample_pixels_progress< lanczos12_sampler		>( this->_srcView, this->_dstView, mat, procWindow, outOfImageProcess, this	); break;

		default: break;
	}
}

}
}
}
