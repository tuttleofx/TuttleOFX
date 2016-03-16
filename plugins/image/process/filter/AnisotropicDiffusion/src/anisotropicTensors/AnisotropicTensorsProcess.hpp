#ifndef _TUTTLE_PLUGIN_TENSORS_PROCESS_HPP_
#define _TUTTLE_PLUGIN_TENSORS_PROCESS_HPP_

#include "AnisotropicTensorsPluginFactory.hpp"
#include "AnisotropicTensorsPlugin.hpp"
#include <imageUtils/ImageTensors.hpp>

#include <terry/globals.hpp>
#include <tuttle/common/math/rectOp.hpp>
#include <tuttle/common/utils/global.hpp>
#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <tuttle/plugin/IProgress.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle
{
namespace plugin
{
namespace anisotropicFilter
{
namespace tensors
{

/**
 * @brief Base class for the generic processor
 */
template <class View>
class AnisotropicTensorsProcess : public ImageGilFilterProcessor<View>
{
public:
    typedef typename View::value_type Pixel;

protected:
    TensorsPlugin& _plugin; ///< Tensor rendering plugin
    OfxRectI _upScaledSrcBounds, _dBounds;
    OFX::BooleanParam* _algorithm; ///< Generation algorithm
    OFX::ChoiceParam* _stAlgo;     ///< Structure tensors algorithm
    OFX::DoubleParam* _alpha;      ///< Pre-blurring (noise scale)
    OFX::DoubleParam* _sigma;      ///< Post-blurring
    OFX::DoubleParam* _sharpness;  ///< Contour preservation
    OFX::DoubleParam* _anisotropy; ///< Anisotropic filtering
    OFX::DoubleParam* _geom_fact;  ///< Geometry factor
    OFX::DoubleParam* _threshold;  ///< Thresholding quantization factor

public:
    AnisotropicTensorsProcess(TensorsPlugin& instance);

    void setup(const OFX::RenderArguments& args);

    void multiThreadProcessImages(const OfxRectI& procWindowRoW);
};
}
}
}
}

#include "AnisotropicTensorsProcess.tcc"

#endif
