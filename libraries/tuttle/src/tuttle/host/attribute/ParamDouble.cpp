#include "ParamDouble.hpp"

#include <tuttle/host/INode.hpp>

namespace tuttle {
namespace host {
namespace attribute {

ParamDouble::ParamDouble( INode&                           effect,
                          const std::string&                         name,
                          const ofx::attribute::OfxhParamDescriptor& descriptor,
                          const std::size_t                          index )
	: Param( effect )
	, ofx::attribute::OfxhParamDouble( descriptor, name, effect.getParamSet(), index )
{
	_value = getDefault();
}

double ParamDouble::getDefault() const
{
	return getProperties().getDoubleProperty( kOfxParamPropDefault, _index );
}

void ParamDouble::get( double& v ) const OFX_EXCEPTION_SPEC
{
	v = _value;
}

void ParamDouble::getAtTime( const OfxTime time, double& v ) const OFX_EXCEPTION_SPEC
{
	v = _value; ///< @todo: in time !
}

void ParamDouble::set( const double& v, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC
{
	_value = v;
	this->paramChanged( change );
}

void ParamDouble::setAtTime( const OfxTime time, const double& v, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC
{
	_value = v; ///< @todo: in time !
	this->paramChanged( change );
}

void ParamDouble::derive( const OfxTime time, double& ) const OFX_EXCEPTION_SPEC
{
	BOOST_THROW_EXCEPTION( ofx::OfxhException( kOfxStatErrMissingHostFeature ) );
}

void ParamDouble::integrate( const OfxTime time1, const OfxTime time2, double& ) const OFX_EXCEPTION_SPEC
{
	BOOST_THROW_EXCEPTION( ofx::OfxhException( kOfxStatErrMissingHostFeature ) );
}

void ParamDouble::copy( const ParamDouble& p ) OFX_EXCEPTION_SPEC
{
	_value = p._value;
	//	paramChanged( ofx::attribute::eChangeUserEdited );
}

void ParamDouble::copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC
{
	const ParamDouble& param = dynamic_cast<const ParamDouble&>( p );

	copy( param );
}

}
}
}

