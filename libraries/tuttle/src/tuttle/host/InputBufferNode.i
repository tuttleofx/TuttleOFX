%include <tuttle/host/global.i>
%include <tuttle/host/memory/MemoryCache.i>
%include <std_vector.i>
%include <std_string.i>

%include <tuttle/host/global.i>
%include <tuttle/host/HostDescriptor.i>
%include <tuttle/host/Node.i>
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

%{
#include <tuttle/host/InputBufferNode.hpp>
%}

namespace std {
%template(UByteVector) vector<unsigned char>;
%template(UShortVector) vector<unsigned short>;
%template(FloatVector) vector<float>;
}

%include <tuttle/host/InputBufferNode.hpp>
