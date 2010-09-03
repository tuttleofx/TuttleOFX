#include "OfxhClipDescriptor.hpp"

namespace tuttle {
namespace host {
namespace ofx {
namespace attribute {

/**
 * descriptor
 */
OfxhClipDescriptor::OfxhClipDescriptor()
	: attribute::OfxhAttributeDescriptor()
{
	/// properties common to the desciptor and instance
	/// the desc and set them, the instance cannot
	static const property::OfxhPropSpec clipDescriptorStuffs[] = {
		{ kOfxPropType, property::eString, 1, true, kOfxTypeClip },
		{ kOfxImageClipPropOptional, property::eInt, 1, false, "0" },
		{ 0 },
	};

	getEditableProperties().addProperties( clipDescriptorStuffs );
}

/**
 * descriptor
 */
OfxhClipDescriptor::OfxhClipDescriptor( const property::OfxhSet& properties )
	: attribute::OfxhAttributeDescriptor( properties )
{
	/// properties common to the desciptor and instance
	/// the desc and set them, the instance cannot
	static const property::OfxhPropSpec clipDescriptorStuffs[] = {
		{ kOfxPropType, property::eString, 1, true, kOfxTypeClip },
		{ kOfxImageClipPropOptional, property::eInt, 1, false, "0" },
		{ 0 },
	};

	getEditableProperties().addProperties( clipDescriptorStuffs );
}

OfxhClipDescriptor::~OfxhClipDescriptor()
{}

}
}
}
}
