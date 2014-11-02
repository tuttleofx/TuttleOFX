#include "ParamDouble.hpp"

#include <tuttle/host/INode.hpp>

namespace tuttle {
namespace host {
namespace attribute {

ParamDouble::ParamDouble( INode&                           effect,
                          const std::string&                         name,
                          const ofx::attribute::OfxhParamDescriptor& descriptor,
                          const std::size_t                          index )
  : AnimatedParamDouble( effect, name, descriptor, index, getDefault() )
{
}

double ParamDouble::getDefault() const
{
	return getProperties().getDoubleProperty( kOfxParamPropDefault, _index );
}


}
}
}

