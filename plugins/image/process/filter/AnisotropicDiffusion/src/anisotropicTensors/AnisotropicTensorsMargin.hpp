#ifndef TENSORS_MARGIN_HPP
#define TENSORS_MARGIN_HPP

#include <ofxsImageEffect.h>
#include <ofxsInteract.h>

namespace tuttle
{
namespace plugin
{
namespace anisotropicFilter
{
namespace tensors
{

class TensorsMargin : public OFX::OverlayInteract
{
public:
    TensorsMargin(OfxInteractHandle handle, OFX::ImageEffect* effect)
        : OFX::OverlayInteract(handle)
    {
        _effect = effect;
    }
    virtual bool draw(const OFX::DrawArgs& args);
};

class TensorsMarginOverlay : public OFX::EffectOverlayDescriptor
{
public:
    OFX::Interact* createInstance(OfxInteractHandle handle, OFX::ImageEffect* effect)
    {
        return new TensorsMargin(handle, effect);
    }

    OfxPluginEntryPoint* getMainEntry() { return NULL; }
};
}
}
}
}

#endif
