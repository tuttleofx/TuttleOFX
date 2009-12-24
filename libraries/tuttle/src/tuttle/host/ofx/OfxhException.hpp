#ifndef _OFXH_EXCEPTION_HPP_
#define _OFXH_EXCEPTION_HPP_

namespace tuttle {
namespace host {
namespace ofx {

/// exception, representing an OfxStatus
/// @todo tuttle: do something, the minimum inherite std::exception...
class OfxhException
{
OfxStatus _stat;

public:
	/// ctor
	OfxhException( OfxStatus stat ) : _stat( stat )
	{}

	/// get the status
	OfxStatus getStatus()
	{
		return _stat;
	}

	// TUTTLE_TODO : mapStatusEnumToStr... in tuttle common
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

