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
	// destination view
	_dst.reset( _plugin.getDstClip()->fetchImage( args.time ) );
	if( !_dst.get() )
		throw( ImageNotReadyException() );
	if( _dst->getRowBytes( ) <= 0 )
		throw( WrongRowBytesException( ) );
	this->_dstView = this->getView( _dst.get(), _plugin.getDstClip()->getPixelRod(args.time) );

	// Make sure bit depths are the same
	if( _srcA->getPixelDepth() != _dst->getPixelDepth() ||
	    _srcB->getPixelDepth() != _dst->getPixelDepth() ||
		_srcA->getPixelComponents() != _dst->getPixelComponents() ||
	    _srcB->getPixelComponents() != _dst->getPixelComponents() )
	{
		throw( BitDepthMismatchException() );
	}
	this->_renderArgs = args;
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
	rgba32f_pixel_t paramRgbaValue;
	View _srcA = subimage_view( this->_srcViewA,
							   procWindow.x1 - this->_renderWindow.x1,
							   procWindow.y1 - this->_renderWindow.y1,
							   procWindow.x2 - procWindow.x1,
							   procWindow.y2 - procWindow.y1 );
	View _srcB = subimage_view( this->_srcViewB,
							   procWindow.x1 - this->_renderWindow.x1,
							   procWindow.y1 - this->_renderWindow.y1,
							   procWindow.x2 - procWindow.x1,
							   procWindow.y2 - procWindow.y1 );
	View _dst = subimage_view( this->_dstView,
							  procWindow.x1 - this->_renderWindow.x1,
							  procWindow.y1 - this->_renderWindow.y1,
							  procWindow.x2 - procWindow.x1,
							  procWindow.y2 - procWindow.y1 );

	color_convert( psnr(_srcA, _srcB, _dst), paramRgbaValue );
	_plugin._qualityMesure->setValueAtTime( this->_renderArgs.time,
	                                        get_color( paramRgbaValue, red_t() ),
	                                        get_color( paramRgbaValue, green_t() ),
	                                        get_color( paramRgbaValue, blue_t() ),
	                                        get_color( paramRgbaValue, alpha_t() )
	                                      );
}

template<class View>
typename DiffProcess<View>::PixelF DiffProcess<View>::psnr(const View & v1, const View & v2, const View & dst)
{
	size_t d = (size_t)(std::pow( 2.0, sizeof( float ) * 8.0 ) ) - 1;
	PixelF psnr = mse( v1, v2, dst );
	for( int i = 0; i < boost::gil::num_channels<PixelF>::type::value; ++i )
	{
		psnr[i] = 10.0 * std::log( ( d * d ) / psnr[i] ) / std::log( 10.0 );
	}
	return psnr;
}

template<class View>
typename DiffProcess<View>::PixelF DiffProcess<View>::mse(const View & v1, const View & v2, const View & dst)
{
	PixelF veqm = tuttle::get_black<PixelF>();
	int w = v1.width();
	int h = v2.height();

	for( typename View::y_coord_t y = 0; y < h; ++y )
	{
		typename View::x_iterator itA = v1.row_begin( y );
		typename View::x_iterator itB = v2.row_begin( y );
		typename View::x_iterator itD = dst.row_begin( y );

		for( typename View::x_coord_t x = 0; x < w; ++x )
		{
			for( int i = 0; i < boost::gil::num_channels<PixelF>::type::value; ++i)
			{
				double d = std::abs((*itA)[i] - (*itA)[i]);
				(*itD)[i] = (*itA)[i];
				veqm[i] += d * d;
			}
			++itA;
			++itB;
			++itD;
		}
	}

	for( int i = 0; i < boost::gil::num_channels<PixelF>::type::value; ++i )
	{
		veqm[i] /= w * h;
	}
	return veqm;
}

}
}
}
