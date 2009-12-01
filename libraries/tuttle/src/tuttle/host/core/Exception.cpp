#include "Exception.hpp"

namespace tuttle {
namespace host {
namespace core {

Exception::Exception( const std::string& msg )
: std::logic_error( std::string("OfxStatErrUnknown") + std::string(" - ") +msg )
, _status(kOfxStatErrUnknown)
{
}

Exception::Exception( const OfxStatus& status, const std::string& msg )
: std::logic_error( std::string(mapStatusToString(status)) + std::string(" - ") +msg )
, _status(status)
{
}

Exception::Exception( const Exception& orig )
: std::logic_error( orig )
{
}

Exception::~Exception( ) throw() { }

}
}
}
