#ifndef _TUTTLE_HOST_OFX_MESSAGE_HPP_
#define _TUTTLE_HOST_OFX_MESSAGE_HPP_

#include "OfxhCore.hpp"
#include <string>

namespace tuttle {
namespace host {
namespace ofx {

class OfxhIMessage
{
public:
	virtual ~OfxhIMessage() {}

	virtual OfxStatus vmessage( const char* type,
	                            const char* id,
	                            const char* format,
	                            va_list     args ) const = 0;
};


}
}
}

#endif

