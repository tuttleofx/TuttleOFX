#ifndef _TUTTLE_PLUGIN_RESIZE_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_RESIZE_PLUGIN_HPP_

#include "ResizeDefinitions.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace resize {

template<typename Scalar>
struct ResizeProcessParams
{
	boost::gil::point2<Scalar> _size;
	bool                       _changeCenter;
	boost::gil::point2<Scalar> _centerPoint;

	EParamFilter _filter;
	double       _filterSize;
	double       _paramB;
	double       _paramC;

        EParamFilterOutOfImage     _outOfImageProcess;
};

/**
 * @brief Resize plugin
 */
class ResizePlugin : public ImageEffectGilPlugin
{
public:
	typedef float Scalar;
	typedef boost::gil::point2<double> Point2;

public:
	ResizePlugin( OfxImageEffectHandle handle );

public:
	ResizeProcessParams<Scalar> getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

	void updateVisibleTools();

	void changedParam          ( const OFX::InstanceChangedArgs &args, const std::string &paramName );

	bool getRegionOfDefinition ( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
	void getRegionsOfInterest  ( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );
	bool isIdentity            ( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );

	void render                ( const OFX::RenderArguments &args );

public:
	OFX::ChoiceParam*	_paramOptions;

	OFX::ChoiceParam*	_paramFormat;

	OFX::BooleanParam*	_paramSplit;

	OFX::ChoiceParam*	_paramDirection;
	OFX::DoubleParam*	_paramScale;
	OFX::DoubleParam*	_paramSize;

	OFX::Double2DParam*	_paramOutputFormat;
	OFX::DoubleParam*	_paramScaleX;
	OFX::DoubleParam*	_paramScaleY;

	OFX::ChoiceParam*	_paramFilter;

	OFX::DoubleParam*	_paramFilterSize;

	OFX::BooleanParam*	_paramCenter;
	OFX::Double2DParam*	_paramCenterPoint;

	OFX::DoubleParam*	_paramB;
	OFX::DoubleParam*	_paramC;

        OFX::ChoiceParam*	_paramOutOfImage;
};

}
}
}

#endif
