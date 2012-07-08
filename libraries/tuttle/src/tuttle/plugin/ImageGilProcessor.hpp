#ifndef _TUTTLE_PLUGIN_IMAGEGILPROCESSOR_HPP_
#define _TUTTLE_PLUGIN_IMAGEGILPROCESSOR_HPP_

#include "exceptions.hpp"
#include "OfxProgress.hpp"

#include <terry/globals.hpp>
#include <tuttle/plugin/ofxToGil/image.hpp>
#include <tuttle/plugin/exceptions.hpp>
#include <tuttle/common/math/rectOp.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <ofxsUtilities.h>

#include <boost/scoped_ptr.hpp>
#include <boost/exception/info.hpp>
#include <boost/exception/error_info.hpp>
#include <boost/throw_exception.hpp>

#include <cstdlib>
#include <vector>

namespace tuttle {
namespace plugin {

/**
 * @brief Base class that can be used to process images of any type using boost::gil library view to access images.
 */
template <class View>
class ImageGilProcessor : public OFX::MultiThread::Processor
	, public tuttle::plugin::OfxProgress
{
public:
	typedef typename View::value_type Pixel;
	typedef typename terry::image_from_view<View>::type Image;

protected:
	OFX::ImageEffect& _effect; ///< effect to render with
	OFX::RenderArguments _renderArgs; ///< render arguments
	OFX::Clip* _clipDst;       ///< Destination image clip
	boost::scoped_ptr<OFX::Image> _dst;
	OfxRectI _dstPixelRod;
	OfxPointI _dstPixelRodSize;
	OfxPointI _renderWindowSize;
	View _dstView; ///< image to process into

private:
	unsigned int _nbThreads;
	EImageOrientation _imageOrientation;

public:
	/** @brief ctor */
	ImageGilProcessor( OFX::ImageEffect& effect, const EImageOrientation imageOrientation );
	virtual ~ImageGilProcessor();

	void setNoMultiThreading()                        { _nbThreads = 1; }
	void setNbThreads( const unsigned int nbThreads ) { _nbThreads = nbThreads; }
	void setNbThreadsAuto()                           { _nbThreads = 0; }

	/** @brief called before any MP is done */
	virtual void preProcess() { progressBegin( _renderWindowSize.y * _renderWindowSize.x ); }

	/** @brief called before any MP is done */
	virtual void postProcess() { progressEnd(); }

	virtual void setup( const OFX::RenderArguments& args )
	{
		// destination view
//		TUTTLE_COUT_INFOS;
//		TUTTLE_COUT_VAR( "dst - fetchImage " << time );
		_dst.reset( _clipDst->fetchImage( args.time ) );
		if( !_dst.get() )
			BOOST_THROW_EXCEPTION( exception::ImageNotReady()
				<< exception::dev() + "Error on clip " + quotes(_clipDst->name()) );
		if( _dst->getRowDistanceBytes() == 0 )
			BOOST_THROW_EXCEPTION( exception::WrongRowBytes()
				<< exception::dev() + "Error on clip " + quotes(_clipDst->name()) );
		
		//		_dstPixelRod = _dst->getRegionOfDefinition(); // bug in nuke, returns bounds
		_dstPixelRod       = _clipDst->getPixelRod( args.time, args.renderScale );
		_dstPixelRodSize.x = ( this->_dstPixelRod.x2 - this->_dstPixelRod.x1 );
		_dstPixelRodSize.y = ( this->_dstPixelRod.y2 - this->_dstPixelRod.y1 );
		_dstView           = getView( _dst.get(), _dstPixelRod );

		#ifndef TUTTLE_PRODUCTION
		// init dst buffer with red to highlight uninitialized pixels
		const OfxRectI dstBounds = this->translateRoWToOutputClipCoordinates( _dst->getBounds() );
		View dstToFill           = boost::gil::subimage_view( _dstView,
		                                                      dstBounds.x1, dstBounds.y1,
		                                                      dstBounds.x2 - dstBounds.x1, dstBounds.y2 - dstBounds.y1 );
		const boost::gil::rgba32f_pixel_t errorColor( 1.0, 0.0, 0.0, 1.0 );
		fill_pixels( dstToFill, errorColor );
		#endif
	}

	/** @brief fetch output and inputs clips */
	virtual void setupAndProcess( const OFX::RenderArguments& args )
	{
		_renderArgs = args;
		_renderWindowSize.x = ( _renderArgs.renderWindow.x2 - _renderArgs.renderWindow.x1 );
		_renderWindowSize.y = ( _renderArgs.renderWindow.y2 - _renderArgs.renderWindow.y1 );
		try
		{
			setup( args );
		}
		catch( exception::Common& )
		{
			progressEnd();

			// if the host is trying to abort the rendering return without error
			if( _effect.abort() )
			{
				return;
			}
			throw;
		}
		catch(...)
		{
			progressEnd();

			// if the host is trying to abort the rendering return without error
			if( _effect.abort() )
			{
				return;
			}
			throw;
		}

		// Call the base class process member
		this->process();
	}

	/**
	 * @brief Return a full gil view of an image.
	 */
	View getView( OFX::Image* img, const OfxRectI& rod ) const
	{
		return tuttle::plugin::getGilView<View>( img, rod, _imageOrientation );
	}
	template<typename CustomView>
	CustomView getCustomView( OFX::Image* img, const OfxRectI& rod ) const
	{
		return tuttle::plugin::getGilView<CustomView>( img, rod, _imageOrientation );
	}

	/** @brief overridden from OFX::MultiThread::Processor. This function is called once on each SMP thread by the base class */
	void multiThreadFunction( const unsigned int threadId, const unsigned int nThreads )
	{
		// slice the y range into the number of threads it has
		const int dy   = std::abs( _renderArgs.renderWindow.y2 - _renderArgs.renderWindow.y1 );
		const int y1   = _renderArgs.renderWindow.y1 + threadId * dy / nThreads;
		const int step = ( threadId + 1 ) * dy / nThreads;
		const int y2   = _renderArgs.renderWindow.y1 + ( step < dy ? step : dy );

		OfxRectI winRoW = _renderArgs.renderWindow;
		winRoW.y1 = y1;
		winRoW.y2 = y2;

		// and render that thread on each
		multiThreadProcessImages( winRoW );
	}

	/** @brief this is called by multiThreadFunction to actually process images, override in derived classes */
	virtual void multiThreadProcessImages( const OfxRectI& windowRoW ) = 0;

	// to output clip coordinates
	OfxRectI translateRoWToOutputClipCoordinates( const OfxRectI& windowRoW ) const
	{
		return translateRegion( windowRoW, _dstPixelRod );
	}

	/** @brief called to process everything */
	virtual void process();
};

template <class View>
ImageGilProcessor<View>::ImageGilProcessor( OFX::ImageEffect& effect, const EImageOrientation imageOrientation )
	: OfxProgress( effect )
	, _effect( effect )
	, _nbThreads( 0 ) // auto, maximum allowable number of CPUs will be used
	, _imageOrientation( imageOrientation )
{
	_renderArgs.renderWindow.x1 = _renderArgs.renderWindow.y1 = _renderArgs.renderWindow.x2 = _renderArgs.renderWindow.y2 = 0;
	_renderArgs.renderScale.x   = _renderArgs.renderScale.y = 0;
	_renderArgs.time            = -1;
	_renderArgs.fieldToRender   = OFX::eFieldNone;

	_clipDst = effect.fetchClip( kOfxImageEffectOutputClipName );
}

template <class View>
ImageGilProcessor<View>::~ImageGilProcessor()
{}

template <class View>
void ImageGilProcessor<View>::process( void )
{
	// is it OK ?
	if( _renderArgs.renderWindow.x2 - _renderArgs.renderWindow.x1 == 0 ||
	    _renderArgs.renderWindow.y2 - _renderArgs.renderWindow.y1 == 0 )
	{
		BOOST_THROW_EXCEPTION( exception::ImageFormat() << exception::user( "RenderWindow empty !" ) );
	}
	// call the pre MP pass
	preProcess();

	// call the base multi threading code, should put a pre & post thread calls in too
	multiThread( _nbThreads );

	// call the post MP pass
	postProcess();
}

}
}

#endif

