#include "OfxhMessageSuite.hpp"
#include "OfxhMessage.hpp"
#include "OfxhCore.hpp"

#include <cstdarg>

namespace tuttle {
namespace host {
namespace ofx {

static OfxStatus message( void* handle, const char* type, const char* id, const char* format, ... )
{
	OfxhIMessage* effectInstance = reinterpret_cast<OfxhIMessage*>( handle );

	if( effectInstance )
	{
		va_list args;
		va_start( args, format );
		effectInstance->vmessage( type, id, format, args );
		va_end( args );
	}
	else
	{
		va_list args;
		va_start( args, format );
		vprintf( format, args );
		va_end( args );
	}
	return kOfxStatOK;
}

/// message suite for an image effect plugin
static struct OfxMessageSuiteV1 gMessageSuite =
{
	message
};

void* getMessageSuite( const int version )
{
	if( version == 1 )
        return static_cast<void*>(&gMessageSuite);
    return NULL;
}

}
}
}

