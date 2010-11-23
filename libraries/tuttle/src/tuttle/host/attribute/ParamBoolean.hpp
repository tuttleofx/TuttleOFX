#ifndef _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMBOOLEAN_HPP_
#define _TUTTLE_HOST_CORE_ATTRIBUTE_PARAMBOOLEAN_HPP_

#include "Param.hpp"

#include <tuttle/host/ofx/attribute/OfxhParamBoolean.hpp>

namespace tuttle {
namespace host {
namespace attribute {

class ParamBoolean : public Param
	, public ofx::attribute::OfxhParamBoolean
{
protected:
	bool _value;

public:
	ParamBoolean( INode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor, const std::size_t index = 0 );
	virtual ParamBoolean* clone() const { return new ParamBoolean( *this ); }

	bool getDefault() const;

	void get( bool& ) const OFX_EXCEPTION_SPEC;
	void getAtTime( const OfxTime time, bool& ) const OFX_EXCEPTION_SPEC;
	void set( const bool&, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC;
	void setAtTime( const OfxTime time, const bool&, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC;

	void copy( const ParamBoolean& p ) OFX_EXCEPTION_SPEC;
	void copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC;
};

}
}
}

#endif
