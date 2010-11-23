#include "ParamPage.hpp"

#include <tuttle/host/INode.hpp>

namespace tuttle {
namespace host {
namespace attribute {

ParamPage::ParamPage( INode&                           effect,
                      const std::string&                         name,
                      const ofx::attribute::OfxhParamDescriptor& descriptor )
	: Param( effect )
	, ofx::attribute::OfxhParamPage( descriptor, name, effect.getParamSet() )
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

