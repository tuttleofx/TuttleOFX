#ifndef _TUTTLE_HOST_CORE_EXCEPTION_HPP
#define _TUTTLE_HOST_CORE_EXCEPTION_HPP

#include <tuttle/host/ofx/ofxhUtilities.h>

#include <ofxCore.h>

#include <stdexcept>

namespace tuttle {
namespace host {
namespace core {

class Exception : public std::logic_error
{
protected:
	OfxStatus _status;

public:
	Exception( const std::string& msg = "" );
	Exception( const OfxStatus& status, const std::string& msg = "" );
	Exception( const Exception& orig );
	virtual ~Exception() throw( );
	const OfxStatus ofxStatus() const { return _status; }

private:
};

}
}
}

#endif
