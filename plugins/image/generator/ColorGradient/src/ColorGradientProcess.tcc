
#include <boost/numeric/conversion/cast.hpp>

namespace tuttle {
namespace plugin {
namespace colorGradient {

template<class View, template<typename> class ColorGradientFunctor>
ColorGradientProcess<View, ColorGradientFunctor>::ColorGradientProcess( ColorGradientPlugin &instance )
: ImageGilFilterProcessor<View>( instance )
, _plugin( instance )
{
}

template<class View, template<typename> class ColorGradientFunctor>
void ColorGradientProcess<View, ColorGradientFunctor>::setup( const OFX::RenderArguments &args )
{
	using namespace boost::gil;

	// destination view
	boost::scoped_ptr<OFX::Image> dst( _plugin._dstClip->fetchImage( args.time ) );
	if( !dst.get( ) )
		throw( ImageNotReadyException( ) );
	this->_dstView = this->getView( dst.get( ), _plugin._dstClip->getPixelRod( args.time ) );

	boost::function_requires<PixelLocatorConcept<Locator> >( );
	gil_function_requires < StepIteratorConcept<typename Locator::x_iterator> >( );

	// params
	ColorGradientProcessParams<View> params = _plugin.getProcessParams<View>();

	OfxRectD rod = _plugin._dstClip->getCanonicalRod( args.time );
	Point dims( rod.x2 - rod.x1, rod.y2 - rod.y1 );
	int yshift = boost::numeric_cast<int>(( dims.x - dims.y ) * 0.5);

	ColorGradientVirtualView colorGradient( Point( dims.x, dims.x ), Locator( Point( 0, 0 ), Point( 1, 1 ), ColorGradientFunctorT( dims, params._points, params._colors ) ) );
	// create a subview depending on the image ratio
	_srcView = subimage_view<>( colorGradient, 0, yshift, boost::numeric_cast<int>(dims.x), boost::numeric_cast<int>(dims.y) );
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 *
 * @param[in] procWindow  Processing window
 */
template<class View, template<typename> class ColorGradientFunctor>
void ColorGradientProcess<View, ColorGradientFunctor>::multiThreadProcessImages( const OfxRectI& procWindow )
{
	using namespace boost::gil;

	for( int y = procWindow.y1;
		 y < procWindow.y2;
		 ++y )
	{
		typename ColorGradientVirtualView::x_iterator src_it = this->_srcView.x_at( procWindow.x1, y );
		typename View::x_iterator dst_it = this->_dstView.x_at( procWindow.x1, y );
		for( int x = procWindow.x1;
			 x < procWindow.x2;
			 ++x, ++src_it, ++dst_it )
		{
			*dst_it = *src_it;
		}
		if( this->progressForward() )
			return;
	}
}

}
}
}
