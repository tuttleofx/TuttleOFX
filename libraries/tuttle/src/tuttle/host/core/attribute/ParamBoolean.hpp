#ifndef _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMBOOLEAN_HPP_
#define _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMBOOLEAN_HPP_

#include <tuttle/host/ofx/attribute/OfxhParamBoolean.hpp>
#include <tuttle/host/core/ImageEffectNode.hpp>

namespace tuttle {
namespace host {
namespace core {


class ParamBoolean : public ofx::attribute::OfxhParamBoolean
{
protected:
	ImageEffectNode& _effect;

	bool _value;

public:
	ParamBoolean( ImageEffectNode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor );
	virtual ParamBoolean* clone() const { return new ParamBoolean( *this ); }

	bool      getDefault() const;
	void get( bool& ) const OFX_EXCEPTION_SPEC;
	void get( const OfxTime time, bool& ) const OFX_EXCEPTION_SPEC;
	void set( const bool& ) OFX_EXCEPTION_SPEC;
	void set( const OfxTime time, const bool& ) OFX_EXCEPTION_SPEC;
	void copy( const ParamBoolean& p ) OFX_EXCEPTION_SPEC
	{
		_value = p._value;
	}
	void copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC
	{
		const ParamBoolean& param = dynamic_cast<const ParamBoolean&>(p);
		copy( param );
	}
};


}
}
}

#endif
