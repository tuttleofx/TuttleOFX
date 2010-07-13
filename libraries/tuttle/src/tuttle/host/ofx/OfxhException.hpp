#ifndef _OFXH_EXCEPTION_HPP_
#define _OFXH_EXCEPTION_HPP_

#include "OfxhCore.hpp"
#include "OfxhUtilities.hpp"

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
	OfxStatus getStatus() const
	{
		return _stat;
	}

	///@todo tuttle : mapStatusEnumToStr... in tuttle common
	std::string getStatusStr() const
	{
		return mapStatusToString( _stat );
	}

};

#if !defined(WINDOWS) || !defined(SWIG)
 #define OFX_EXCEPTION_SPEC throw(tuttle::host::ofx::OfxhException)
#else
 #define OFX_EXCEPTION_SPEC
#endif

}
}
}

#endif

