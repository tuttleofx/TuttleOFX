#ifndef LENSDISTORTPROCESS_HPP
#define LENSDISTORTPROCESS_HPP

#include <tuttle/plugin/global.hpp>
#include "lensDistortAlgorithm.hpp"
#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>
#include <tuttle/plugin/image/gil/globals.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle {
namespace plugin {
namespace lens {

namespace bgil = boost::gil;

/**
 * @brief Plugin rendering
 */
template<class View>
class LensDistortProcess : public ImageGilFilterProcessor<View>
{
typedef double Scalar;     // calculations are in double

protected:
	LensDistortPlugin& _plugin;
	LensDistortProcessParams<Scalar> _p;
	EParamLensType _lensType;
	EParamCenterType _centerType;
	EParamInterpolation _interpolation;

public:
	LensDistortProcess( LensDistortPlugin& instance );

	void setup( const OFX::RenderArguments& args );

	void multiThreadProcessImages( const OfxRectI& procWindowRoW );

private:
	template<class Sampler>
	void lensDistort( View& srcView, View& dstView, const OfxRectI& procWindow );
};

}
}
}

#include "LensDistortProcess.tcc"

#endif
