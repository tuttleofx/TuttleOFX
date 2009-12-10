#ifndef _TUTTLE_HOST_CORE_IMEMORYCACHE_HPP_
#define _TUTTLE_HOST_CORE_IMEMORYCACHE_HPP_

namespace tuttle {
namespace host {
namespace core {

typedef unsigned long long SizeInteger;

class IMemoryCache
{
public:
	virtual SizeInteger getMemorySizeUsed() const       = 0;
	virtual SizeInteger getMemorySizeAllocated() const  = 0;
	virtual SizeInteger getMemorySizeAuthorized() const = 0;
	virtual SizeInteger clear( SizeInteger size )       = 0;
	virtual SizeInteger clearOne()                      = 0;
	virtual SizeInteger clearAll()                      = 0;
};

}
}
}

#endif
