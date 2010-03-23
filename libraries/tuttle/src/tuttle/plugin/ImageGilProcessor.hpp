#ifndef _TUTTLE_PLUGIN_IMAGEGILPROCESSOR_HPP_
#define _TUTTLE_PLUGIN_IMAGEGILPROCESSOR_HPP_

#include "ofxsImageEffect.h"
#include "ofxsMultiThread.h"
#include "ofxsUtilities.h"
#include "PluginException.hpp"
#include "Progress.hpp"

#include <tuttle/common/image/gilGlobals.hpp>

#include <boost/scoped_ptr.hpp>

#include <cstdlib>
#include <vector>

namespace tuttle {
namespace plugin {

template<class View>
View getView( OFX::Image* img, const OfxRectI& rod );


/**
 * @brief Base class that can be used to process images of any type using boost::gil library view to access images.
 */
template <class View>
class ImageGilProcessor : public OFX::MultiThread::Processor, public tuttle::plugin::Progress
{
public:
    typedef typename View::value_type Pixel;
    typedef typename image_from_view<View>::type Image;
private:
	unsigned int _nbThreads;
protected:
	OFX::ImageEffect& _effect; ///< @brief effect to render with
	OfxRectI _renderWindow; ///< @brief render window to use
	View _dstView; ///< @brief image to process into
	boost::scoped_ptr<OFX::Image> _dst;

public:
	/** @brief ctor */
	ImageGilProcessor( OFX::ImageEffect& effect );
	virtual ~ImageGilProcessor();

	void setNoMultiThreading() { _nbThreads = 1; }
	void setNbThreads( const unsigned int nbThreads ) { _nbThreads = nbThreads; }
	void setNbThreadsAuto() { _nbThreads = 0; }
	
    /** @brief called before any MP is done */
    virtual void preProcess() { progressBegin( _renderWindow.y2 - _renderWindow.y1 ); }

    /** @brief called before any MP is done */
    virtual void postProcess() { progressEnd(); }

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
			COUT_ERROR( "ImageNotReadyException" );
			progressEnd( );
			return;
		}
		catch( PluginException& e )
		{
			COUT_EXCEPTION( e );
			progressEnd( );
			return;
		}
		catch( ... )
		{
			COUT_ERROR( "Unknown exception." );
			progressEnd( );
			return;
		}

		// Call the base class process member
		this->process();
	}

	View getView( OFX::Image* img, const OfxRectI& rod )
	{
		return tuttle::plugin::getView<View>( img, rod );
	}

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
	, _nbThreads( 0 ) // auto, maximum allowable number of CPUs will be used
	, _effect( effect )
{
	_renderWindow.x1 = _renderWindow.y1 = _renderWindow.x2 = _renderWindow.y2 = 0;
}

template <class View>
ImageGilProcessor<View>::~ImageGilProcessor()
{
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
	multiThread( _nbThreads );

	// call the post MP pass
	postProcess();
}

template<class View>
View getView( OFX::Image* img, const OfxRectI& rod )
{
	using namespace boost::gil;
    typedef typename View::value_type Pixel;

	//OfxRectI rod = img->getRegionOfDefinition();
	OfxRectI bounds = img->getBounds();
	point2<int> tileSize = point2<int>( bounds.x2 - bounds.x1,
										bounds.y2 - bounds.y1 );

	// Build views
	View tileView = interleaved_view( tileSize.x, tileSize.y,
									  static_cast<Pixel*>( img->getPixelData() ),
									  img->getRowBytes( ) );

	// if the tile is equals to the full image
	// directly return the tile
	if( bounds.x1 == rod.x1 && bounds.y1 == rod.y1 &&
	    bounds.x2 == rod.x2 && bounds.y2 == rod.y2 )
		return tileView;

	// view the tile as a full image
	return subimage_view( tileView, -bounds.x1, -bounds.y1, rod.x2-rod.x1, rod.y2-rod.y1 );
}


}
}

#endif

