#ifndef _TUTTLE_HOST_OFX_IMAGEEFFECTHOST_HPP_
#define _TUTTLE_HOST_OFX_IMAGEEFFECTHOST_HPP_

#include <tuttle/host/exceptionsHost.hpp>

#include <ofxCore.h>
#include <ofxImageEffect.h>

#include "OfxhHost.hpp"
#include "OfxhProgress.hpp"
#include "OfxhTimeline.hpp"
#include "OfxhMemory.hpp"
#include "OfxhInteract.hpp"
#include "attribute/OfxhParam.hpp"
#include "attribute/OfxhClipImage.hpp"

namespace tuttle {
namespace host {
namespace ofx {

// forward declare
class OfxhPlugin;

namespace attribute {
class OfxhClipDescriptor;
class OfxhClip;
}

namespace imageEffect {

// forward declare
class OfxhImageEffectPlugin;
class OfxhOverlayInstance;
class OfxhImageEffectNode;
class OfxhImageEffectNodeDescriptor;

/**
 * An image effect host, passed to the setHost function of all image effect plugins
 */
class OfxhImageEffectHost : public OfxhHost
{
public:
	OfxhImageEffectHost();
	virtual ~OfxhImageEffectHost();

	/// fetch a suite
	virtual void* fetchSuite( const char* suiteName, const int suiteVersion );

	#ifndef SWIG
	/**
	 * Create a new instance of an image effect plug-in.
	 *
	 * It is called by ImageEffectPlugin::createInstance which the
	 * client code calls when it wants to make a new instance.
	 *
	 * param: clientData - the clientData passed into the ImageEffectPlugin::createInstance (tuttle remove this parameter)
	 *   @param plugin - the plugin being created
	 *   @param desc - the descriptor for that plugin
	 *   @param context - the context to be created in
	 */
	virtual OfxhImageEffectNode* newInstance( OfxhImageEffectPlugin&         plugin,
	                                          OfxhImageEffectNodeDescriptor& desc,
	                                          const std::string&             context ) const = 0;

	/**
	 * Function called as each plugin binary is found and loaded from disk
	 *
	 * Use this in any dialogue etc... showing progress
	 */
	virtual void loadingStatus( const std::string& );

	/**
	 * Override this to filter out plugins which the host can't support for whatever reason
	 *
	 *   @param plugin - the plugin to examine
	 *   @param reason - set this to report the reason the plugin was not loaded
	 */
	virtual bool pluginSupported( const OfxhImageEffectPlugin& plugin, std::string& reason ) const;

	/// Override this to create a descriptor, this makes the 'root' descriptor
	virtual OfxhImageEffectNodeDescriptor* makeDescriptor( OfxhImageEffectPlugin& plugin ) const = 0;

	/// used to construct a context description, rootContext is the main context
	virtual OfxhImageEffectNodeDescriptor* makeDescriptor( const OfxhImageEffectNodeDescriptor& rootContext, OfxhImageEffectPlugin& plug ) const = 0;

	/// used to construct populate the cache
	virtual OfxhImageEffectNodeDescriptor* makeDescriptor( const std::string& bundlePath, OfxhImageEffectPlugin& plug ) const = 0;

	/**
	 *  Override this to initialise an image effect descriptor after it has been
	 *  created.
	 */
	virtual void initDescriptor( OfxhImageEffectNodeDescriptor& desc ) const;
	#endif
};

}
}
}
}

#endif

