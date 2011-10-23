#ifndef _TUTTLE_PLUGIN_PINNING_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_PINNING_PLUGIN_HPP_

#include "PinningDefinitions.hpp"

#include <tuttle/plugin/global.hpp>

#include <ofxsImageEffect.h>

#include <terry/sampler/sampler.hpp>

#include <boost/gil/gil_all.hpp>

#include <boost/numeric/ublas/matrix.hpp>


namespace tuttle {
namespace plugin {
namespace pinning {

template<typename Scalar>
struct Perspective
{
	double _width, _height;
	boost::numeric::ublas::bounded_matrix<Scalar,3,3> _matrix;
};

template<typename Scalar>
struct Bilinear
{
	double _width, _height;
	boost::numeric::ublas::bounded_matrix<Scalar,2,4> _matrix;
};

template<typename Scalar>
struct PinningProcessParams
{
	Perspective<Scalar> _perspective;
	Bilinear<Scalar> _bilinear;

	EParamMethod _method;
	
	terry::sampler::EParamFilter _interpolation;
	double                                     _filterSize;
	double                                     _filterSigma;
	double                                     _paramB;
	double                                     _paramC;

	terry::sampler::EParamFilterOutOfImage     _outOfImageProcess;
};

/**
 * @brief Pinning plugin
 */
class PinningPlugin : public OFX::ImageEffect
{
public:
	typedef double Scalar;
	typedef boost::gil::point2<double> Point2;
	
public:
    PinningPlugin( OfxImageEffectHandle handle );

public:
	PinningProcessParams<Scalar> getProcessParams( const OfxTime time, const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

	void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );
	bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );
	void render( const OFX::RenderArguments &args );

public:
	// do not need to delete these, the ImageEffect is managing them for us
	OFX::Clip* _clipSrc; ///< Source image clip
	OFX::Clip* _clipDst; ///< Destination image clip

	OFX::ChoiceParam*  _paramMethod;
	OFX::ChoiceParam*  _paramInterpolation;
	OFX::PushButtonParam* _paramSetToCornersIn;
	OFX::PushButtonParam* _paramSetToCornersOut;
	OFX::BooleanParam*  _paramInverse;

//	OFX::ChoiceParam*  _paramManipulatorMode;
	OFX::BooleanParam*  _paramOverlay;


	OFX::ChoiceParam*	_paramFilter;

	OFX::IntParam*		_paramFilterSize;
	OFX::DoubleParam*	_paramFilterSigma;

	OFX::DoubleParam*	_paramB;
	OFX::DoubleParam*	_paramC;

	OFX::ChoiceParam*	_paramOutOfImage;

/*
	//TODO-vince //
	OFX::GroupParam* _paramGroupCentre;
	OFX::Double2DParam* _paramPointCentre;
	OFX::BooleanParam*  _paramOverlayCentre;
	OFX::RGBParam*  _paramOverlayCentreColor;
	///////////////////
*/

	OFX::GroupParam* _paramGroupIn;
	OFX::Double2DParam* _paramPointIn0;
	OFX::Double2DParam* _paramPointIn1;
	OFX::Double2DParam* _paramPointIn2;
	OFX::Double2DParam* _paramPointIn3;
	OFX::BooleanParam*  _paramOverlayIn;
	OFX::RGBParam*  _paramOverlayInColor;

	OFX::GroupParam* _paramGroupOut;
	OFX::Double2DParam* _paramPointOut0;
	OFX::Double2DParam* _paramPointOut1;
	OFX::Double2DParam* _paramPointOut2;
	OFX::Double2DParam* _paramPointOut3;
	OFX::BooleanParam*  _paramOverlayOut;
	OFX::RGBParam*  _paramOverlayOutColor;

	OFX::GroupParam* _paramGroupPerspMatrix;
	OFX::Double3DParam* _paramPerspMatrixRow0;
	OFX::Double3DParam* _paramPerspMatrixRow1;
	OFX::Double3DParam* _paramPerspMatrixRow2;

	OFX::GroupParam* _paramGroupBilMatrix;
	OFX::Double2DParam* _paramBilMatrixRow0;
	OFX::Double2DParam* _paramBilMatrixRow1;
	OFX::Double2DParam* _paramBilMatrixRow2;
	OFX::Double2DParam* _paramBilMatrixRow3;
};

}
}
}

#endif
