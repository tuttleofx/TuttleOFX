#ifndef _TUTTLE_PLUGIN_CONVOLUTION_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_CONVOLUTION_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

#include <boost/lexical_cast.hpp>

namespace tuttle {
namespace plugin {
namespace convolution {

static const std::string kParamSize     = "size";
static const unsigned int kParamSizeMax = 9;
static const std::string kParamCoef     = "coef_";

static const std::string kHelpButton = "Help";
static const std::string kHelpString = "<b>Convolution</b> plugin is used to ???.  <br />";

inline std::string getCoefName( const unsigned int y, const unsigned int x )
{
	return kParamCoef + boost::lexical_cast<std::string>( y ) + "_" + boost::lexical_cast<std::string>( x );
}

}
}
}

#endif
