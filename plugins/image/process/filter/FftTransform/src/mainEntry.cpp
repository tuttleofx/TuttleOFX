#define OFXPLUGIN_VERSION_MAJOR 1
#define OFXPLUGIN_VERSION_MINOR 0

/**
 * This plugin uses fftw3 which is GPL licensed, thus, this plugin (only) is GPL licensed.
 */

#include <tuttle/plugin/Plugin.hpp>
#include "fft/FftPluginFactory.hpp"
#include "ifft/IfftPluginFactory.hpp"

namespace OFX
{
namespace Plugin
{
void getPluginIDs( OFX::PluginFactoryArray& ids )
{
	mAppendPluginFactory( ids, tuttle::plugin::fftTransform::fft::FftPluginFactory, "fr.tuttle.fft" );
	mAppendPluginFactory( ids, tuttle::plugin::fftTransform::ifft::IfftPluginFactory, "fr.tuttle.ifft" );
}

}
}
