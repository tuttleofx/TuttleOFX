/**
 * This plugin uses fftw3 which is GPL licensed, thus, this plugin (only) is GPL licensed.
 */

#ifndef _TUTTLE_PLUGIN_FFTTRANSFORM_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_FFTTRANSFORM_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle {
namespace plugin {
namespace fftTransform {
namespace fft {

const static std::string kHelpButton = "Help";

const static std::string kFftTypeLong = "fftTypeLong";
const static std::string kFftType     = "fftType";

const static std::string kHelpString = "<b>Fft</b> plugin is used to apply an fft transform.  <br />";

enum EfftType { eFftMag, eFftMod, eFftPhase };

}
}
}
}

#endif
