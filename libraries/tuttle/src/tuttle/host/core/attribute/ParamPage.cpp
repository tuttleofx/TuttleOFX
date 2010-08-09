#include "ParamPage.hpp"

namespace tuttle {
namespace host {
namespace core {

ParamPage::ParamPage( ImageEffectNode&                                   effect,
                                  const std::string&                                 name,
                                  const ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhParamPage( descriptor, name, effect ),
	_effect( effect )
{}

void ParamPage::copy( const ParamPage& p ) OFX_EXCEPTION_SPEC
{
//	paramChanged( ofx::attribute::eChangeUserEdited );
}

void ParamPage::copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC
{
	const ParamPage& param = dynamic_cast<const ParamPage&>(p);
	copy( param );
}

}
}
}

