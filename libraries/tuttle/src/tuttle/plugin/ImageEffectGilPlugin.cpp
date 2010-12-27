#include "ImageEffectGilPlugin.hpp"

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {

ImageEffectGilPlugin::ImageEffectGilPlugin( OfxImageEffectHandle handle )
: OFX::ImageEffect( handle )
{
	_clipSrc = fetchClip( kOfxImageEffectSimpleSourceClipName );
	_clipDst = fetchClip( kOfxImageEffectOutputClipName );
}

ImageEffectGilPlugin::~ImageEffectGilPlugin()
{}

}
}
