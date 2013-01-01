%include <tuttle/host/global.i>
%include <tuttle/host/HostDescriptor.i>
%include <tuttle/host/INode.i>
%include <tuttle/host/ofx/OfxhException.i>
%include <tuttle/host/ofx/property/OfxhPropertyTemplate.i>
%include <tuttle/host/ofx/property/OfxhSet.i>
%include <tuttle/host/ofx/property/OfxhNotifyHook.i>
%include <tuttle/host/ofx/property/OfxhGetHook.i>
%include <tuttle/host/ofx/attribute/OfxhParamSet.i>
//%include <tuttle/host/ofx/attribute/OfxhClipImageSet.i>
%include <tuttle/host/ofx/OfxhImageEffectPlugin.i>
//%include <tuttle/host/ofx/OfxhImageEffectPluginCache.i>
%include <tuttle/host/ofx/OfxhImageEffectNodeBase.i>
%include <tuttle/host/ofx/OfxhOverlayInteract.i>

%{
#include <tuttle/host/ImageEffectNode.hpp>
%}

%include <tuttle/host/ImageEffectNode.hpp>
