#ifndef _TUTTLE_HOST_OFX_PARAM_PARAMSTRING_HPP_
#define _TUTTLE_HOST_OFX_PARAM_PARAMSTRING_HPP_

#include "OfxhParam.hpp"
#include "OfxhParamDescriptor.hpp"
#include "OfxhParamSet.hpp"
#include "OfxhKeyframeParam.hpp"

namespace tuttle {
namespace host {
namespace ofx {
namespace attribute {

class OfxhParamString
	: public OfxhParam
	, public OfxhKeyframeParam
{
mutable std::string _returnValue;     ///< location to hold temporary return value. Should delegate this to implementation!!!

public:
	typedef std::string BaseType;
	OfxhParamString( const OfxhParamDescriptor& descriptor, const std::string& name, OfxhParamSet& setInstance ) : OfxhParam( descriptor, name, setInstance ) {}

	virtual void get( std::string& ) const OFX_EXCEPTION_SPEC                                                  = 0;
	virtual void getAtTime( const OfxTime time, std::string& ) const OFX_EXCEPTION_SPEC                        = 0;
	virtual void set( const std::string&, const EChange change )                OFX_EXCEPTION_SPEC             = 0;
	virtual void setAtTime( const OfxTime time, const std::string&, const EChange change )  OFX_EXCEPTION_SPEC = 0;

	/// implementation of var args function
	virtual void getV( va_list arg ) const OFX_EXCEPTION_SPEC;

	/// implementation of var args function
	virtual void getV( const OfxTime time, va_list arg ) const OFX_EXCEPTION_SPEC;

	/// implementation of var args function
	virtual void setV( va_list arg, const EChange change ) OFX_EXCEPTION_SPEC;

	/// implementation of var args function
	virtual void setV( const OfxTime time, va_list arg, const EChange change ) OFX_EXCEPTION_SPEC;

	std::ostream& displayValues( std::ostream& os ) const
	{
		os << getString();
		return os;
	}
};

}
}
}
}

#endif

