/*
 * @file   ImageGilProcessor.hpp
 * @author edubois
 * @brief  This file contains a useful base class that can be used to process images of any type
 * @date   2009/05/29
 */

#ifndef _TUTTLE_PLUGIN_IMAGEGILPROCESSOR_HPP
#define _TUTTLE_PLUGIN_IMAGEGILPROCESSOR_HPP

#include "ofxsImageEffect.h"
#include "ofxsMultiThread.h"
#include "ofxsUtilities.h"
#include "PluginException.hpp"

#include <cstdlib>

namespace tuttle {
namespace plugin {

/// @brief Base class to process images with

template <class View>
class ImageGilProcessor : public OFX::MultiThread::Processor
{
protected:
	OFX::ImageEffect& _effect; ///< @brief effect to render with
	View _dstView; ///< @brief image to process into
	OfxRectI _renderWindow; ///< @brief render window to use

public:
	/** @brief ctor */
	ImageGilProcessor( OFX::ImageEffect& effect );

	/** @brief set the destination image */
	void setDstImg( View& v ) { _dstView = v; }

	/** @brief reset the render window */
	void setRenderWindow( OfxRectI rect ) { _renderWindow = rect; }

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

	/** @brief called before any MP is done */
	virtual void preProcess() {}

	/** @brief this is called by multiThreadFunction to actually process images, override in derived classes */
	virtual void multiThreadProcessImages( OfxRectI window ) = 0;

	/** @brief called before any MP is done */
	virtual void postProcess() {}

	/** @brief called to process everything */
	virtual void process();
};

template <class View>
ImageGilProcessor<View>::ImageGilProcessor( OFX::ImageEffect& effect )
	: _effect( effect )
{
	_renderWindow.x1 = _renderWindow.y1 = _renderWindow.x2 = _renderWindow.y2 = 0;
}

template <class View>
void ImageGilProcessor<View>::process( void )
{
	COUT( "process->renderwindow : x1:"<<_renderWindow.x1<<" y1:"<<_renderWindow.y1<<" x2:"<<_renderWindow.x2<<" y2:"<<_renderWindow.y2 );
	// is it OK ?
	if( _renderWindow.x2 - _renderWindow.x1 == 0 || _renderWindow.y2 - _renderWindow.y1 == 0 )
	{
		throw PluginException("RenderWindow empty !");
	}
	// call the pre MP pass
	preProcess();

	// call the base multi threading code, should put a pre & post thread calls in too
	multiThread();

	// call the post MP pass
	postProcess();
}

}
}

#endif  /* _IMAGEGILPROCESSOR_HPP */

