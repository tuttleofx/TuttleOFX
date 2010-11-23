#include "ParamGroup.hpp"

#include <tuttle/host/INode.hpp>

namespace tuttle {
namespace host {
namespace attribute {

ParamGroup::ParamGroup( INode&                           effect,
                        const std::string&                         name,
                        const ofx::attribute::OfxhParamDescriptor& descriptor )
	: Param( effect )
	, ofx::attribute::OfxhParamGroup( descriptor, name, effect.getParamSet() )
{}

void ParamGroup::copy( const ParamGroup& p ) OFX_EXCEPTION_SPEC
{
	//paramChanged( ofx::attribute::eChangeUserEdited );
}

void ParamGroup::copy( const OfxhParam& p ) OFX_EXCEPTION_SPEC
{
	const ParamGroup& param = dynamic_cast<const ParamGroup&>( p );

	copy( param );
}

}
}
}

