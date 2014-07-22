%include <tuttle/host/global.i>
%include <tuttle/host/memory/IMemoryCache.i>

%{
#include <tuttle/host/memory/MemoryCache.hpp>
%}

%include <tuttle/host/memory/MemoryCache.hpp>


%extend tuttle::host::memory::MemoryCache
{
	std::string __str__() const
	{
		std::stringstream s;
		s << *self;
		return s.str();
	}
}
