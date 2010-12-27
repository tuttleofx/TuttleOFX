#ifndef _TUTTLE_PLUGIN_BASICKEYER_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_BASICKEYER_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

#include <boost/lexical_cast.hpp>

namespace tuttle {
namespace plugin {
namespace basicKeyer {

const static std::string kParamMode         = "mode";
const static std::string kParamModePosition = "position";
const static std::string kParamModeColor    = "color";
enum EParamMode
{
	eParamModePosition = 0,
	eParamModeColor
};

const static std::string kParamNbPoints = "nbPoints";
const static unsigned int kMaxNbPoints  = 50;
const static std::string kParamPoint    = "point";
const static std::string kParamColor    = "color";
const static std::string kParamMargin   = "margin";
const static std::string kParamSoftness = "softness";

inline std::string getPointParamName( const unsigned int i )
{
	return kParamPoint + boost::lexical_cast<std::string>( i );
}

inline std::string getColorParamName( const unsigned int i )
{
	return kParamColor + boost::lexical_cast<std::string>( i );
}

}
}
}

#endif
