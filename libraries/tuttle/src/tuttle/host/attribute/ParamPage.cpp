#include "ParamPage.hpp"

namespace tuttle {
namespace host {
namespace attribute {

ParamPage::ParamPage( ImageEffectNode&                           effect,
                      const std::string&                         name,
                      const ofx::attribute::OfxhParamDescriptor& descriptor )
	: Param( effect )
	, ofx::attribute::OfxhParamPage( descriptor, name, effect )
{}

void ParamPage::copy( const ParamPage& p ) OFX_EXCEPTION_SPEC
{
	//	paramChanged( ofx::attribute::eChangeUserEdited );
}

void ParamPage::copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC
{
	const ParamPage& param = dynamic_cast<const ParamPage&>( p );

	copy( param );
}

}
}
}

