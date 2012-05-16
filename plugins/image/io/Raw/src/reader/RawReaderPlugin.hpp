#ifndef _TUTTLE_PLUGIN_RAWREADERPLUGIN_HPP_
#define _TUTTLE_PLUGIN_RAWREADERPLUGIN_HPP_

#include "RawReaderDefinitions.hpp"
#include <tuttle/plugin/context/ReaderPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace raw {
namespace reader {

template<typename Scalar>
struct RawReaderProcessParams
{
	std::string    _filepath;       ///< filepath
	EFiltering     _filtering;
	EInterpolation _interpolation;
	float          _gammaPower;
	float          _gammaToe;
	double         _redAbber;
	double         _greenAbber;

	boost::gil::point2<Scalar> _greyboxPoint;
	boost::gil::point2<Scalar> _greyboxSize;
	
	float                      _bright;
};

/**
 * @brief Raw reader
 *
 */
class RawReaderPlugin : public ReaderPlugin
{
public:
	typedef float Scalar;
	typedef boost::gil::point2<double> Point2;
public:
	RawReaderPlugin( OfxImageEffectHandle handle );

public:
	RawReaderProcessParams<Scalar> getProcessParams( const OfxTime time );

	void updateInfos( const OfxTime time );

	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
	void getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );

	void render( const OFX::RenderArguments& args );

public:
	/// @name user parameters
	/// @{
	OFX::ChoiceParam*    _paramFiltering;    ///< Filtering mode
	OFX::ChoiceParam*    _paramInterpolation;
	
	OFX::DoubleParam*    _paramGammaPower;
	OFX::DoubleParam*    _paramGammaToe;
	OFX::DoubleParam*    _paramRedAbber;
	OFX::DoubleParam*    _paramGreenAbber;
	
	
	OFX::DoubleParam*    _paramBright;
	
	OFX::Double2DParam*    _paramGreyboxPoint;
	OFX::Double2DParam*    _paramGreyboxSize;
	/// @}
};

}
}
}
}

#endif
