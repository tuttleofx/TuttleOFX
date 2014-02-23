%include <tuttle/host/global.i>
%include <tuttle/host/memory/IMemoryPool.i>

%{
#include <tuttle/host/memory/MemoryPool.hpp>
%}

%include <tuttle/host/memory/MemoryPool.hpp>

%extend tuttle::host::memory::MemoryPool
{
	std::string __str__() const
	{
		std::stringstream s;
		s << *self;
		return s.str();
	}
}

