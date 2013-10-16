#ifndef _TUTTLE_PLUGIN_IMAGEFILTERPROCESSOR_HPP_
#define _TUTTLE_PLUGIN_IMAGEFILTERPROCESSOR_HPP_

#include "ImageProcessor.hpp"

namespace tuttle {
namespace plugin {

/**
 * @brief Base class that can be used to process images of any type using boost::gil library view to access images.
 */
class ImageFilterProcessor : public ImageProcessor
{
protected:
	OFX::Clip* _clipSrc;       ///< Source image clip
	boost::scoped_ptr<OFX::Image> _src;
	OfxRectI _srcPixelRod;

public:
	ImageFilterProcessor( OFX::ImageEffect& effect, const EImageOrientation imageOrientation )
		: ImageProcessor( effect, imageOrientation )
	{
		_clipSrc = effect.fetchClip( kOfxImageEffectSimpleSourceClipName );

		if( ! _clipSrc->isConnected() )
			BOOST_THROW_EXCEPTION( exception::ImageNotConnected() );
	}
	
	virtual ~ImageFilterProcessor()
	{}

	virtual void setup( const OFX::RenderArguments& args )
	{
		ImageProcessor::setup( args );

		// source view
		_src.reset( _clipSrc->fetchImage( args.time ) );
		if( ! _src.get() )
			BOOST_THROW_EXCEPTION( exception::ImageNotReady()
					<< exception::dev() + "Error on clip " + quotes(_clipSrc->name()) );
		if( _src->getRowDistanceBytes() == 0 )
			BOOST_THROW_EXCEPTION( exception::WrongRowBytes()
					<< exception::dev() + "Error on clip " + quotes(_clipSrc->name()) );

		if( OFX::getImageEffectHostDescription()->hostName == "uk.co.thefoundry.nuke" )
		{
			// bug in nuke, getRegionOfDefinition() on OFX::Image returns bounds
			_srcPixelRod   = _clipSrc->getPixelRod( args.time, args.renderScale );
		}
		else
		{
			_srcPixelRod = _src->getRegionOfDefinition();
		}
	}
};


}
}

#endif

