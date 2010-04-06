#include <tuttle/common/math/rectOp.hpp>
#include <tuttle/common/image/gilGlobals.hpp>

#include "DiffPlugin.hpp"

namespace tuttle {
namespace plugin {
namespace quality {

template<class View>
DiffProcess<View>::DiffProcess( DiffPlugin &instance )
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
	_srcA.reset( _plugin.getSrcClipA()->fetchImage( args.time ) );
	if( !_srcA.get() )
		throw( ImageNotReadyException() );
	if( _srcA->getRowBytes( ) <= 0 )
		throw( WrongRowBytesException( ) );
	this->_srcViewA = this->getView( _srcA.get(), _plugin.getSrcClipA()->getPixelRod(args.time) );
	// clip B
	_srcB.reset( _plugin.getSrcClipB()->fetchImage( args.time ) );
	if( !_srcB.get() )
		throw( ImageNotReadyException() );
	if( _srcB->getRowBytes( ) <= 0 )
		throw( WrongRowBytesException( ) );
	this->_srcViewB = this->getView( _srcB.get(), _plugin.getSrcClipB()->getPixelRod(args.time) );

	// Make sure bit depths are the same
	if( _srcA->getPixelDepth() != this->_dst->getPixelDepth() ||
	    _srcB->getPixelDepth() != this->_dst->getPixelDepth() ||
		_srcA->getPixelComponents() != this->_dst->getPixelComponents() ||
	    _srcB->getPixelComponents() != this->_dst->getPixelComponents() )
	{
		throw( BitDepthMismatchException() );
	}
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 *
 * @param[in] procWindow  Processing window
 */
template<class View>
void DiffProcess<View>::multiThreadProcessImages( const OfxRectI& procWindow )
{
	using namespace boost::gil;
	View _srcA = subimage_view( this->_srcViewA,
							    procWindow.x1,
							    procWindow.y1,
							    procWindow.x2 - procWindow.x1,
							    procWindow.y2 - procWindow.y1 );
	View _srcB = subimage_view( this->_srcViewB,
							    procWindow.x1,
							    procWindow.y1,
							    procWindow.x2 - procWindow.x1,
							    procWindow.y2 - procWindow.y1 );
	View _dst = subimage_view( this->_dstView,
							   procWindow.x1,
							   procWindow.y1,
							   procWindow.x2 - procWindow.x1,
							   procWindow.y2 - procWindow.y1 );

	rgba32f_pixel_t paramRgbaValue;
	color_convert( psnr(_srcA, _srcB, _dst), paramRgbaValue );
	_plugin._qualityMesure->setValueAtTime( this->_renderArgs.time,
	                                        get_color( paramRgbaValue, red_t() ),
	                                        get_color( paramRgbaValue, green_t() ),
	                                        get_color( paramRgbaValue, blue_t() ),
	                                        get_color( paramRgbaValue, alpha_t() )
	                                      );
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 *
 * @param[in] procWindow  Processing window
 */
template<>
void DiffProcess<boost::gil::rgba32f_view_t>::multiThreadProcessImages( const OfxRectI& procWindow )
{
	using namespace boost::gil;
	rgba32f_view_t _srcA = subimage_view( this->_srcViewA,
							    procWindow.x1,
							    procWindow.y1,
							    procWindow.x2 - procWindow.x1,
							    procWindow.y2 - procWindow.y1 );
	rgba32f_view_t _srcB = subimage_view( this->_srcViewB,
							    procWindow.x1,
							    procWindow.y1,
							    procWindow.x2 - procWindow.x1,
							    procWindow.y2 - procWindow.y1 );
	rgba32f_view_t _dst = subimage_view( this->_dstView,
							   procWindow.x1,
							   procWindow.y1,
							   procWindow.x2 - procWindow.x1,
							   procWindow.y2 - procWindow.y1 );

	rgba32f_pixel_t paramRgbaValue;
	color_convert( psnr(color_converted_view<rgba16_pixel_t>(_srcA), color_converted_view<rgba16_pixel_t>(_srcB), color_converted_view<rgba16_pixel_t>(_dst)), paramRgbaValue );
	_plugin._qualityMesure->setValueAtTime( this->_renderArgs.time,
	                                        get_color( paramRgbaValue, red_t() ),
	                                        get_color( paramRgbaValue, green_t() ),
	                                        get_color( paramRgbaValue, blue_t() ),
	                                        get_color( paramRgbaValue, alpha_t() )
	                                      );
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 *
 * @param[in] procWindow  Processing window
 */
template<>
void DiffProcess<boost::gil::rgb32f_view_t>::multiThreadProcessImages( const OfxRectI& procWindow )
{
	using namespace boost::gil;
	rgb32f_view_t _srcA = subimage_view( this->_srcViewA,
							    procWindow.x1,
							    procWindow.y1,
							    procWindow.x2 - procWindow.x1,
							    procWindow.y2 - procWindow.y1 );
	rgb32f_view_t _srcB = subimage_view( this->_srcViewB,
							    procWindow.x1,
							    procWindow.y1,
							    procWindow.x2 - procWindow.x1,
							    procWindow.y2 - procWindow.y1 );
	rgb32f_view_t _dst = subimage_view( this->_dstView,
							   procWindow.x1,
							   procWindow.y1,
							   procWindow.x2 - procWindow.x1,
							   procWindow.y2 - procWindow.y1 );

	rgba32f_pixel_t paramRgbaValue;
	color_convert( psnr(color_converted_view<rgb16_pixel_t>(_srcA),
	                    color_converted_view<rgb16_pixel_t>(_srcB),
						color_converted_view<rgb16_pixel_t>(_dst)),
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
DiffProcess<View>::psnr(const SView & v1, const SView & v2, const SView & dst)
{
	typedef boost::gil::pixel<boost::gil::bits32f, boost::gil::layout<typename boost::gil::color_space_type<SView>::type> > Pixel32F;
	typedef typename boost::gil::channel_type<Pixel32F>::type Value32F;
	typedef typename boost::gil::channel_type<typename SView::value_type>::type SValueType;

	size_t d = (size_t)(std::pow( 2.0, sizeof( SValueType ) * 8.0 ) ) - 1;
	Pixel32F psnr = mse( v1, v2, dst );
	for( int i = 0; i < boost::gil::num_channels<Pixel32F>::type::value; ++i )
	{
		psnr[i] = Value32F( 20.0 * std::log10( d / std::sqrt(psnr[i]) ) );
	}
	return psnr;
}

template<class View>
template<class SView>
boost::gil::pixel<boost::gil::bits32f, boost::gil::layout<typename boost::gil::color_space_type<SView>::type> >
DiffProcess<View>::mse(const SView & v1, const SView & v2, const SView & dst)
{
	typedef boost::gil::pixel<boost::gil::bits32f, boost::gil::layout<typename boost::gil::color_space_type<SView>::type> > Pixel32F;
	typedef typename boost::gil::channel_type<Pixel32F>::type Value32F;
	Pixel32F veqm = tuttle::get_black<Pixel32F>();
	int w = v1.width();
	int h = v1.height();

	for( typename SView::y_coord_t y = 0; y < h; ++y )
	{
		typename SView::x_iterator itA = v1.row_begin( y );
		typename SView::x_iterator itB = v2.row_begin( y );
		typename SView::x_iterator itD = dst.row_begin( y );

		for( typename SView::x_coord_t x = 0; x < w; ++x )
		{
			for( int i = 0; i < boost::gil::num_channels<Pixel32F>::type::value; ++i)
			{
				Value32F d = (Value32F)std::abs(double((*itA)[i] - (*itB)[i]));
				(*itD)[i] = (*itA)[i];
				veqm[i] += d * d;
			}
			++itA;
			++itB;
			++itD;
		}
	}

	for( int i = 0; i < boost::gil::num_channels<Pixel32F>::type::value; ++i )
	{
		veqm[i] /= (w * h);
	}
	return veqm;
}

}
}
}
