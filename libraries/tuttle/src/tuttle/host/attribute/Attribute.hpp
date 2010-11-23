#ifndef _TUTTLE_HOST_ATTRIBUTE_HPP_
#define _TUTTLE_HOST_ATTRIBUTE_HPP_

#include <string>

namespace tuttle {
namespace host {

class INode;

namespace attribute {

class Attribute
{
protected:
	/*const*/ INode& _effect;

public:
	Attribute( INode& effect );
	virtual ~Attribute() = 0;

	virtual const std::string& getName() const = 0;
	const INode&     getNode() const;
};

}
}
}

#endif

