/**
 * This plugin uses fftw3 which is GPL licensed, thus, this plugin (only) is GPL licensed.
 */

#include "FftProcess.hpp"
#include "FftPlugin.hpp"
#include "../fftEngine/IfftwWrapper.hpp"

namespace tuttle {
namespace plugin {
namespace fftTransform {
namespace fft {

template<class View>
FftProcess<View>::FftProcess( FftPlugin& instance )
	: ImageGilFilterProcessor<View>( instance )
	, _plugin( instance )
{
	this->setNoMultiThreading();
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 *
 * @param[in] procWindowRoW  Processing window in RoW
 */
template<class View>
void FftProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;

	View src = subimage_view( this->_srcView, procWindowRoW.x1, procWindowRoW.y1,
	                          procWindowRoW.x2 - procWindowRoW.x1,
	                          procWindowRoW.y2 - procWindowRoW.y1 );
	View dst = subimage_view( this->_dstView, procWindowRoW.x1, procWindowRoW.y1,
	                          procWindowRoW.x2 - procWindowRoW.x1,
	                          procWindowRoW.y2 - procWindowRoW.y1 );

	// Create a planar floating point view
	//	typedef pixel<bits32f, layout<typename color_space_type<View>::type> > Pixel32f;
	typedef rgb32f_pixel_t Pixel32f;
	typedef typename boost::gil::image<Pixel32f, true> PlanarImage;
	typedef typename PlanarImage::view_t PlanarView;
	PlanarImage simg( src.dimensions() );
	PlanarView svw( view( simg ) );
	rgb32f_planar_image_t dimg( src.dimensions() );
	rgb32f_planar_view_t dvw( view( dimg ) );

	// Convert source view type to planar view
	copy_and_convert_pixels( src, svw );
	// Apply fft on each plane
	tuttle::plugin::fft::FftwWrapperCPU fft;

	FftTransformProcessParams params = _plugin.getProcessParams();

	switch( params._type )
	{
		case eFftMag:
		{
			for( int c = 0; c < num_channels<Pixel32f>::value; ++c )
			{
				fft.fftMag( (const float*)boost::gil::planar_view_get_raw_data( svw, c ),
				            (float*)boost::gil::planar_view_get_raw_data( dvw, c ),
				            src.width(), src.height() );
			}
			break;
		}
		case eFftMod:
		{
			for( int c = 0; c < num_channels<Pixel32f>::value; ++c )
			{
				fft.fftMod( (const float*)boost::gil::planar_view_get_raw_data( svw, c ),
				            (float*)boost::gil::planar_view_get_raw_data( dvw, c ),
				            src.width(), src.height() );
			}
			break;
		}
		case eFftPhase:
		{
			for( int c = 0; c < num_channels<Pixel32f>::value; ++c )
			{
				fft.fftPhase( (const float*)boost::gil::planar_view_get_raw_data( svw, c ),
				              (float*)boost::gil::planar_view_get_raw_data( dvw, c ),
				              src.width(), src.height() );
			}
			break;
		}
	}

	// Convert planar view to dst view type
	copy_and_convert_pixels( dvw, dst );
}

}
}
}
}
