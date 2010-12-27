/**
 * This plugin uses fftw3 which is GPL licensed, thus, this plugin (only) is GPL licensed.
 */

#ifndef _TUTTLE_PLUGIN_IFFT_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_IFFT_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle {
namespace plugin {
namespace fftTransform {
namespace ifft {

const static std::string kSourcePhase  = "SourceMod";
const static std::string kSourceModule = "SourcePhase";

const static std::string kHelpButton = "Help";

const static std::string kHelpString = "<b>Ifft</b> plugin is used to inverse an fft transform.  <br />";

}
}
}
}

#endif
