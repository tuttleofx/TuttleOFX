#include "OfxhAttribute.hpp"

namespace tuttle {
namespace host {
namespace ofx {
namespace attribute {

OfxhAttribute::OfxhAttribute( const property::OfxhSet& properties )
	: _properties( properties )
{
	assert( getAttributeType().c_str() );
}

OfxhAttribute::OfxhAttribute( const OfxhAttributeDescriptor& desc )
	: _properties( desc.getProperties() )
{
	assert( getAttributeType().c_str() );
}

OfxhAttribute::~OfxhAttribute() {}

}
}
}
}
