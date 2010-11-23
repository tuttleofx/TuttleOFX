#include "ParamInteger.hpp"

#include <tuttle/host/INode.hpp>

namespace tuttle {
namespace host {
namespace attribute {

ParamInteger::ParamInteger( INode&                           effect,
                            const std::string&                         name,
                            const ofx::attribute::OfxhParamDescriptor& descriptor,
                            const std::size_t                          index )
	: Param( effect )
	, ofx::attribute::OfxhParamInteger( descriptor, name, effect.getParamSet(), index )
{
	_value = getDefault();
}

int ParamInteger::getDefault() const
{
	return getProperties().getIntProperty( kOfxParamPropDefault );
}

void ParamInteger::get( int& v ) const OFX_EXCEPTION_SPEC
{
	v = _value;
}

void ParamInteger::getAtTime( const OfxTime time, int& v ) const OFX_EXCEPTION_SPEC
{
	v = _value; ///< @todo: in time !
}

void ParamInteger::set( const int& v, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC
{
	_value = v;
	this->paramChanged( change );
}

void ParamInteger::setAtTime( const OfxTime time, const int& v, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC
{
	_value = v; ///< @todo: in time !
	this->paramChanged( change );
}

void ParamInteger::copy( const ParamInteger& p ) OFX_EXCEPTION_SPEC
{
	_value = p._value;
	//	paramChanged( ofx::attribute::eChangeUserEdited );
}

void ParamInteger::copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC
{
	const ParamInteger& param = dynamic_cast<const ParamInteger&>( p );

	copy( param );
}

}
}
}

