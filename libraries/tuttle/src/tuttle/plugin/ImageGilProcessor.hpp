#ifndef _TUTTLE_PLUGIN_IMAGEGILPROCESSOR_HPP_
#define _TUTTLE_PLUGIN_IMAGEGILPROCESSOR_HPP_

#include "ofxsImageEffect.h"
#include "ofxsMultiThread.h"
#include "ofxsUtilities.h"
#include "PluginException.hpp"
#include "Progress.hpp"

#include <tuttle/common/image/gilGlobals.hpp>

#include <cstdlib>
#include <vector>

namespace tuttle {
namespace plugin {

/// @brief Base class to process images with

/**
 * @brief Base class that can be used to process images of any type using boost::gil library view to access images.
 */
template <class View>
class ImageGilProcessor : public OFX::MultiThread::Processor, public tuttle::plugin::Progress
{
public:
    typedef typename View::value_type Pixel;
    typedef typename image_from_view<View>::type Image;
protected:
	OFX::ImageEffect& _effect; ///< @brief effect to render with
	View _dstView; ///< @brief image to process into
	OfxRectI _renderWindow; ///< @brief render window to use

public:
	/** @brief ctor */
	ImageGilProcessor( OFX::ImageEffect& effect );

    /** @brief called before any MP is done */
    virtual void preProcess(void) { progressBegin( _renderWindow.y2 - _renderWindow.y1 ); }

    /** @brief called before any MP is done */
    virtual void postProcess(void) { progressEnd(); }

	virtual void setup( const OFX::RenderArguments& args ){}

	/** @brief fetch output and inputs clips */
	virtual void setupAndProcess( const OFX::RenderArguments& args )
	{
		_renderWindow = args.renderWindow;
		try
		{
			setup( args );
		}
		catch( ImageNotReadyException& e )
		{
			// stop the process but don't display an error
			progressEnd( );
			return;
		}
		catch( PluginException& e )
		{
			COUT_EXCEPTION( e );
			progressEnd( );
			return;
		}

		// Call the base class process member
		this->process();
	}

	View getView( OFX::Image* img, const OfxRectI& rod );

	/** @brief overridden from OFX::MultiThread::Processor. This function is called once on each SMP thread by the base class */
	void multiThreadFunction( unsigned int threadId, unsigned int nThreads )
	{
		// slice the y range into the number of threads it has
		int dy = std::abs( _renderWindow.y2 - _renderWindow.y1 );
		int y1 = _renderWindow.y1 + threadId * dy / nThreads;
		int step = ( threadId + 1 ) * dy / nThreads;
		int y2   = _renderWindow.y1 + ( step < dy ? step : dy );

		OfxRectI win = _renderWindow;
		win.y1 = y1;
		win.y2 = y2;

		// and render that thread on each
		multiThreadProcessImages( win );
	}

	/** @brief this is called by multiThreadFunction to actually process images, override in derived classes */
	virtual void multiThreadProcessImages( const OfxRectI& window ) = 0;

	/** @brief called to process everything */
	virtual void process();
};

template <class View>
ImageGilProcessor<View>::ImageGilProcessor( OFX::ImageEffect& effect )
	: Progress( effect )
	, _effect( effect )
{
	_renderWindow.x1 = _renderWindow.y1 = _renderWindow.x2 = _renderWindow.y2 = 0;
}

template <class View>
void ImageGilProcessor<View>::process( void )
{
	// is it OK ?
	if( _renderWindow.x2 - _renderWindow.x1 == 0 || _renderWindow.y2 - _renderWindow.y1 == 0 )
	{
		throw PluginException( "RenderWindow empty !" );
	}
	// call the pre MP pass
	preProcess();

	// call the base multi threading code, should put a pre & post thread calls in too
	multiThread();

	// call the post MP pass
	postProcess();
}

template<class View>
View ImageGilProcessor<View>::getView( OFX::Image* img, const OfxRectI& rod )
{
	using namespace boost::gil;

	//OfxRectI srcImgRod = _src->getRegionOfDefinition();
	OfxRectI imgBounds = img->getBounds();
	point2<int> tileSize = point2<int>( imgBounds.x2 - imgBounds.x1,
											 imgBounds.y2 - imgBounds.y1 );

	// Build views
	View srcTileView = interleaved_view( tileSize.x, tileSize.y,
										 static_cast<Pixel*>( img->getPixelData() ),
										 img->getRowBytes( ) );

	// if the tile is the full image
	// directly return the tile
	if( imgBounds.x1 == 0 && imgBounds.y1 == 0 &&
	    imgBounds.x2 == rod.x2 && imgBounds.y2 == rod.y2 )
		return srcTileView;

	// view the tile as a full image
	return subimage_view( srcTileView, -imgBounds.x1, -imgBounds.y1, static_cast<int>(rod.x2-rod.x1), static_cast<int>(rod.y2-rod.y1) );
}


}
}

#endif

