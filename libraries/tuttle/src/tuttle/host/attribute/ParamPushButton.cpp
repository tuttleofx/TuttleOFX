#include "ParamPushButton.hpp"

#include <tuttle/host/INode.hpp>

namespace tuttle {
namespace host {
namespace attribute {

ParamPushButton::ParamPushButton( INode&                           effect,
                                  const std::string&                         name,
                                  const ofx::attribute::OfxhParamDescriptor& descriptor )
	: Param( effect )
	, ofx::attribute::OfxhParamPushButton( descriptor, name, effect.getParamSet() )
{}

void ParamPushButton::copy( const ParamPushButton& p ) OFX_EXCEPTION_SPEC
{
	// do not action the push button on copy...
	//paramChanged( ofx::attribute::eChangeUserEdited );
}

void ParamPushButton::copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC
{
	const ParamPushButton& param = dynamic_cast<const ParamPushButton&>( p );

	copy( param );
}

}
}
}

