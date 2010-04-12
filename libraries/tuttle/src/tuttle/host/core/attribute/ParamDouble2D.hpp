#ifndef _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMDOUBLE2D_HPP_
#define _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMDOUBLE2D_HPP_

#include <tuttle/host/ofx/attribute/OfxhMultiDimParam.hpp>
#include "ParamDouble.hpp"
#include <tuttle/host/core/ImageEffectNode.hpp>

namespace tuttle {
namespace host {
namespace core {


class ParamDouble2D : public ofx::attribute::OfxhMultiDimParam<ParamDouble, 2 >
{
protected:
	ImageEffectNode& _effect;

	OfxPointD _value;

public:
	ParamDouble2D( ImageEffectNode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor );
	ParamDouble2D* clone() const { return new ParamDouble2D( *this ); }

	OfxPointD getDefault() const;
	void get( double& x, double& y ) const OFX_EXCEPTION_SPEC;
	void get( const OfxTime time, double& x, double& y ) const OFX_EXCEPTION_SPEC;
	void set( const double &x, const double &y ) OFX_EXCEPTION_SPEC;
	void set( const OfxTime time, const double &x, const double &y ) OFX_EXCEPTION_SPEC;
	void copy( const ParamDouble2D& p ) OFX_EXCEPTION_SPEC
	{
		_value = p._value;
	}
	void          copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC
	{
		const ParamDouble2D& param = dynamic_cast<const ParamDouble2D&>(p);
		copy( param );
	}
};


}
}
}


#endif
