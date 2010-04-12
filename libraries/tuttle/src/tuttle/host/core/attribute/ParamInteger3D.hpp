#ifndef _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMINTEGER3D_HPP_
#define _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMINTEGER3D_HPP_

#include "ParamInteger.hpp"
#include <tuttle/host/ofx/attribute/OfxhMultiDimParam.hpp>
#include <tuttle/host/core/ImageEffectNode.hpp>

namespace tuttle {
namespace host {
namespace core {

class ParamInteger3D : public ofx::attribute::OfxhMultiDimParam<ParamInteger, 3 >
{
protected:
	ImageEffectNode& _effect;

	Ofx3DPointI _value;

public:
	ParamInteger3D( ImageEffectNode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor );
	ParamInteger3D* clone() const { return new ParamInteger3D( *this ); }

	Ofx3DPointI getDefault() const;
	void get( int& x, int& y, int& z ) const OFX_EXCEPTION_SPEC;
	void get( const OfxTime time, int& x, int& y, int& z ) const OFX_EXCEPTION_SPEC;
	void set( const int &x, const int &y, const int &z ) OFX_EXCEPTION_SPEC;
	void set( const OfxTime time, const int &x, const int &y, const int &z ) OFX_EXCEPTION_SPEC;
	void copy( const ParamInteger3D& p ) OFX_EXCEPTION_SPEC
	{
		_value = p._value;
	}
	void          copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC
	{
		const ParamInteger3D& param = dynamic_cast<const ParamInteger3D&>(p);
		copy( param );
	}
};


}
}
}

#endif
