#include <boost/numeric/conversion/cast.hpp>

#include <terry/globals.hpp>
#include <terry/algorithm/transform_pixels_progress.hpp>

namespace tuttle {
namespace plugin {
namespace idKeyer {

template<class View>
IdKeyerProcess<View>::IdKeyerProcess( IdKeyerPlugin& instance )
	: Parent( instance, eImageOrientationIndependant )
	, _plugin( instance )
{}

template<class View>
void IdKeyerProcess<View>::setup( const OFX::RenderArguments& args )
{
	using namespace boost::gil;
	Parent::setup( args );

	_params = _plugin.getProcessParams<View>();
}

struct id_keyer_t
{
	typedef std::vector<terry::rgba32f_pixel_t> PixelList;
	PixelList _colors;

	id_keyer_t( PixelList colors )
		: _colors ( colors )
	{
	}

	template<class Pixel>
	Pixel operator()( const Pixel& src ) const
	{
		using namespace terry;
		Pixel res = src;

		for( PixelList::const_iterator color = _colors.begin(); color != _colors.end(); ++color )
		{
			//TUTTLE_LOG_WARNING( get_color( res, red_t()   ) << "\t" << get_color( *color, red_t()   ) );
			if( get_color( res, red_t()   ) == get_color( *color, red_t()   ) &&
				get_color( res, green_t() ) == get_color( *color, green_t() ) &&
				get_color( res, blue_t()  ) == get_color( *color, blue_t()  ) &&
				get_color( res, alpha_t()  ) == get_color( *color, alpha_t()  )
				)
			{
				get_color( res, red_t() )   = 1.0;
				get_color( res, green_t() ) = 1.0;
				get_color( res, blue_t() )  = 1.0;
			}
			else
			{
				get_color( res, red_t() )   = 0.0;
				get_color( res, green_t() ) = 0.0;
				get_color( res, blue_t() )  = 0.0;
			}
		}

		get_color( res, alpha_t() ) = 1.0;
		return res;
	}
};


/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template<class View>
void IdKeyerProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace terry;
	using namespace terry::algorithm;
	const OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	const OfxRectI procWindowSrc = translateRegion( procWindowRoW, this->_srcPixelRod );
	const OfxPointI procWindowSize = { procWindowRoW.x2 - procWindowRoW.x1,
									   procWindowRoW.y2 - procWindowRoW.y1 };
	View src = subimage_view( this->_srcView, procWindowSrc.x1, procWindowSrc.y1,
							  procWindowSize.x, procWindowSize.y );
	View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
							  procWindowSize.x, procWindowSize.y );

	transform_pixels_progress(
			src,
			dst,
			id_keyer_t( _params._colors ),
			*this );
}

}
}
}
