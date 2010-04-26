#include <tuttle/common/utils/global.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/PluginException.hpp>
#include <tuttle/common/image/gilGlobals.hpp>

#include <cmath>
#include <vector>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace invert {

struct inverter
{
	template< class P>
	P operator()( const P& p ) const
	{
		using namespace bgil;
		P p2;
		// @todo TODO_OFX Improve this: Don't consider alpha
		// http://stlab.adobe.com/gil/html/color__convert_8hpp-source.html
		// typedef typename channel_type<P2>::type T2;
		// channel_convert<T2>(alpha_or_max(src))
		// mpl::contains<typename color_space_type<Pixel>::type,alpha_t>() -> mpl::true_ ?
		const int nc = std::min( (int)num_channels<P>::type::value, 3 );

		for( int n = 0; n < nc; ++n )
		{
			p2[n] = channel_traits< typename channel_type< P >::type >::max_value() - p[n] +
			        channel_traits< typename channel_type< P >::type >::min_value();
		}
		if( num_channels<P>::type::value > 3 )
			p2[3] = p[3];
		return p2;
	}
};

template<class View>
InvertProcess<View>::InvertProcess( InvertPlugin& instance )
	: ImageGilFilterProcessor<View>( instance ),
	_plugin( instance )
{}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template<class View>
void InvertProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	OfxPointI procWindowSize = { procWindowRoW.x2 - procWindowRoW.x1,
							     procWindowRoW.y2 - procWindowRoW.y1 };

	View src = subimage_view( this->_srcView, procWindowOutput.x1, procWindowOutput.y1,
							  procWindowSize.x,
							  procWindowSize.y );
	View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
							  procWindowSize.x,
							  procWindowSize.y );

	transform_pixels( src, dst, inverter() );
}

}
}
}
