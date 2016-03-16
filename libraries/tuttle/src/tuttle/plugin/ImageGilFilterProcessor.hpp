#ifndef _TUTTLE_PLUGIN_IMAGEGILFILTERPROCESSOR_HPP_
#define _TUTTLE_PLUGIN_IMAGEGILFILTERPROCESSOR_HPP_

#include "ImageGilProcessor.hpp"

#include <boost/scoped_ptr.hpp>

namespace tuttle
{
namespace plugin
{

/**
 * @brief Base class that can be used to process images of any type using boost::gil library view to access images.
 */
template <class SView, class DView = SView>
class ImageGilFilterProcessor : public ImageGilProcessor<DView>
{
protected:
    OFX::Clip* _clipSrc; ///< Source image clip
    boost::scoped_ptr<OFX::Image> _src;
    OfxRectI _srcPixelRod;
    SView _srcView; ///< @brief source clip (filters have only one input)

public:
    ImageGilFilterProcessor(OFX::ImageEffect& effect, const EImageOrientation imageOrientation);
    virtual ~ImageGilFilterProcessor();

    virtual void setup(const OFX::RenderArguments& args);
};

template <class SView, class DView>
ImageGilFilterProcessor<SView, DView>::ImageGilFilterProcessor(OFX::ImageEffect& effect,
                                                               const EImageOrientation imageOrientation)
    : ImageGilProcessor<DView>(effect, imageOrientation)
{
    _srcPixelRod.x1 = _srcPixelRod.y1 = _srcPixelRod.x2 = _srcPixelRod.y2 = 0;

    _clipSrc = effect.fetchClip(kOfxImageEffectSimpleSourceClipName);

    if(!_clipSrc->isConnected())
        BOOST_THROW_EXCEPTION(exception::ImageNotConnected());
}

template <class SView, class DView>
ImageGilFilterProcessor<SView, DView>::~ImageGilFilterProcessor()
{
}

template <class SView, class DView>
void ImageGilFilterProcessor<SView, DView>::setup(const OFX::RenderArguments& args)
{
    ImageGilProcessor<DView>::setup(args);

    // source view
    //	TUTTLE_LOG_INFOS;
    //	TUTTLE_LOG_VAR( TUTTLE_INFO, "src - fetchImage " << time );
    _src.reset(_clipSrc->fetchImage(args.time));
    if(!_src.get())
        BOOST_THROW_EXCEPTION(exception::ImageNotReady() << exception::dev() + "Error on clip " + quotes(_clipSrc->name())
                                                         << exception::time(args.time));
    if(_src->getRowDistanceBytes() == 0)
        BOOST_THROW_EXCEPTION(exception::WrongRowBytes() << exception::dev() + "Error on clip " + quotes(_clipSrc->name())
                                                         << exception::time(args.time));

    if(OFX::getImageEffectHostDescription()->hostName == "uk.co.thefoundry.nuke")
    {
        // bug in nuke, getRegionOfDefinition() on OFX::Image returns bounds
        _srcPixelRod = _clipSrc->getPixelRod(args.time, args.renderScale);
    }
    else
    {
        _srcPixelRod = _src->getRegionOfDefinition();
    }
    _srcView = ImageGilProcessor<DView>::template getCustomView<SView>(_src.get(), _srcPixelRod);

    //	// Make sure bit depths are same
    //	if( this->_src->getPixelDepth() != this->_dst->getPixelDepth() ||
    //	    this->_src->getPixelComponents() != this->_dst->getPixelComponents() )
    //		BOOST_THROW_EXCEPTION( exception::BitDepthMismatch() );
}
}
}

#endif
