%include <tuttle/host/global.i>

%include <tuttle/host/ofx/attribute/OfxhClip.hpp>
%include <tuttle/host/ofx/property/OfxhSet.i>

%include <ofxCore.i>

%include <std_string.i>

%include <boost_shared_ptr.i>
%include <typemaps.i>

%shared_ptr(tuttle::host::ofx::imageEffect::OfxhImage)

%{
#include <tuttle/host/ofx/OfxhImage.hpp>
%}

%include <tuttle/host/ofx/OfxhImage.hpp>




