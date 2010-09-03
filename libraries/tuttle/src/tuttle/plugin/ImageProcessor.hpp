#ifndef _TUTTLE_PLUGIN_IMAGEPROCESSOR_HPP_
#define _TUTTLE_PLUGIN_IMAGEPROCESSOR_HPP_

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

/** @file This file contains a useful base class that can be used to process images
 *
 * The code below is not so much a skin on the base OFX classes, but code used in implementing
 * specific image processing algorithms. As such it does not sit in the support include lib, but in
 * its own include directory.
 */
namespace tuttle {
namespace plugin {

////////////////////////////////////////////////////////////////////////////////
// base class to process images with
class ImageProcessor : public OFX::MultiThread::Processor
{
protected:
	OFX::ImageEffect& _effect;      /**< @brief effect to render with */
	OFX::Image* _dstImg;       /**< @brief image to process into */
	OfxRectI _renderWindow; /**< @brief render window to use */

public:
	/** @brief ctor */
	ImageProcessor( OFX::ImageEffect& effect )
		: _effect( effect )
		, _dstImg( 0 )
	{
		_renderWindow.x1 = _renderWindow.y1 = _renderWindow.x2 = _renderWindow.y2 = 0;
	}

	/** @brief set the destination image */
	void setDstImg( OFX::Image* v ) { _dstImg = v; }

	/** @brief reset the render window */
	void setRenderWindow( OfxRectI rect ) { _renderWindow = rect; }

	/** @brief overridden from OFX::MultiThread::Processor. This function is called once on each SMP thread by the base class */
	void multiThreadFunction( unsigned int threadId, unsigned int nThreads )
	{
		// slice the y range into the number of threads it has
		unsigned int dy   = _renderWindow.y2 - _renderWindow.y1;
		unsigned int y1   = _renderWindow.y1 + threadId * dy / nThreads;
		unsigned int step = ( threadId + 1 ) * dy / nThreads;
		unsigned int y2   = _renderWindow.y1 + ( step < dy ? step : dy );
		OfxRectI win      = _renderWindow;

		win.y1 = y1; win.y2 = y2;

		// and render that thread on each
		multiThreadProcessImages( win );
	}

	/** @brief called before any MP is done */
	virtual void preProcess( void ) {}

	/** @brief this is called by multiThreadFunction to actually process images, override in derived classes */
	virtual void multiThreadProcessImages( const OfxRectI& window ) = 0;

	/** @brief called before any MP is done */
	virtual void postProcess( void ) {}

	/** @brief called to process everything */
	virtual void process( void )
	{
		// is it OK ?
		if( !_dstImg || ( _renderWindow.x2 - _renderWindow.x1 == 0 && _renderWindow.y2 - _renderWindow.y1 ) )
			return;

		// call the pre MP pass
		preProcess();

		// call the base multi threading code, should put a pre & post thread calls in too
		multiThread();

		// call the post MP pass
		postProcess();
	}

};

}
}
#endif
