#ifndef _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMINTEGER_HPP_
#define _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMINTEGER_HPP_

#include <tuttle/host/ofx/attribute/OfxhParamInteger.hpp>
#include <tuttle/host/core/ImageEffectNode.hpp>

namespace tuttle {
namespace host {
namespace core {

class ParamInteger : public ofx::attribute::OfxhParamInteger
{
protected:
	ImageEffectNode& _effect;
	int _value; /// @todo link de parametres

public:
	ParamInteger( ImageEffectNode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor, const std::size_t index = 0 );
	ParamInteger* clone() const { return new ParamInteger( *this ); }

	int       getDefault() const;
	void get( int& ) const OFX_EXCEPTION_SPEC;
	void get( const OfxTime time, int& ) const OFX_EXCEPTION_SPEC;
	void set( const int &) OFX_EXCEPTION_SPEC;
	void set( const OfxTime time, const int &) OFX_EXCEPTION_SPEC;
	void          copy( const ParamInteger& p ) OFX_EXCEPTION_SPEC
	{
		_value = p._value;
	}
	void          copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC
	{
		const ParamInteger& param = dynamic_cast<const ParamInteger&>(p);
		copy( param );
	}
};

}
}
}


#endif
