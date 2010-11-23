#ifndef _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMDOUBLE_HPP_
#define _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMDOUBLE_HPP_

#include "Param.hpp"

#include <tuttle/host/ofx/attribute/OfxhParamDouble.hpp>

namespace tuttle {
namespace host {
namespace attribute {

class ParamDouble : public Param
	, public ofx::attribute::OfxhParamDouble
{
protected:
	double _value;

public:
	ParamDouble( INode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor, const std::size_t index = 0 );
	ParamDouble* clone() const { return new ParamDouble( *this ); }

	double getDefault() const;

	void get( double& ) const OFX_EXCEPTION_SPEC;
	void getAtTime( const OfxTime time, double& ) const OFX_EXCEPTION_SPEC;
	void set( const double&, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC;
	void setAtTime( const OfxTime time, const double&, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC;

	void derive( const OfxTime time, double& ) const OFX_EXCEPTION_SPEC;
	void integrate( const OfxTime time1, const OfxTime time2, double& ) const OFX_EXCEPTION_SPEC;

	void copy( const ParamDouble& p ) OFX_EXCEPTION_SPEC;
	void copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC;
};

}
}
}

#endif
