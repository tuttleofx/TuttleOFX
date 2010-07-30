#ifndef _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMCHOICE_HPP_
#define _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMCHOICE_HPP_

#include <tuttle/host/ofx/attribute/OfxhParamChoice.hpp>
#include <tuttle/host/core/ImageEffectNode.hpp>

namespace tuttle {
namespace host {
namespace core {

class ParamChoice : public ofx::attribute::OfxhParamChoice
{
protected:
	ImageEffectNode& _effect;

	int _value;

public:
	ParamChoice( ImageEffectNode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor );
	ParamChoice* clone() const { return new ParamChoice( *this ); }

	int getDefault() const;
	
	void get( int& ) const OFX_EXCEPTION_SPEC;
	void getAtTime( const OfxTime time, int& ) const OFX_EXCEPTION_SPEC;
	void set( const int&, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC;
	void setAtTime( const OfxTime time, const int&, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC;
	
	void copy( const ParamChoice& p ) OFX_EXCEPTION_SPEC
	{
		_value = p._value;
	}
	void copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC
	{
		const ParamChoice& param = dynamic_cast<const ParamChoice&>(p);
		copy( param );
	}
};

}
}
}


#endif
