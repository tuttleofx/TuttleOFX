#ifndef _OFXH_EXCEPTION_HPP_
#define _OFXH_EXCEPTION_HPP_

#include "OfxhCore.hpp"
#include "OfxhUtilities.hpp"

#include <boost/throw_exception.hpp>
#include <boost/exception/exception.hpp>

#include <stdexcept>

namespace tuttle {
namespace host {
namespace ofx {

/**
 * exception, representing an OfxStatus
 */
class OfxhException : public std::logic_error, virtual public boost::exception
{
	OfxStatus _stat;

public:
	explicit OfxhException( const std::string& what )
	: boost::exception()
	, std::logic_error( what )
	{
	}

	/// ctor
	explicit OfxhException( OfxStatus stat )
		: boost::exception()
		, std::logic_error( mapStatusToString( stat ) )
		, _stat( stat )
	{
	}

	/// ctor
	explicit OfxhException( OfxStatus stat, const std::string& what )
		: boost::exception()
		, std::logic_error( mapStatusToString( stat ) + " " + what )
		, _stat( stat )
	{
	}

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

//#if !defined(WINDOWS) || !defined(SWIG)
// #define OFX_EXCEPTION_SPEC throw(tuttle::host::ofx::OfxhException)
//#else
 #define OFX_EXCEPTION_SPEC
//#endif

}
}
}

#endif

