/**
 * This plugin uses fftw3 which is GPL licensed, thus, this plugin (only) is GPL licensed.
 */

#include <ofxsImageEffect.h>
#include "fft/FftPluginFactory.hpp"
#include "ifft/IfftPluginFactory.hpp"

namespace OFX
{
namespace Plugin
{
void getPluginIDs( OFX::PluginFactoryArray& ids )
{
	static tuttle::plugin::fftTransform::fft::FftPluginFactory p1( "fr.tuttle.fft", 1, 0 );

	ids.push_back( &p1 );
	static tuttle::plugin::fftTransform::ifft::IfftPluginFactory p2( "fr.tuttle.ifft", 1, 0 );
	ids.push_back( &p2 );
}

}
}
