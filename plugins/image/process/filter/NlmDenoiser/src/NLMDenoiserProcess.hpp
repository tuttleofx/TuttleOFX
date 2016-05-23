#ifndef _TUTTLE_PLUGIN_NLMDENOISERPROCESS_HPP_
#define _TUTTLE_PLUGIN_NLMDENOISERPROCESS_HPP_

#include "NLMDenoiserPlugin.hpp"

#include <tuttle/common/utils/global.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>
#include <terry/globals.hpp>

#include <cmath>
#include <vector>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>

#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/array.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/scoped_array.hpp>

namespace tuttle
{
namespace plugin
{
namespace nlmDenoiser
{

struct NlmParams
{
    boost::array<float, 4> mix;
    boost::array<float, 4> bws;
    int patchRadius;
    int regionRadius;
    double preBlurring;
};

/**
 * @brief Base class for the denoising processor
 */
template <class View>
class NLMDenoiserProcess : public ImageGilProcessor<View>
{
public:
    typedef typename View::value_type Pixel;
    typedef typename boost::gil::channel_type<View>::type Channel;

protected:
    OFX::BooleanParam* _paramOptimized;     ///< Perform optimization (quality++, speed+++)
    OFX::DoubleParam* _paramPreBlurring;    ///< Perform pre blurring
    OFX::IntParam* _paramPatchRadius;       ///< Patch size for nl-means algorithm
    OFX::IntParam* _paramRegionRadius;      ///< Region radius size
    OFX::IntParam* _paramDepth;             ///< depth for nl-means algorithm
    OFX::DoubleParam* _paramRedStrength;    ///< Red color effect mix
    OFX::DoubleParam* _paramGreenStrength;  ///< Green color effect mix
    OFX::DoubleParam* _paramBlueStrength;   ///< Blue color effect mix
    OFX::DoubleParam* _paramRedGrainSize;   ///< Red color effect bandwidth
    OFX::DoubleParam* _paramGreenGrainSize; ///< Green color effect bandwidth
    OFX::DoubleParam* _paramBlueGrainSize;  ///< Blue color effect bandwidth

    std::vector<View> _srcViews; ///< Array of source image view (3D-NLMeans)
    boost::ptr_vector<OFX::Image> _srcImgs;

    NLMDenoiserPlugin& _plugin; ///< Rendering plugin

    int _margin;              ///< Margin
    OfxRectI _upScaledBounds; ///< Upscaled source bounds (margin upscaling)

protected:
    void addFrame(const OfxRectI& dBounds, const int dstBitDepth, const int dstComponents, const double time, const int z);

public:
    NLMDenoiserProcess(NLMDenoiserPlugin& instance);
    ~NLMDenoiserProcess();

    void setup(const OFX::RenderArguments& args);
    void preProcess();
    void multiThreadProcessImages(const OfxRectI& procWindowRoW);

    double computeBandwidth();
    void nlMeans(View& dst, const OfxRectI& procWindow, const NlmParams& params);

    void computeWeights(const std::vector<View>& srcViews, const OfxRectI& procWindow, boost::gil::rgba32f_view_t& view_wc,
                        boost::gil::rgba32f_view_t& view_norm, const NlmParams& params);
};
}
}
}

#include "NLMDenoiserProcess.tcc"

#endif
