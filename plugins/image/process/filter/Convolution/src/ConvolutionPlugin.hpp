#ifndef _TUTTLE_PLUGIN_CONVOLUTION_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_CONVOLUTION_PLUGIN_HPP_

#include "ConvolutionDefinitions.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

#include <terry/filter/convolve.hpp>

//#include <boost/numeric/ublas/matrix.hpp>

#include <vector>

namespace tuttle
{
namespace plugin
{
namespace convolution
{

struct ConvolutionProcessParams
{
    //	boost::numeric::ublas::matrix<double> _convMatrix;
    typedef float Scalar;
    typedef terry::filter::kernel_1d<Scalar> Kernel;

    boost::gil::point2<unsigned int> _size;

    EParamBorder _border;
    terry::filter::convolve_boundary_option _boundary_option;

    Kernel _convX;
    Kernel _convY;
};

/**
 * @brief Convolution plugin
 */
class ConvolutionPlugin : public ImageEffectGilPlugin
{
public:
    ConvolutionPlugin(OfxImageEffectHandle handle);

public:
    void render(const OFX::RenderArguments& args);

    bool isIdentity(const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime);
    void getRegionsOfInterest(const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois);
    bool getRegionOfDefinition(const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod);

    void changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName);

    ConvolutionProcessParams getProcessParams() const;

public:
    OFX::Int2DParam* _paramSize;
    OFX::ChoiceParam* _paramBorder;
    std::vector<std::vector<OFX::DoubleParam*> > _paramCoef;
};
}
}
}

#endif
