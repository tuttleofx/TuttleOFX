#ifndef _TUTTLE_HOST_ATTRIBUTE_HPP_
#define _TUTTLE_HOST_ATTRIBUTE_HPP_

#include <string>

namespace tuttle {
namespace host {

class ImageEffectNode;

namespace attribute {

class Attribute
{
protected:
	/*const*/ ImageEffectNode& _effect;

public:
	Attribute( ImageEffectNode& effect );
	virtual ~Attribute() = 0;

	virtual const std::string& getName() const = 0;
	const ImageEffectNode&     getNode() const;
};

}
}
}

#endif

