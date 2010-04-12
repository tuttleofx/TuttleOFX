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

class OfxhParamString :
	public OfxhParam,
	public OfxhKeyframeParam
{
	mutable std::string _returnValue; ///< location to hold temporary return value. Should delegate this to implementation!!!

public:
	typedef std::string BaseType;
	OfxhParamString( const OfxhParamDescriptor& descriptor, const std::string& name, OfxhParamSet& setInstance ) : OfxhParam( descriptor, name, setInstance ) {}

	virtual void get( std::string& ) const OFX_EXCEPTION_SPEC = 0;
	virtual void get( const OfxTime time, std::string& ) const OFX_EXCEPTION_SPEC = 0;
	virtual void set( const char* )                OFX_EXCEPTION_SPEC = 0;
	virtual void set( const OfxTime time, const char* )  OFX_EXCEPTION_SPEC = 0;
	virtual void set( const std::string& )                OFX_EXCEPTION_SPEC = 0;
	virtual void set( const OfxTime time, const std::string& )  OFX_EXCEPTION_SPEC = 0;

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

