%include <tuttle/host/core/Exception.i>
%include <tuttle/host/core/HostDescriptor.i>
%include <tuttle/host/ofx/OfxhException.i>
%include <tuttle/host/ofx/property/OfxhPropertyTemplate.i>
%include <tuttle/host/ofx/property/OfxhSet.i>
%include <tuttle/host/ofx/property/OfxhNotifyHook.i>
%include <tuttle/host/ofx/property/OfxhGetHook.i>
//%include <tuttle/host/ofx/attribute/OfxhParamSet.i>
//%include <tuttle/host/ofx/attribute/OfxhClipImageSet.i>
%include <tuttle/host/ofx/OfxhImageEffectPlugin.i>
//%include <tuttle/host/ofx/OfxhImageEffectPluginCache.i>
%include <tuttle/host/ofx/OfxhImageEffectNodeBase.i>

%{
#include <tuttle/host/core/ImageEffectNode.hpp>
%}

%include <tuttle/host/core/ImageEffectNode.hpp>
