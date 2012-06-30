#include "Attribute.hpp"

#include <tuttle/host/INode.hpp>

namespace tuttle {
namespace host {
namespace attribute {

Attribute::Attribute( INode& effect )
	: _effect( effect )
{}

Attribute::Attribute( const Attribute& other )
	: _effect( other._effect )
{}

Attribute::~Attribute() {}

Attribute& Attribute::operator=( const Attribute& other )
{}

const INode& Attribute::getNode() const
{
	return _effect;
}

}
}
}
