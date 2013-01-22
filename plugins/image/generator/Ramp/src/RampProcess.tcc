#include "RampAlgorithm.hpp"
#include "RampPlugin.hpp"

namespace tuttle {
namespace plugin {
namespace ramp {

template<class View>
RampProcess<View>::RampProcess( RampPlugin &effect )
    : ImageGilProcessor<View>( effect, eImageOrientationIndependant )
    , _plugin( effect )
{
}

template<class View>
void RampProcess<View>::setup( const OFX::RenderArguments& args )
{
    using namespace terry;
    ImageGilProcessor<View>::setup( args );
    RampProcessParams _params = _plugin.getProcessParams( args.renderScale );

    boost::function_requires<PixelLocatorConcept<Locator> >();
    gil_function_requires < StepIteratorConcept<typename Locator::x_iterator> >();

    OfxRectD rod = _plugin._clipDst->getCanonicalRod( args.time );
    Point dims( rod.x2 - rod.x1, rod.y2 - rod.y1 );

    float size = std::min( dims.x, dims.y );
    Point tileSize( size, size );


    int xshift = 0.0;
    int yshift = 0.0;

    if( size == dims.x )
        yshift = - boost::numeric_cast<int>( ( dims.y - dims.x ) * 0.5 );
    else
        xshift = - boost::numeric_cast<int>( ( dims.x - dims.y ) * 0.5 );

    // create a squared color wheel
    RampVirtualView checker( Point( size, size ), Locator( Point( 0, 0 ), Point( 1, 1 ), RampFunctorT( dims.x, dims.y, _params.direction, _params.color ) ) );
    // create a subview depending on the image ratio
    _srcView = subimage_view<>( checker, xshift, yshift, boost::numeric_cast<int>( dims.x ), boost::numeric_cast<int>( dims.y ) );
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template<class View>
void RampProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
    using namespace boost::gil;
    OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
    const OfxPointI procWindowSize = {
        procWindowRoW.x2 - procWindowRoW.x1,
        procWindowRoW.y2 - procWindowRoW.y1
    };

    for( int y = procWindowOutput.y1;
             y < procWindowOutput.y2;
             ++y )
    {
        typename RampVirtualView::x_iterator src_it = this->_srcView.x_at( procWindowOutput.x1, y );
        typename View::x_iterator dst_it = this->_dstView.x_at( procWindowOutput.x1, y );
        for( int x = procWindowOutput.x1;
             x < procWindowOutput.x2;
             ++x, ++src_it, ++dst_it )
        {
            (*dst_it) = (*src_it);
        }
        if( this->progressForward( procWindowSize.x ) )
            return;
    }
}

}
}
}
