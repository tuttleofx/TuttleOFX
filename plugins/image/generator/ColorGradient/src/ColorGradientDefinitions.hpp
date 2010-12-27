#ifndef _TUTTLE_PLUGIN_COLORGRADIENT_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_COLORGRADIENT_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

#include <boost/lexical_cast.hpp>

namespace tuttle {
namespace plugin {
namespace colorGradient {

const static unsigned int kMaxNbPoints = 50;
const static std::string kPoint        = "point";
const static std::string kColor        = "color";

const static std::string kGradientType         = "type";
const static std::string kGradientType1DLinear = "1D linear";
const static std::string kGradientType1DRadial = "1D radial";
const static std::string kGradientType2D       = "2D";

enum EGradientType
{
	eGradientType1DLinear = 0,
	eGradientType1DRadial,
	eGradientType2D
};

const static std::string kNbPoints = "nbPoints";

inline std::string getPointParamName( const unsigned int i )
{
	return kPoint + boost::lexical_cast<std::string>( i );
}

inline std::string getColorParamName( const unsigned int i )
{
	return kColor + boost::lexical_cast<std::string>( i );
}

}
}
}

#endif
