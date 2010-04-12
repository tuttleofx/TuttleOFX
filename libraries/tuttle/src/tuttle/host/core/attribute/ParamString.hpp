#ifndef _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMSTRING_HPP_
#define _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMSTRING_HPP_

#include <tuttle/host/ofx/attribute/OfxhParamString.hpp>
#include <tuttle/host/core/ImageEffectNode.hpp>

namespace tuttle {
namespace host {
namespace core {

class ParamString : public ofx::attribute::OfxhParamString
{
protected:
	ImageEffectNode& _effect;
	std::string _value; /// @todo link de parametres

public:
	ParamString( ImageEffectNode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor );
	ParamString* clone() const { return new ParamString( *this ); }

	const std::string& getDefault() const;
	void          get( std::string& ) const OFX_EXCEPTION_SPEC;
	void          get( const OfxTime time, std::string& ) const OFX_EXCEPTION_SPEC;
	void          set( const char* ) OFX_EXCEPTION_SPEC;
	void          set( const std::string& value ) OFX_EXCEPTION_SPEC { _value = value; }
	void          set( const OfxTime time, const char* ) OFX_EXCEPTION_SPEC;
	void          set( const OfxTime time, const std::string& value ) OFX_EXCEPTION_SPEC { _value = value; }
	void          copy( const ParamString& p ) OFX_EXCEPTION_SPEC
	{
		_value = p._value;
	}
	void          copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC
	{
		const ParamString& param = dynamic_cast<const ParamString&>(p);
		copy( param );
	}
};

}
}
}

#endif
