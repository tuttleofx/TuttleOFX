#include "ParamGroup.hpp"

namespace tuttle {
namespace host {
namespace core {

ParamGroup::ParamGroup( ImageEffectNode&                                   effect,
					  const std::string&                                 name,
					  const ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhParamGroup( descriptor, name, effect ),
	_effect( effect )
{}

void ParamGroup::copy( const ParamGroup& p ) OFX_EXCEPTION_SPEC
{
	//paramChanged( ofx::attribute::eChangeUserEdited );
}

void ParamGroup::copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC
{
	const ParamGroup& param = dynamic_cast<const ParamGroup&>(p);
	copy( param );
}

}
}
}

