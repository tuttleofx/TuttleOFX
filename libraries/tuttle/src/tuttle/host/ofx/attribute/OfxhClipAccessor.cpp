#include "OfxhClipAccessor.hpp"

namespace tuttle {
namespace host {
namespace ofx {
namespace attribute {

OfxhClipAccessor::OfxhClipAccessor()
{}

OfxhClipAccessor::~OfxhClipAccessor()
{}

/**
 * is the clip optional
 */
bool OfxhClipAccessor::isOptional() const
{
	return getProperties().getIntProperty( kOfxImageClipPropOptional ) != 0;
}

}
}
}
}

