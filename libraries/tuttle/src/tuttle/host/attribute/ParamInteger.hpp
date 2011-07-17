#ifndef _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMINTEGER_HPP_
#define _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMINTEGER_HPP_

#include "Param.hpp"

#include <tuttle/host/ofx/attribute/OfxhParamInteger.hpp>

namespace tuttle {
namespace host {
namespace attribute {

class ParamInteger : public Param
	, public ofx::attribute::OfxhParamInteger
{
protected:
	int _value; /// @todo link de parametres

public:
	ParamInteger( INode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor, const std::size_t index = 0 );
	ParamInteger* clone() const { return new ParamInteger( *this ); }

	int getDefault() const;

	void getValue( int& ) const OFX_EXCEPTION_SPEC;
	void getValueAtTime( const OfxTime time, int& ) const OFX_EXCEPTION_SPEC;
	void setValue( const int&, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC;
	void setValueAtTime( const OfxTime time, const int&, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC;

	void setValueFromExpression( const std::string& value, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC;

	void copy( const ParamInteger& p ) OFX_EXCEPTION_SPEC;
	void copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC;
};

}
}
}

#endif
