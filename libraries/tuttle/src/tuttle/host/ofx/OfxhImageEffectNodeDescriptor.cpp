#include "OfxhImageEffectNodeDescriptor.hpp"
#include "OfxhPlugin.hpp"
#include "OfxhPluginBinary.hpp"

#include <tuttle/host/Core.hpp> /// @todo tuttle: please remove this ! (don't use as singleton)

namespace tuttle {
namespace host {
namespace ofx {
namespace imageEffect {


////////////////////////////////////////////////////////////////////////////////
// descriptor

/// properties common on an effect and a descriptor
static property::OfxhPropSpec effectDescriptorStuff[] = {
	/* name                                 type                   dim. r/o default value */
	{ kOfxPropType, property::eString, 1, true, kOfxTypeImageEffect },
	{ kOfxPropName, property::eString, 1, false, "UNIQUE_NAME_NOT_SET" },
	{ kOfxPropLabel, property::eString, 1, false, "" },
	{ kOfxPropShortLabel, property::eString, 1, false, "" },
	{ kOfxPropLongLabel, property::eString, 1, false, "" },
	{ kOfxImageEffectPropSupportedContexts, property::eString, 0, false, "" },
	{ kOfxImageEffectPluginPropGrouping, property::eString, 1, false, "" },
	{ kOfxImageEffectPluginPropSingleInstance, property::eInt, 1, false, "0" },
	{ kOfxImageEffectPluginRenderThreadSafety, property::eString, 1, false, kOfxImageEffectRenderInstanceSafe },
	{ kOfxImageEffectPluginPropHostFrameThreading, property::eInt, 1, false, "1" },
	{ kOfxImageEffectPluginPropOverlayInteractV1, property::ePointer, 1, false, NULL },
	{ kOfxImageEffectPropSupportsMultiResolution, property::eInt, 1, false, "1" },
	{ kOfxImageEffectPropSupportsTiles, property::eInt, 1, false, "1" },
	{ kOfxImageEffectPropTemporalClipAccess, property::eInt, 1, false, "0" },
	{ kOfxImageEffectPropSupportedPixelDepths, property::eString, 0, false, "" },
	{ kOfxImageEffectPluginPropFieldRenderTwiceAlways, property::eInt, 1, false, "1" },
	{ kOfxImageEffectPropSupportsMultipleClipDepths, property::eInt, 1, false, "0" },
	{ kOfxImageEffectPropSupportsMultipleClipPARs, property::eInt, 1, false, "0" },
	{ kOfxImageEffectPropClipPreferencesSlaveParam, property::eString, 0, false, "" },
	{ kOfxImageEffectInstancePropSequentialRender, property::eInt, 1, false, "0" },
	{ kOfxPluginPropFilePath, property::eString, 1, true, "" },
	{ 0 }
};

OfxhImageEffectNodeDescriptor::OfxhImageEffectNodeDescriptor()
	: OfxhImageEffectNodeBase( effectDescriptorStuff ),
	_plugin( NULL )
{
	/// @todo tuttle...
}

OfxhImageEffectNodeDescriptor::OfxhImageEffectNodeDescriptor( OfxhPlugin& plug )
	: OfxhImageEffectNodeBase( effectDescriptorStuff ),
	_plugin( &plug )
{
	_properties.setStringProperty( kOfxPluginPropFilePath, plug.getBinary().getBundlePath() );
	tuttle::host::Core::instance().getHost().initDescriptor( *this );
}

OfxhImageEffectNodeDescriptor::OfxhImageEffectNodeDescriptor( const OfxhImageEffectNodeDescriptor& other, OfxhPlugin& plug )
	: OfxhImageEffectNodeBase( other._properties ),
	_plugin( &plug )
{
	_properties.setStringProperty( kOfxPluginPropFilePath, plug.getBinary().getBundlePath() );
	tuttle::host::Core::instance().getHost().initDescriptor( *this );
}

OfxhImageEffectNodeDescriptor::OfxhImageEffectNodeDescriptor( const std::string& bundlePath, OfxhPlugin& plug )
	: OfxhImageEffectNodeBase( effectDescriptorStuff ),
	_plugin( &plug )
{
	_properties.setStringProperty( kOfxPluginPropFilePath, bundlePath );
	tuttle::host::Core::instance().getHost().initDescriptor( *this );
}

OfxhImageEffectNodeDescriptor::~OfxhImageEffectNodeDescriptor()
{}

bool OfxhImageEffectNodeDescriptor::operator==( const This& other ) const
{
	if( OfxhImageEffectNodeBase::operator!=(other) ||
	    attribute::OfxhParamSetDescriptor::operator!=(other) ||
	    _clipsByOrder != other._clipsByOrder )
		return false;
	return true;
}


/**
 * @brief create a new clip and add this to the clip map
 */
attribute::OfxhClipImageDescriptor* OfxhImageEffectNodeDescriptor::defineClip( const std::string& name )
{
	attribute::OfxhClipImageDescriptor* c = new attribute::OfxhClipImageDescriptor( name );

	_clips[name] = c;
	_clipsByOrder.push_back( c );
	return c;
}

/// @warning tuttle some modifs here, doc needs update
/// get the interact description, this will also call describe on the interact
void OfxhImageEffectNodeDescriptor::initOverlayDescriptor( int bitDepthPerComponent, bool hasAlpha )
{
	if( _overlayDescriptor.getState() == interact::eUninitialised )
	{
		// OK, we need to describe it, set the entry point and describe away
		_overlayDescriptor.setEntryPoint( getOverlayInteractMainEntry() );
		_overlayDescriptor.describe( bitDepthPerComponent, hasAlpha );
	}
}

void OfxhImageEffectNodeDescriptor::addClip( const std::string& name, attribute::OfxhClipImageDescriptor* clip )
{
	_clips[name] = clip;
	_clipsByOrder.push_back( clip );
}

}
}
}
}

