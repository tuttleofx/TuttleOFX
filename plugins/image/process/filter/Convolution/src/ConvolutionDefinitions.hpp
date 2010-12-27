#ifndef _TUTTLE_PLUGIN_CONVOLUTION_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_CONVOLUTION_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

#include <boost/lexical_cast.hpp>

namespace tuttle {
namespace plugin {
namespace convolution {

const static std::string kParamSize     = "size";
const static unsigned int kParamSizeMax = 9;
const static std::string kParamCoef     = "coef_";

const static std::string kHelpButton = "Help";
const static std::string kHelpString = "<b>Convolution</b> plugin is used to ???.  <br />";

inline std::string getCoefName( const unsigned int y, const unsigned int x )
{
	return kParamCoef + boost::lexical_cast<std::string>( y ) + "_" + boost::lexical_cast<std::string>( x );
}

}
}
}

#endif
