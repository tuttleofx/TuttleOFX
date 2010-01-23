/**
 * @file LutProcess.tcc
 * @brief
 * @author
 * @date    01/10/09 12:01
 *
 */

#if defined(_MSC_VER)
	#pragma warning( disable : 4244 )
#endif

#include <tuttle/common/utils/global.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/Progress.hpp>
#include <tuttle/plugin/PluginException.hpp>
#include <tuttle/common/image/gilGlobals.hpp>

#include <cmath>
#include <vector>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/filesystem/fstream.hpp>

#include "lutEngine/lut_reader.h"
#include "lutEngine/hd3d_lut.h"
#include "lutEngine/hd3d_tetra_interpolator.h"

#include "LutProcess.hpp"
#include "LutDefinitions.hpp"

namespace tuttle {
namespace plugin {
namespace lut {

using namespace boost::gil;
using namespace boost::filesystem;

template<class View>
LutProcess<View>::LutProcess( LutPlugin& instance )
	: tuttle::plugin::ImageGilProcessor<View>( instance ),
	tuttle::plugin::Progress( instance ),
	_plugin( instance )
{
    _lut3D = &_plugin.lut3D();
}

template<class View>
void LutProcess<View>::setupAndProcess( const OFX::RenderArguments& args )
{
	try
	{
        boost::scoped_ptr< OFX::Image > src( _plugin.getSrcClip()->fetchImage( args.time ) );
        if( !src.get() )
            throw( ImageNotReadyException() );
        OfxRectI sBounds = src->getBounds();

        OFX::BitDepthEnum srcBitDepth         = src->getPixelDepth();
        OFX::PixelComponentEnum srcComponents = src->getPixelComponents();

        // Build destination view
        this->_srcView = interleaved_view( std::abs( sBounds.x2 - sBounds.x1 ), std::abs( sBounds.y2 - sBounds.y1 ),
                                           static_cast<value_t*>( src->getPixelData() ),
                                           src->getRowBytes() );

        boost::scoped_ptr<OFX::Image> dst( _plugin.getDstClip()->fetchImage( args.time ) );
        if( !dst.get() )
            throw( ImageNotReadyException() );
        OfxRectI dBounds = dst->getBounds();

        OFX::BitDepthEnum dstBitDepth         = dst->getPixelDepth();
        OFX::PixelComponentEnum dstComponents = dst->getPixelComponents();

        // Make sure bit depths are same
        if( srcBitDepth != dstBitDepth || srcComponents != dstComponents )
        {
            throw( BitDepthMismatchException() );
        }

        // Build destination view
        this->_dstView = interleaved_view( std::abs( dBounds.x2 - dBounds.x1 ), std::abs( dBounds.y2 - dBounds.y1 ),
                                           static_cast<value_t*>( dst->getPixelData() ), dst->getRowBytes() );

        // Set the render window
        this->setRenderWindow( args.renderWindow );
        // Call the base class process member
        this->process();
    }
	catch( PluginException& e )
	{
		COUT_EXCEPTION( e );
	}
}

/**
 * @brief Function called by rendering thread each time
 *        a process must be done.
 *
 * @param[in] procWindow  Processing window
 */
template<class View>
void LutProcess<View>::multiThreadProcessImages( OfxRectI procWindow )
{
	try
	{
		View src = subimage_view( this->_srcView, 
                                  procWindow.x1 - this->_renderWindow.x1,
                                  procWindow.y1  - this->_renderWindow.y1,
		                          procWindow.x2 - procWindow.x1,
		                          procWindow.y2 - procWindow.y1 );
		View dst = subimage_view( this->_dstView,
                                  procWindow.x1 - this->_renderWindow.x1,
                                  procWindow.y1  - this->_renderWindow.y1,
		                          procWindow.x2 - procWindow.x1,
		                          procWindow.y2 - procWindow.y1 );
		applyLut( dst, src );
	}
	catch( PluginException& e )
	{
		COUT_EXCEPTION( e );
	}
}

template<class View>
void LutProcess<View>::applyLut( View& dst, View& src )
{
	typedef typename View::x_iterator vIterator;
	typedef typename channel_type<View>::type dPix_t;
	size_t w = src.width();
	size_t h = src.height();

	for( size_t y = 0; y < h; ++y )
	{
		vIterator sit = src.row_begin( y );
		vIterator dit = dst.row_begin( y );
		for( size_t x = 0; x < w; ++x )
		{
			Color col = _lut3D->getColor( ( *sit )[0], ( *sit )[1], ( *sit )[2] );
			( *dit )[0] = static_cast<dPix_t>(col.x);
			( *dit )[1] = static_cast<dPix_t>(col.y);
			( *dit )[2] = static_cast<dPix_t>(col.z);
			if( dst.num_channels() > 3 )
				( *dit )[3] = channel_traits< typename channel_type< View >::type >::max_value();
			++sit;
			++dit;
		}
	}
}

}
}
}
