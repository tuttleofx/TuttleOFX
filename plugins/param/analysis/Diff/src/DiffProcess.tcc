#include "DiffPlugin.hpp"

#include <tuttle/common/math/rectOp.hpp>
#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/image/gil/basic_colors.hpp>

namespace tuttle {
namespace plugin {
namespace quality {

template<class View>
DiffProcess<View>::DiffProcess( DiffPlugin& instance )
	: ImageGilProcessor<View>( instance )
	, _plugin( instance )
{
	this->setNoMultiThreading();
}

template<class View>
void DiffProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilProcessor<View>::setup( args );

	// sources view
	// clip A
	_srcA.reset( _plugin._clipSrcA->fetchImage( args.time ) );
	if( !_srcA.get() )
		BOOST_THROW_EXCEPTION( exception::ImageNotReady() );
	if( _srcA->getRowBytes() == 0 )
		BOOST_THROW_EXCEPTION( exception::WrongRowBytes() );
	this->_srcViewA = this->getView( _srcA.get(), _plugin._clipSrcA->getPixelRod( args.time ) );
	//	_srcPixelRodA = _srcA->getRegionOfDefinition(); // bug in nuke, returns bounds
	_srcPixelRodA = _plugin._clipSrcA->getPixelRod( args.time );

	// clip B
	_srcB.reset( _plugin._clipSrcB->fetchImage( args.time ) );
	if( !_srcB.get() )
		BOOST_THROW_EXCEPTION( exception::ImageNotReady() );
	if( _srcB->getRowBytes() == 0 )
		BOOST_THROW_EXCEPTION( exception::WrongRowBytes() );
	this->_srcViewB = this->getView( _srcB.get(), _plugin._clipSrcB->getPixelRod( args.time ) );
	//	_srcPixelRodB = _srcB->getRegionOfDefinition(); // bug in nuke, returns bounds
	_srcPixelRodB = _plugin._clipSrcB->getPixelRod( args.time );

	// Make sure bit depths are the same
	if( _srcA->getPixelDepth() != this->_dst->getPixelDepth() ||
	    _srcB->getPixelDepth() != this->_dst->getPixelDepth() ||
	    _srcA->getPixelComponents() != this->_dst->getPixelComponents() ||
	    _srcB->getPixelComponents() != this->_dst->getPixelComponents() )
	{
		BOOST_THROW_EXCEPTION( exception::BitDepthMismatch() );
	}
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template<class View>
void DiffProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	OfxPointI procWindowSize = {
		procWindowRoW.x2 - procWindowRoW.x1,
		procWindowRoW.y2 - procWindowRoW.y1
	};
	View srcViewA = subimage_view( this->_srcViewA,
	                               procWindowRoW.x1 - _srcPixelRodA.x1,
	                               procWindowRoW.y1 - _srcPixelRodA.y1,
	                               procWindowSize.x,
	                               procWindowSize.y );
	View srcViewB = subimage_view( this->_srcViewB,
	                               procWindowRoW.x1 - _srcPixelRodB.x1,
	                               procWindowRoW.y1 - _srcPixelRodB.y1,
	                               procWindowSize.x,
	                               procWindowSize.y );
	View dstView = subimage_view( this->_dstView,
	                              procWindowRoW.x1 - this->_dstPixelRod.x1,
	                              procWindowRoW.y1 - this->_dstPixelRod.y1,
	                              procWindowSize.x,
	                              procWindowSize.y );

	rgba32f_pixel_t paramRgbaValue;
	color_convert( psnr( srcViewA, srcViewB, dstView ), paramRgbaValue );
	_plugin._qualityMesure->setValueAtTime( this->_renderArgs.time,
	                                        get_color( paramRgbaValue, red_t() ),
	                                        get_color( paramRgbaValue, green_t() ),
	                                        get_color( paramRgbaValue, blue_t() ),
	                                        get_color( paramRgbaValue, alpha_t() )
	                                        );
}

template<>
void DiffProcess<boost::gil::rgba32f_view_t>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	OfxPointI procWindowSize = {
		procWindowRoW.x2 - procWindowRoW.x1,
		procWindowRoW.y2 - procWindowRoW.y1
	};
	rgba32f_view_t srcViewA = subimage_view( this->_srcViewA,
	                                         procWindowRoW.x1 - _srcPixelRodA.x1,
	                                         procWindowRoW.y1 - _srcPixelRodA.y1,
	                                         procWindowSize.x,
	                                         procWindowSize.y );
	rgba32f_view_t srcViewB = subimage_view( this->_srcViewB,
	                                         procWindowRoW.x1 - _srcPixelRodB.x1,
	                                         procWindowRoW.y1 - _srcPixelRodB.y1,
	                                         procWindowSize.x,
	                                         procWindowSize.y );
	rgba32f_view_t dstView = subimage_view( this->_dstView,
	                                        procWindowRoW.x1 - this->_dstPixelRod.x1,
	                                        procWindowRoW.y1 - this->_dstPixelRod.y1,
	                                        procWindowSize.x,
	                                        procWindowSize.y );

