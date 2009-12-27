#ifndef _OFXH_EXCEPTION_HPP_
#define _OFXH_EXCEPTION_HPP_

#include <ofxCore.h>

#include <stdexcept>

namespace tuttle {
namespace host {
namespace ofx {

/**
 * exception, representing an OfxStatus
 */
class OfxhException : public std::logic_error
{
OfxStatus _stat;

public:
	explicit OfxhException( const std::string& what ) : std::logic_error( what )
	{}

	/// ctor
	explicit OfxhException( OfxStatus stat )
		: std::logic_error( mapStatusToString( stat ) )
		, _stat( stat )
	{}

	/// ctor
	explicit OfxhException( OfxStatus stat, const std::string& what )
		: std::logic_error( mapStatusToString( stat ) + " " + what )
		, _stat( stat )
	{}

	/// get the status
	OfxStatus getStatus()
	{
		return _stat;
	}

	///@todo tuttle : mapStatusEnumToStr... in tuttle common
	const std::string getStatusStr()
	{
		return mapStatusToString( _stat );
	}

};

#ifndef WINDOWS
 #define OFX_EXCEPTION_SPEC throw (tuttle::host::ofx::OfxhException)
#else
 #define OFX_EXCEPTION_SPEC
#endif

}
}
}

#endif

