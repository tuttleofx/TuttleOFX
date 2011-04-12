#ifndef _TUTTLE_PLUGIN_IMAGEGILFILTERPROCESSOR_HPP_
#define _TUTTLE_PLUGIN_IMAGEGILFILTERPROCESSOR_HPP_

#include "ImageGilProcessor.hpp"

#include <boost/scoped_ptr.hpp>

namespace tuttle {
namespace plugin {

/**
 * @brief Base class that can be used to process images of any type using boost::gil library view to access images.
 */
template <class SView, class DView = SView>
class ImageGilFilterProcessor : public ImageGilProcessor<DView>
{
protected:
	OFX::Clip* _clipSrc;       ///< Source image clip
	boost::scoped_ptr<OFX::Image> _src;
	OfxRectI _srcPixelRod;
	SView _srcView; ///< @brief source clip (filters have only one input)

public:
	ImageGilFilterProcessor( OFX::ImageEffect& effect );
	virtual ~ImageGilFilterProcessor();

	virtual void setup( const OFX::RenderArguments& args );
};

template<class SView, class DView>
ImageGilFilterProcessor<SView, DView>::ImageGilFilterProcessor( OFX::ImageEffect& effect )
	: ImageGilProcessor<DView>( effect )
{
	_clipSrc = effect.fetchClip( kOfxImageEffectSimpleSourceClipName );
}

template<class SView, class DView>
ImageGilFilterProcessor<SView, DView>::~ImageGilFilterProcessor()
{}

template<class SView, class DView>
void ImageGilFilterProcessor<SView, DView>::setup( const OFX::RenderArguments& args )
{
	ImageGilProcessor<DView>::setup( args );

	// source view
	this->_src.reset( _clipSrc->fetchImage( args.time ) );
	if( !this->_src.get() )
		BOOST_THROW_EXCEPTION( exception::ImageNotReady() );
	if( this->_src->getRowBytes() == 0 )
		BOOST_THROW_EXCEPTION( exception::WrongRowBytes() );
	//	_srcPixelRod = _src->getRegionOfDefinition(); // bug in nuke, returns bounds
	_srcPixelRod   = _clipSrc->getPixelRod( args.time, args.renderScale );
	this->_srcView = tuttle::plugin::getView<SView>( this->_src.get(), _srcPixelRod );

//	// Make sure bit depths are same
//	if( this->_src->getPixelDepth() != this->_dst->getPixelDepth() ||
//	    this->_src->getPixelComponents() != this->_dst->getPixelComponents() )
//		BOOST_THROW_EXCEPTION( exception::BitDepthMismatch() );
}

}
}

#endif

