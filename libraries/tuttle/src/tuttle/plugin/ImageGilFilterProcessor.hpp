#ifndef _TUTTLE_PLUGIN_IMAGEGILFILTERPROCESSOR_HPP_
#define _TUTTLE_PLUGIN_IMAGEGILFILTERPROCESSOR_HPP_

#include "ImageGilProcessor.hpp"

#include <boost/scoped_ptr.hpp>

namespace tuttle {
namespace plugin {

/**
 * @brief Base class that can be used to process images of any type using boost::gil library view to access images.
 */
template <class View>
class ImageGilFilterProcessor : public ImageGilProcessor<View>
{
protected:
    OFX::Clip* _clipSrc;       ///< Source image clip
	boost::scoped_ptr<OFX::Image> _src;
	OfxRectI _srcPixelRod;
	View _srcView; ///< @brief source clip (filters have only one input)

public:
	ImageGilFilterProcessor( OFX::ImageEffect& effect );
	virtual ~ImageGilFilterProcessor();

	virtual void setup( const OFX::RenderArguments& args );
};

template <class View>
ImageGilFilterProcessor<View>::ImageGilFilterProcessor( OFX::ImageEffect& effect )
	: ImageGilProcessor<View>( effect )
{
    _clipSrc = effect.fetchClip( kOfxImageEffectSimpleSourceClipName );
}

template <class View>
ImageGilFilterProcessor<View>::~ImageGilFilterProcessor()
{
}

template <class View>
void ImageGilFilterProcessor<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilProcessor<View>::setup( args );

	// source view
	this->_src.reset( _clipSrc->fetchImage( args.time ) );
	if( !this->_src.get( ) )
		throw( ImageNotReadyException( ) );
	if( this->_src->getRowBytes( ) <= 0 )
		throw( WrongRowBytesException( ) );
//	_srcPixelRod = _src->getRegionOfDefinition(); // bug in nuke, returns bounds
	_srcPixelRod = _clipSrc->getPixelRod(args.time); // bug in nuke, if previous node is "Dot"...
	this->_srcView = this->getView( this->_src.get(), _srcPixelRod );

	// Make sure bit depths are same
	if( this->_src->getPixelDepth() != this->_dst->getPixelDepth() ||
	    this->_src->getPixelComponents() != this->_dst->getPixelComponents() )
	    throw( BitDepthMismatchException( ) );
}


}
}

#endif

