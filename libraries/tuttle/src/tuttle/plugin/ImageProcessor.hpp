#ifndef _TUTTLE_PLUGIN_IMAGEPROCESSOR_HPP_
#define _TUTTLE_PLUGIN_IMAGEPROCESSOR_HPP_

#include "exceptions.hpp"
#include "OfxProgress.hpp"

#include <tuttle/plugin/image.hpp>
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
 * @brief Base class that can be used to process images of any type.
 */
class ImageProcessor : public OFX::MultiThread::Processor
	, public tuttle::plugin::OfxProgress
{
protected:
	OFX::ImageEffect& _effect; ///< effect to render with
	OFX::RenderArguments _renderArgs; ///< render arguments
	OFX::Clip* _clipDst;       ///< Destination image clip
	boost::scoped_ptr<OFX::Image> _dst;
	OfxRectI _dstPixelRod;
	OfxPointI _dstPixelRodSize;
	OfxPointI _renderWindowSize;
	EImageOrientation _imageOrientation;

private:
	unsigned int _nbThreads;

public:
	/** @brief ctor */
	ImageProcessor( OFX::ImageEffect& effect, const EImageOrientation imageOrientation )
		: OfxProgress( effect )
		, _effect( effect )
		, _imageOrientation( imageOrientation )
		, _nbThreads( 0 ) // auto, maximum allowable number of CPUs will be used
	{
		_dstPixelRod.x1 = _dstPixelRod.y1 = _dstPixelRod.x2 = _dstPixelRod.y2 = 0;
		_dstPixelRodSize.x = _dstPixelRodSize.y = 0;
		_renderWindowSize.x = _renderWindowSize.y = 0;
		_renderArgs.renderWindow.x1 = _renderArgs.renderWindow.y1 = _renderArgs.renderWindow.x2 = _renderArgs.renderWindow.y2 = 0;
		_renderArgs.renderScale.x   = _renderArgs.renderScale.y = 0;
		_renderArgs.time            = -1;
		_renderArgs.fieldToRender   = OFX::eFieldNone;

		_clipDst = effect.fetchClip( kOfxImageEffectOutputClipName );
	}

	virtual ~ImageProcessor()
	{}

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
//		TUTTLE_LOG_INFOS;
//		TUTTLE_LOG_VAR( TUTTLE_INFO,  "dst - fetchImage " << time );
		_dst.reset( _clipDst->fetchImage( args.time ) );
		if( !_dst.get() )
			BOOST_THROW_EXCEPTION( exception::ImageNotReady()
				<< exception::dev() + "Error on clip " + quotes(_clipDst->name())
				<< exception::time( args.time ) );
		if( _dst->getRowDistanceBytes() == 0 )
			BOOST_THROW_EXCEPTION( exception::WrongRowBytes()
				<< exception::dev() + "Error on clip " + quotes(_clipDst->name())
				<< exception::time( args.time ) );
		
		if( OFX::getImageEffectHostDescription()->hostName == "uk.co.thefoundry.nuke" )
		{
			// bug in nuke, getRegionOfDefinition() on OFX::Image returns bounds
			_dstPixelRod   = _clipDst->getPixelRod( args.time, args.renderScale );
		}
		else
		{
			_dstPixelRod = _dst->getRegionOfDefinition();
		}
		_dstPixelRodSize.x = ( this->_dstPixelRod.x2 - this->_dstPixelRod.x1 );
		_dstPixelRodSize.y = ( this->_dstPixelRod.y2 - this->_dstPixelRod.y1 );
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
	virtual void process()
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
};


}
}

#endif

