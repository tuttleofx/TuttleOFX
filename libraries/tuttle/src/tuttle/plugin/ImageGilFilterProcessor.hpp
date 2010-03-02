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
    OFX::Clip           *_srcClip;       ///< Source image clip
    OFX::Clip           *_dstClip;       ///< Destination image clip
	boost::scoped_ptr<OFX::Image> _src;
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
    _srcClip = effect.fetchClip( kOfxImageEffectSimpleSourceClipName );
    _dstClip = effect.fetchClip( kOfxImageEffectOutputClipName );
}

template <class View>
ImageGilFilterProcessor<View>::~ImageGilFilterProcessor()
{
}

template <class View>
void ImageGilFilterProcessor<View>::setup( const OFX::RenderArguments& args )
{
	using namespace boost::gil;

	// destination view
	this->_dst.reset( _dstClip->fetchImage( args.time ) );
	if( !this->_dst.get( ) )
	    throw( ImageNotReadyException( ) );
	if( this->_dst->getRowBytes( ) <= 0 )
		throw( WrongRowBytesException( ) );
	this->_dstView = this->getView( this->_dst.get(), _dstClip->getPixelRod(args.time) );

	// source view
	this->_src.reset( _srcClip->fetchImage( args.time ) );
	if( !this->_src.get( ) )
		throw( ImageNotReadyException( ) );
	if( this->_src->getRowBytes( ) <= 0 )
		throw( WrongRowBytesException( ) );
	this->_srcView = this->getView( this->_src.get(), _srcClip->getPixelRod(args.time) );

	// Make sure bit depths are same
	if( this->_src->getPixelDepth() != this->_dst->getPixelDepth() ||
	    this->_src->getPixelComponents() != this->_dst->getPixelComponents() )
	    throw( BitDepthMismatchException( ) );
}


}
}

#endif

