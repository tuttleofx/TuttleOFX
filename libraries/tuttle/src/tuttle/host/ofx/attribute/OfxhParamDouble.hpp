#ifndef _TUTTLE_HOST_OFX_PARAM_PARAMDOUBLE_HPP_
#define _TUTTLE_HOST_OFX_PARAM_PARAMDOUBLE_HPP_

#include "OfxhParam.hpp"
#include "OfxhParamDescriptor.hpp"
#include "OfxhParamSet.hpp"
#include "OfxhKeyframeParam.hpp"


namespace tuttle {
namespace host {
namespace ofx {
namespace attribute {


class OfxhParamDouble :
    public OfxhParam,
	public OfxhKeyframeParam
{
public:
	typedef double BaseType;
	OfxhParamDouble( const OfxhParamDescriptor& descriptor, const std::string& name, OfxhParamSet& setInstance )
		: OfxhParam( descriptor, name, setInstance )
	{
		getEditableProperties().addNotifyHook( kOfxParamPropDisplayMin, this );
		getEditableProperties().addNotifyHook( kOfxParamPropDisplayMax, this );
	}

	// Deriving implementatation needs to overide these
	virtual void get( double& ) const OFX_EXCEPTION_SPEC = 0;
	virtual void get( const OfxTime time, double& ) const OFX_EXCEPTION_SPEC = 0;
	virtual void set( const double& )                               OFX_EXCEPTION_SPEC = 0;
	virtual void set( const OfxTime time, const double& )                 OFX_EXCEPTION_SPEC = 0;
	virtual void derive( const OfxTime time, double& ) const OFX_EXCEPTION_SPEC = 0;
	virtual void integrate( const OfxTime time1, const OfxTime time2, double& ) const OFX_EXCEPTION_SPEC = 0;

	/// implementation of var args function
	virtual void getV( va_list arg ) const OFX_EXCEPTION_SPEC;

	/// implementation of var args function
	virtual void getV( const OfxTime time, va_list arg ) const OFX_EXCEPTION_SPEC;

	/// implementation of var args function
	virtual void setV( va_list arg ) OFX_EXCEPTION_SPEC;

	/// implementation of var args function
	virtual void setV( const OfxTime time, va_list arg ) OFX_EXCEPTION_SPEC;

	/// implementation of var args function
	virtual void deriveV( const OfxTime time, va_list arg ) const OFX_EXCEPTION_SPEC;

	/// implementation of var args function
	virtual void integrateV( const OfxTime time1, const OfxTime time2, va_list arg ) const OFX_EXCEPTION_SPEC;
};


}
}
}
}


#endif

