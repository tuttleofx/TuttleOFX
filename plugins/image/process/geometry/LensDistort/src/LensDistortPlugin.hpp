#ifndef _TUTTLE_PLUGIN_LENSDISTORTPLUGIN_HPP_
#define _TUTTLE_PLUGIN_LENSDISTORTPLUGIN_HPP_

#include "lensDistortDefinitions.hpp"
#include "lensDistortProcessParams.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

#include <boost/gil/utilities.hpp>
#include <string>

namespace tuttle {
namespace plugin {
namespace lens {

/**
 * @brief Main class of the lens distortion
 */
class LensDistortPlugin : public ImageEffectGilPlugin
{
public:
	typedef double Scalar;
	typedef boost::gil::point2<double> Point2;

public:
	///@{
	OFX::Clip* _srcRefClip; ///< source ref image clip
	///@}

	///@{
	OFX::BooleanParam* _reverse; ///< reverse the effect
	OFX::BooleanParam* _displaySource; ///< do nothing (so host displays input)
	OFX::ChoiceParam* _lensType; ///< choice to select lens type
	OFX::DoubleParam* _coef1; ///< distortion coeffiscient
	OFX::DoubleParam* _coef2; ///< distortion coeffiscient for fish-eye lens
	OFX::DoubleParam* _squeeze; ///< squeeze coefficient horizontally/vertically (not implemented yet)
	OFX::Double2DParam* _asymmetric; ///< lens distortion is asymmetric horizontally/vertically (not implemented yet)
	OFX::Double2DParam* _center; ///< center coordonnates
	OFX::BooleanParam*  _centerOverlay; ///< lens center overlay
	OFX::ChoiceParam* _centerType; ///< centered the lens distortion on source RoD or image size (not implemented yet)
	OFX::DoubleParam* _postScale; ///< scale after applying the lens distortion
	OFX::DoubleParam* _preScale; ///< scale before applying the lens distortion
	OFX::ChoiceParam* _interpolation; ///< interpolation method
	OFX::ChoiceParam* _resizeRod; ///< Choice how to resize the RoD (default 'no' resize)
	OFX::DoubleParam* _resizeRodManualScale; ///< scale the output RoD

	OFX::GroupParam* _groupDisplayParams; ///< group of all overlay options (don't modify the output image)
	OFX::BooleanParam*  _gridOverlay; ///< grid overlay
	OFX::Double2DParam* _gridCenter; ///< grid center
	OFX::BooleanParam*  _gridCenterOverlay; ///< grid center overlay
	OFX::Double2DParam* _gridScale; ///< grid scale

	OFX::BooleanParam* _debugDisplayRoi; ///< debug display options

	OFX::PushButtonParam* _helpButton; ///< display help informations
	///@}

	///@{
	/// can't use static because it's common to all plugin instances...
	/// so it's not perfect but it's just here for debug purpose (used in overlay)
	static OfxRectD _dstRoi;
	static OfxRectD _srcRoi;
	static OfxRectD _srcRealRoi;
	///@}

public:
	LensDistortPlugin( OfxImageEffectHandle handle );

	void render( const OFX::RenderArguments& args );
	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );
	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
	void getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );

public:
	/**
	 * @todo pixelRatio !
	 */
	LensDistortProcessParams<Scalar> getProcessParams( const OfxRectD& inputRod, const OfxRectD& outputRod, const OfxRectD& optionalInputRod, const double pixelAspectRatio, const bool reverse = false ) const;
	LensDistortProcessParams<Scalar> getProcessParams( const OfxRectD& inputRod, const OfxRectD& outputRod, const double pixelAspectRatio, const bool reverse = false ) const
	{
		static const OfxRectD noOptionalInputRod = { 0, 0, 0, 0 };

		return getProcessParams( inputRod, outputRod, noOptionalInputRod, pixelAspectRatio, reverse );
	}

	const EParamLensType      getLensType() const      { return static_cast<EParamLensType>( _lensType->getValue() ); }
	const EParamInterpolation getInterpolation() const { return static_cast<EParamInterpolation>( _interpolation->getValue() ); }
	const EParamCenterType    getCenterType() const    { return static_cast<EParamCenterType>( _centerType->getValue() ); }
	const EParamResizeRod     getResizeRod() const     { return static_cast<EParamResizeRod>( _resizeRod->getValue() ); }

private:
	void initParamsProps();
};

}
}
}

#endif
