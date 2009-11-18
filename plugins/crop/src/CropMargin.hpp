#ifndef VOLET_MARGIN_HPP
#define VOLET_MARGIN_HPP


#include <tuttle/common/utils/global.hpp>

#include <ofxsImageEffect.h>
#include <ofxsInteract.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {

class CropMargin : public OFX::OverlayInteract
{
public:

    CropMargin( OfxInteractHandle handle, OFX::ImageEffect *effect ) : OFX::OverlayInteract( handle )
    {
        _effect = effect;
    }
    virtual bool draw( const OFX::DrawArgs &args );
};

class CropMarginOverlay : public OFX::EffectOverlayDescriptor
{
public:

    OFX::Interact* createInstance( OfxInteractHandle handle, OFX::ImageEffect *effect )
    {
        return new CropMargin( handle, effect );
    }

    OfxPluginEntryPoint* getMainEntry( )
    {
        return NULL;
    }
};

}

#endif
