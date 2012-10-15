%include <tuttle/host/global.i>
%include <tuttle/host/attribute/Image.i>

%include <boost_shared_ptr.i>

%shared_ptr(tuttle::host::attribute::Image)

%{
#include <tuttle/host/memory/IMemoryCache.hpp>
%}

%include <tuttle/host/memory/IMemoryCache.hpp>

