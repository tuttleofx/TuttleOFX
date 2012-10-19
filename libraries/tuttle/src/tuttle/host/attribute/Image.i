%include <tuttle/host/global.i>
%include <boost_shared_ptr.i>

%shared_ptr(tuttle::host::attribute::Image)

%{
#include <tuttle/host/attribute/Image.hpp>
%}

%include <tuttle/host/attribute/Image.hpp>

