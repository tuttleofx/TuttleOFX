#include "ParamDouble.hpp"

#include <tuttle/host/INode.hpp>

namespace tuttle
{
namespace host
{
namespace attribute
{

ParamDouble::ParamDouble(INode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor,
                         const std::size_t index)
    : AnimatedParamDouble(effect, name, descriptor, index, 0.0)
{
    this->_value = getDefault();
}

double ParamDouble::getDefault() const
{
    return getProperties().getDoubleProperty(kOfxParamPropDefault, this->_index);
}
}
}
}
