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
#define SWIG_FILE_WITH_INIT
#include <tuttle/host/InputBufferNode.hpp>
%}

%include "wrappers/numpy.i"

%init %{
import_array();
%}

%apply (float* INPLACE_ARRAY2, int DIM1, int DIM2) {(float* rawBuffer, int width, int height)};
%apply (unsigned short* INPLACE_ARRAY2, int DIM1, int DIM2) {(unsigned short* rawBuffer, int width, int height)};
%apply (unsigned char* INPLACE_ARRAY2, int DIM1, int DIM2) {(unsigned char* rawBuffer, int width, int height)};

namespace std {
%template(UByteVector) vector<unsigned char>;
%template(UShortVector) vector<unsigned short>;
%template(FloatVector) vector<float>;
}

%include <tuttle/host/InputBufferNode.hpp>


%clear (float* rawBuffer, int width, int height);
%clear (unsigned short* rawBuffer, int width, int height);
%clear (unsigned char* rawBuffer, int width, int height);