	rgba32f_pixel_t paramRgbaValue;
	color_convert( psnr( color_converted_view<rgba16_pixel_t>( srcViewA ), color_converted_view<rgba16_pixel_t>( srcViewB ), color_converted_view<rgba16_pixel_t>( dstView ) ), paramRgbaValue );
	_plugin._qualityMesure->setValueAtTime( this->_renderArgs.time,
	                                        get_color( paramRgbaValue, red_t() ),
	                                        get_color( paramRgbaValue, green_t() ),
	                                        get_color( paramRgbaValue, blue_t() ),
	                                        get_color( paramRgbaValue, alpha_t() )
	                                        );
}

template<>
void DiffProcess<boost::gil::rgb32f_view_t>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	OfxPointI procWindowSize = {
		procWindowRoW.x2 - procWindowRoW.x1,
		procWindowRoW.y2 - procWindowRoW.y1
	};
	rgb32f_view_t srcViewA = subimage_view( this->_srcViewA,
	                                        procWindowRoW.x1 - _srcPixelRodA.x1,
	                                        procWindowRoW.y1 - _srcPixelRodA.y1,
	                                        procWindowSize.x,
	                                        procWindowSize.y );
	rgb32f_view_t srcViewB = subimage_view( this->_srcViewB,
	                                        procWindowRoW.x1 - _srcPixelRodB.x1,
	                                        procWindowRoW.y1 - _srcPixelRodB.y1,
	                                        procWindowSize.x,
	                                        procWindowSize.y );
	rgb32f_view_t dstView = subimage_view( this->_dstView,
	                                       procWindowRoW.x1 - this->_dstPixelRod.x1,
	                                       procWindowRoW.y1 - this->_dstPixelRod.y1,
	                                       procWindowSize.x,
	                                       procWindowSize.y );

	rgba32f_pixel_t paramRgbaValue;
	color_convert( psnr( color_converted_view<rgb16_pixel_t>( srcViewA ),
	                     color_converted_view<rgb16_pixel_t>( srcViewB ),
	                     color_converted_view<rgb16_pixel_t>( dstView ) ),
	               paramRgbaValue );
	_plugin._qualityMesure->setValueAtTime( this->_renderArgs.time,
	                                        get_color( paramRgbaValue, red_t() ),
	                                        get_color( paramRgbaValue, green_t() ),
	                                        get_color( paramRgbaValue, blue_t() ),
	                                        get_color( paramRgbaValue, alpha_t() )
	                                        );
}

template<class View>
template<class SView>
boost::gil::pixel<boost::gil::bits32f, boost::gil::layout<typename boost::gil::color_space_type<SView>::type> >
DiffProcess<View>::psnr( const SView& v1, const SView& v2, const SView& dst )
{
	typedef boost::gil::pixel<boost::gil::bits32f, boost::gil::layout<typename boost::gil::color_space_type<SView>::type> > Pixel32F;
	typedef typename boost::gil::channel_type<Pixel32F>::type Value32F;
	typedef typename boost::gil::channel_type<typename SView::value_type>::type SValueType;

	size_t d      = (size_t)( std::pow( 2.0, sizeof( SValueType ) * 8.0 ) ) - 1;
	Pixel32F psnr = mse( v1, v2, dst );
	for( int i = 0; i < boost::gil::num_channels<Pixel32F>::type::value; ++i )
	{
		psnr[i] = Value32F( 20.0 * std::log10( d / std::sqrt( psnr[i] ) ) );
	}
	return psnr;
}

template<class View>
template<class SView>
boost::gil::pixel<boost::gil::bits32f, boost::gil::layout<typename boost::gil::color_space_type<SView>::type> >
DiffProcess<View>::mse( const SView& v1, const SView& v2, const SView& dst )
{
	typedef boost::gil::pixel<boost::gil::bits32f, boost::gil::layout<typename boost::gil::color_space_type<SView>::type> > Pixel32F;
	typedef typename boost::gil::channel_type<Pixel32F>::type Value32F;
	Pixel32F veqm = get_black<Pixel32F>();
	int w         = v1.width();
	int h         = v1.height();

	for( typename SView::y_coord_t y = 0; y < h; ++y )
	{
		typename SView::x_iterator itA = v1.row_begin( y );
		typename SView::x_iterator itB = v2.row_begin( y );
		typename SView::x_iterator itD = dst.row_begin( y );

		for( typename SView::x_coord_t x = 0; x < w; ++x )
		{
			for( int i = 0; i < boost::gil::num_channels<Pixel32F>::type::value; ++i )
			{
				Value32F d = ( Value32F ) std::abs( double( ( *itA )[i] - ( *itB )[i]) );
				( *itD )[i] = ( *itA )[i];
				veqm[i]    += d * d;
			}
			++itA;
			++itB;
			++itD;
		}
	}

	for( int i = 0; i < boost::gil::num_channels<Pixel32F>::type::value; ++i )
	{
		veqm[i] /= ( w * h );
	}
	return veqm;
}

}
}
}
