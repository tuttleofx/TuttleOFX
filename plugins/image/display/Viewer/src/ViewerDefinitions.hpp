#ifndef _TUTTLE_PLUGIN_VIEWER_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_VIEWER_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle
{
namespace plugin
{
namespace viewer
{

static const std::string kViewerHelp = "Tuttle Viewer Help\n"
                                       "i                  : information about image (dimensions, bit depth, channels)\n"
                                       "z                  : zoom view to 1:1\n"
                                       "h, F1              : print help\n"
                                       "SHIFT + V          : flip\n"
                                       "SHIFT + H          : flop\n"
                                       "clic on image      : print RGBA values\n"
                                       "ESC, Return, Space : quit and continue process";
}
}
}

#endif
