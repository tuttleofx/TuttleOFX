#include "Attribute.hpp"

#include <tuttle/host/ImageEffectNode.hpp>

namespace tuttle {
namespace host {
namespace attribute {

Attribute::Attribute( ImageEffectNode& effect )
: _effect(effect)
{
}
Attribute::~Attribute() {}

const ImageEffectNode& Attribute::getNode() const
{
	return _effect;
}

}
}
}
