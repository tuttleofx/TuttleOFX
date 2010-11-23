#ifndef _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMSTRING_HPP_
#define _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMSTRING_HPP_

#include "Param.hpp"

#include <tuttle/host/ofx/attribute/OfxhParamString.hpp>
#include <tuttle/host/ImageEffectNode.hpp>

namespace tuttle {
namespace host {
namespace attribute {

class ParamString : public Param
	, public ofx::attribute::OfxhParamString
{
protected:
	std::string _value; /// @todo link de parametres

public:
	ParamString( INode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor );
	ParamString* clone() const { return new ParamString( *this ); }

	const std::string& getDefault() const;

	void get( std::string& ) const OFX_EXCEPTION_SPEC;
	void getAtTime( const OfxTime time, std::string& ) const OFX_EXCEPTION_SPEC;
	void set( const std::string& value, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC;
	void setAtTime( const OfxTime time, const std::string& value, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC;

	void copy( const ParamString& p ) OFX_EXCEPTION_SPEC;
	void copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC;
};

}
}
}

#endif
