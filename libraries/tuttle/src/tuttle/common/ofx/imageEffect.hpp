#ifndef _TUTTLE_COMMON_OFX_IMAGEEFFECT_HPP_
#define _TUTTLE_COMMON_OFX_IMAGEEFFECT_HPP_

#include <tuttle/common/exceptions.hpp>

#include <string>

namespace tuttle {
namespace ofx {
namespace imageEffect {

/** @brief Enumerates the contexts a plugin can be used in */
enum EContext
{
	eContextNone,
	eContextGenerator,
	eContextFilter,
	eContextTransition,
	eContextPaint,
	eContextGeneral,
	eContextRetimer,
	eContextReader,
	eContextWriter
};

const std::string mapContextEnumToString( const EContext s );

/** @brief Enumerates the pixel depths supported */
enum EBitDepth
{
	eBitDepthNone   = -1, ///< @brief bit depth that indicates no data is present
	eBitDepthCustom = 0, ///< some non standard bit depth
	eBitDepthUByte  = 1,
	eBitDepthUShort = 2,
	eBitDepthFloat  = 4
};

const std::string mapBitDepthEnumToString( const EBitDepth e );
EBitDepth         mapBitDepthStringToEnum( const std::string& str );

/** @brief Enumerates the component types supported */
enum EPixelComponent
{
	ePixelComponentNone,
	ePixelComponentRGBA,
	ePixelComponentRGB,
	ePixelComponentAlpha,
	ePixelComponentCustom ///< some non standard pixel type
};

std::string     mapPixelComponentEnumToString( const EPixelComponent e );
EPixelComponent mapPixelComponentStringToEnum( const std::string& str );

inline std::size_t numberOfComponents( const EPixelComponent c )
{
	switch( c )
	{
		case ePixelComponentRGBA:
			return 4;
		case ePixelComponentRGB:
			return 3;
		case ePixelComponentAlpha:
			return 1;
		case ePixelComponentNone:
			return 0;
		case ePixelComponentCustom:
			BOOST_THROW_EXCEPTION( exception::Value()
			    << exception::user() + "Can't retrieve the number of values inside a custom pixel component." );
	}
	return 0;
}

/// get me deepest bit depth
std::string findDeepestBitDepth( const std::string& s1, const std::string& s2 );

}
}
}

#endif

