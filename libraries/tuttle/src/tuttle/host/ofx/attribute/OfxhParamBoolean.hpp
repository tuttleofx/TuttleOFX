#ifndef _TUTTLE_HOST_OFX_PARAM_PARAMBOOLEAN_HPP_
#define _TUTTLE_HOST_OFX_PARAM_PARAMBOOLEAN_HPP_

#include "OfxhParam.hpp"
#include "OfxhParamDescriptor.hpp"
#include "OfxhParamSet.hpp"
#include "OfxhKeyframeParam.hpp"


namespace tuttle {
namespace host {
namespace ofx {
namespace attribute {


class OfxhParamBoolean :
    public OfxhParam,
	public OfxhKeyframeParam
{
public:
	typedef bool BaseType;
	OfxhParamBoolean( const OfxhParamDescriptor& descriptor, const std::string& name, OfxhParamSet& setInstance ) : OfxhParam( descriptor, name, setInstance ) {}

	// Deriving implementatation needs to overide these
	virtual void get( bool& ) const OFX_EXCEPTION_SPEC = 0;
	virtual void get( const OfxTime time, bool& ) const OFX_EXCEPTION_SPEC = 0;
	virtual void set( const bool& )                OFX_EXCEPTION_SPEC = 0;
	virtual void set( const OfxTime time, const bool& )  OFX_EXCEPTION_SPEC = 0;

	/// implementation of var args function
	virtual void getV( va_list arg ) const OFX_EXCEPTION_SPEC;

	/// implementation of var args function
	virtual void getV( const OfxTime time, va_list arg ) const OFX_EXCEPTION_SPEC;

	/// implementation of var args function
	virtual void setV( va_list arg ) OFX_EXCEPTION_SPEC;

	/// implementation of var args function
	virtual void setV( const OfxTime time, va_list arg ) OFX_EXCEPTION_SPEC;
};


}
}
}
}


#endif

