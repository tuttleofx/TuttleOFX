#include "ParamPushButton.hpp"

namespace tuttle {
namespace host {
namespace attribute {

ParamPushButton::ParamPushButton( ImageEffectNode&                                   effect,
                                  const std::string&                                 name,
                                  const ofx::attribute::OfxhParamDescriptor& descriptor )
	: Param( effect ),
	ofx::attribute::OfxhParamPushButton( descriptor, name, effect )
{
}

void ParamPushButton::copy( const ParamPushButton& p ) OFX_EXCEPTION_SPEC
{
	// do not action the push button on copy...
	//paramChanged( ofx::attribute::eChangeUserEdited );
}

void ParamPushButton::copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC
{
	const ParamPushButton& param = dynamic_cast<const ParamPushButton&>(p);
	copy( param );
}

}
}
}

