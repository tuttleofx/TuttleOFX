#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/image/gil/algorithm.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <boost/gil/extension/numeric/kernel.hpp>
#include <boost/gil/extension/numeric/convolve.hpp>
#include <boost/gil/utilities.hpp>
#include <boost/mpl/if.hpp>
#include <boost/type_traits/is_floating_point.hpp>

#include <cmath>

namespace tuttle {
namespace plugin {
namespace sobel {


struct transform_pixel_gradientDirection_t
{
	template< typename Pixel>
	Pixel operator()( const Pixel& p1, const Pixel& p2 ) const
	{
		Pixel res;
//		channel_gradientDirection_t<Pixel>( )( p1, p2, res );
		res[0] = std::atan( p1[0] / p2[0] );
		return res;
	}
};

template<class View>
SobelProcess<View>::SobelProcess( SobelPlugin &effect )
: ImageGilFilterProcessor<View>( effect )
, _plugin( effect )
{
}

template <class View>
void SobelProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args );
	_params = _plugin.getProcessParams( args.renderScale );
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template<class View>
void SobelProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost;
	using namespace boost::gil;
	namespace bgil = boost::gil;
	
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	OfxPointI procWindowSize  = {
		procWindowRoW.x2 - procWindowRoW.x1,
		procWindowRoW.y2 - procWindowRoW.y1
	};
	
	typedef typename View::point_t Point;
	typedef typename channel_mapping_type<View>::type Channel;
	typedef typename mpl::if_< is_floating_point<Channel>,
	                           Channel,
							   bgil::bits32f>::type ChannelFloat;
	typedef bgil::pixel<ChannelFloat, gray_layout_t> PixelGray;
	typedef bgil::image<PixelGray, false> ImageGray;
	typedef typename ImageGray::view_t ViewGray;

	View dst = subimage_view( this->_dstView,
	                          procWindowOutput.x1, procWindowOutput.y1,
	                          procWindowSize.x, procWindowSize.y );

	Point proc_tl( procWindowRoW.x1 - this->_srcPixelRod.x1, procWindowRoW.y1 - this->_srcPixelRod.y1 );
	
	correlate_rows_cols<PixelGray>(
		color_converted_view<PixelGray>( this->_srcView ),
		_params._xKernelGaussianDerivative,
		_params._xKernelGaussian,
		kth_channel_view<0>(this->_dstView),
		proc_tl,
		_params._boundary_option );
	if( progressForward( dst.height() ) )
		return;

	correlate_rows_cols<PixelGray>(
		color_converted_view<PixelGray>( this->_srcView ),
		_params._yKernelGaussian,
		_params._yKernelGaussianDerivative,
		kth_channel_view<1>(dst),
		proc_tl,
		_params._boundary_option );
	if( progressForward( dst.height() ) )
		return;

	transform_pixels_progress(
		kth_channel_view<0>(dst), // srcX
		kth_channel_view<1>(dst), // srcY
		kth_channel_view<2>(dst), // dst: gradient direction
		transform_pixel_gradientDirection_t(),
		*this
		);
	if( progressForward( dst.height() ) )
		return;
	

}

}
}
}
