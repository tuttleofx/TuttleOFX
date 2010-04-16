#ifndef _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMDOUBLE_HPP_
#define _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMDOUBLE_HPP_

#include <tuttle/host/ofx/attribute/OfxhParamDouble.hpp>
#include <tuttle/host/core/ImageEffectNode.hpp>

namespace tuttle {
namespace host {
namespace core {

class ParamDouble : public ofx::attribute::OfxhParamDouble
{
protected:
	ImageEffectNode& _effect;
	double _value;

public:
	ParamDouble( ImageEffectNode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor, const std::size_t index = 0 );
	ParamDouble* clone() const { return new ParamDouble( *this ); }

	double    getDefault() const;
	void get( double& ) const OFX_EXCEPTION_SPEC;
	void get( const OfxTime time, double& ) const OFX_EXCEPTION_SPEC;
	void set( const double&, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC;
	void set( const OfxTime time, const double&, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC;
	void derive( const OfxTime time, double& ) const OFX_EXCEPTION_SPEC;
	void integrate( const OfxTime time1, const OfxTime time2, double& ) const OFX_EXCEPTION_SPEC;
	void          copy( const ParamDouble& p ) OFX_EXCEPTION_SPEC
	{
		_value = p._value;
	}
	void          copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC
	{
		const ParamDouble& param = dynamic_cast<const ParamDouble&>(p);
		copy( param );
	}
};


}
}
}


#endif
