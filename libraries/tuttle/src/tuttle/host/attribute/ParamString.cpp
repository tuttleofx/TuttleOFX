#include "ParamString.hpp"

#include <tuttle/host/INode.hpp>

namespace tuttle {
namespace host {
namespace attribute {

ParamString::ParamString( INode&                           effect,
                          const std::string&                         name,
                          const ofx::attribute::OfxhParamDescriptor& descriptor )
	: Param( effect )
	, ofx::attribute::OfxhParamString( descriptor, name, effect.getParamSet() )
{
	_value = getDefault();
}

const std::string& ParamString::getDefault() const
{
	return getProperties().getStringProperty( kOfxParamPropDefault );
}

void ParamString::get( std::string& v ) const OFX_EXCEPTION_SPEC
{
	v = _value;
}

void ParamString::getAtTime( const OfxTime time, std::string& v ) const OFX_EXCEPTION_SPEC
{
	v = _value; ///< @todo: in time !
}

void ParamString::set( const std::string& v, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC
{
	_value = v;
	this->paramChanged( change );
}

void ParamString::setAtTime( const OfxTime time, const std::string& v, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC
{
	_value = v; ///< @todo: in time !
	this->paramChanged( change );
}

void ParamString::copy( const ParamString& p ) OFX_EXCEPTION_SPEC
{
	_value = p._value;
	//	paramChanged( ofx::attribute::eChangeUserEdited );
}

void ParamString::copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC
{
	const ParamString& param = dynamic_cast<const ParamString&>( p );

	copy( param );
}

}
}
}

