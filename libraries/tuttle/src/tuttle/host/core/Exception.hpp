#ifndef _TUTTLE_HOST_CORE_EXCEPTION_HPP
#define _TUTTLE_HOST_CORE_EXCEPTION_HPP

#include <tuttle/host/ofx/ofxhUtilities.h>

#include <ofxCore.h>

#include <sstream>
#include <stdexcept>

namespace tuttle {
namespace host {
namespace core {
namespace exception {

class LogicError : public std::logic_error
{
protected:
	OfxStatus _status;
	std::stringstream _ss;

public:
	LogicError( const std::string& msg = "" );
	LogicError( const OfxStatus& status, const std::string& msg = "" );
	LogicError( const LogicError& other );
	virtual ~LogicError() throw( );
	const OfxStatus ofxStatus() const { return _status; }
	const std::string ofxStatusStr() const { return ofx::mapStatusToString( _status ); }
	template<typename T>
	LogicError& operator<<( const T& v ) { _ss << v; return *this; }

	virtual const std::string message() const throw()
	{
		std::stringstream ss;
		ss << ofxStatusStr() << " : " << std::logic_error::what() << _ss;
		return ss.str();
	}
	virtual const char* what() const throw()
	{
		return message().c_str();
	}

private:
};

}
}
}
}

#endif
