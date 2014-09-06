#include "ParamInteger.hpp"

#include <tuttle/host/INode.hpp>

namespace tuttle {
namespace host {
namespace attribute {

ParamInteger::ParamInteger( INode&                           effect,
                            const std::string&                         name,
                            const ofx::attribute::OfxhParamDescriptor& descriptor,
                            const std::size_t                          index )
  : AnimatedParamInteger( effect, name, descriptor, index, getDefault() )
{
}

int ParamInteger::getDefault() const
{
	return getProperties().getIntProperty( kOfxParamPropDefault );
}

}
}
}

