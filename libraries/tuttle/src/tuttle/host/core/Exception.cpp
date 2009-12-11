#include "Exception.hpp"

namespace tuttle {
namespace host {
namespace core {
namespace exception {

LogicError::LogicError( const std::string& msg )
	: std::logic_error( msg ),
	_status( kOfxStatErrUnknown )
{}

LogicError::LogicError( const OfxStatus& status, const std::string& msg )
	: std::logic_error( msg ),
	_status( status )
{}

LogicError::LogicError( const LogicError& other )
	: std::logic_error( other ),
	_status( other._status )
{
	_ss << other._ss; // no copy ctor for stringstream
}

LogicError::~LogicError() throw( ) {}

}
}
}
}
