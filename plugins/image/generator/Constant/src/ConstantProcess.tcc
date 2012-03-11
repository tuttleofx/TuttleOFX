#include "ConstantProcess.hpp"

#include <tuttle/plugin/ofxToGil/point.hpp>

#include <boost/numeric/conversion/cast.hpp>

namespace tuttle {
namespace plugin {
namespace constant {

template<class View>
ConstantProcess<View>::ConstantProcess( ConstantPlugin& instance )
	: ImageGilProcessor<View>( instance, eImageOrientationIndependant )
	, _plugin( instance ) {}

template<class View>
ConstantParams<View> ConstantProcess<View>::getParams()
{
	using namespace boost::gil;
	ConstantParams<View> params;

	OfxRGBAColourD c = _plugin._color->getValue();
	color_convert( rgba32f_pixel_t( c.r, c.g, c.b, c.a ), params._color );
	return params;
}

template<class View>
void ConstantProcess<View>::setup( const OFX::RenderArguments& args )
{
	using namespace boost::gil;
	ImageGilProcessor<View>::setup( args );

	boost::function_requires<PixelLocatorConcept<Locator> >();
	gil_function_requires < StepIteratorConcept<typename Locator::x_iterator> >();

	// params
	ConstantParams<View> params = getParams();

	OfxRectD rod = _plugin._clipDst->getCanonicalRod( args.time );
	Point dims( rod.x2 - rod.x1, rod.y2 - rod.y1 );
	int yshift = boost::numeric_cast<int>( ( dims.x - dims.y ) * 0.5 );

	// create a squared constant plane
	ConstantVirtualView constant( Point( dims.x, dims.x ), Locator( Point( 0, 0 ), Point( 1, 1 ), ConstantFunctorT( params._color ) ) );
	// create a subview depending on the image ratio
	_srcView = subimage_view<>( constant, 0, yshift, boost::numeric_cast<int>( dims.x ), boost::numeric_cast<int>( dims.y ) );
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template<class View>
void ConstantProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	const OfxPointI procWindowSize = {
		procWindowRoW.x2 - procWindowRoW.x1,
		procWindowRoW.y2 - procWindowRoW.y1 };

	for( int y = procWindowOutput.y1;
	     y < procWindowOutput.y2;
	     ++y )
	{
		typename ConstantVirtualView::x_iterator src_it     = this->_srcView.x_at( procWindowOutput.x1, y );
		typename View::x_iterator dst_it                    = this->_dstView.x_at( procWindowOutput.x1, y );
		for( int x = procWindowOutput.x1;
		     x < procWindowOutput.x2;
		     ++x, ++src_it, ++dst_it )
		{
			*dst_it = *src_it;
		}
		if( this->progressForward( procWindowSize.x ) )
			return;
	}
}

}
}
}
