#ifndef _PDE_DENOISER_PROCESS_HPP_
#define _PDE_DENOISER_PROCESS_HPP_

#include "../imageUtils/ImageTensors.hpp"
#include <tuttle/plugin/ImageGilFilterProcessor.hpp>

#include <tuttle/common/utils/global.hpp>
#include <tuttle/common/math/rectOp.hpp>
#include <terry/globals.hpp>
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
namespace diffusion
{

using namespace imageUtils;

struct region_t
{
    unsigned int dox, doy;
    unsigned int dw, dh;
};

/**
 * @brief Base class for the denoising processor
 *
 */
template <class View>
class AnisotropicDiffusionProcess : public ImageGilFilterProcessor<View>
{
    typedef typename View::value_type Pixel;

protected:
    AnisotropicDiffusionPlugin& _plugin; ///< Rendering plugin
    boost::scoped_ptr<OFX::Image> _srcTensor;
    OFX::BooleanParam* _fast_approx; ///< Perform fast approximation
    OFX::RGBParam* _amplitude;       ///< Red amplitude control parameter
    View _srcView;                   ///< Source image view
    View _srcTensorView;             ///< Source tensors image view
    OfxRectI _upScaledSrcBounds, _dBounds;

public:
    AnisotropicDiffusionProcess<View>(AnisotropicDiffusionPlugin& instance);

    void setup(const OFX::RenderArguments& args);

    void multiThreadProcessImages(const OfxRectI& procWindowRoW);

    // Blur anisotopric
    void blur_anisotropic(View& dst, View& src, View& G, const region_t& dregion, const OfxRGBColourD& amplitude,
                          const bool fast_approx = true, const float dl = 0.8f, const float da = 30.0f,
                          const float gauss_prec = 2.0f);
};
}
}
}
}

#include "AnisotropicDiffusionProcess.tcc"

#endif
