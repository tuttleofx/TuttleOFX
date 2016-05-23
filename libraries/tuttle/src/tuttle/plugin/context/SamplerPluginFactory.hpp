#ifndef _TUTTLE_PLUGIN_SAMPLERPLUGINFACTORY_HPP_
#define _TUTTLE_PLUGIN_SAMPLERPLUGINFACTORY_HPP_

#include "SamplerDefinition.hpp"

#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

namespace tuttle
{
namespace plugin
{

void describeSamplerParamsInContext(OFX::ImageEffectDescriptor& desc, OFX::EContext context)
{
    OFX::ChoiceParamDescriptor* filter = desc.defineChoiceParam(kParamFilter);
    filter->setLabel(kParamFilterLabel);

    filter->appendOption(kParamFilterNearest);
    filter->appendOption(kParamFilterBilinear);
    filter->appendOption(kParamFilterBC);
    filter->appendOption(kParamFilterBicubic);
    filter->appendOption(kParamFilterCatrom);
    filter->appendOption(kParamFilterKeys);
    filter->appendOption(kParamFilterSimon);
    filter->appendOption(kParamFilterRifman);
    filter->appendOption(kParamFilterMitchell);
    filter->appendOption(kParamFilterParzen);
    filter->appendOption(kParamFilterLanczos);
    filter->appendOption(kParamFilterLanczos3);
    filter->appendOption(kParamFilterLanczos4);
    filter->appendOption(kParamFilterLanczos6);
    filter->appendOption(kParamFilterLanczos12);
#if(TUTTLE_EXPERIMENTAL)
    filter->appendOption(kParamFilterGaussian);
#endif

    filter->setDefault(terry::sampler::eParamFilterBicubic);
    filter->setHint("Interpolation methods\n"
                    "\n"
                    "nearest: Nearest Neighbor sampler\n"
                    "bilinear: Bilinear sample\n"
                    "bc: 2nd order sampler with B-C parametrable\n"
                    "bicubic: Cubic filter(0.0, 0.0)\n"
                    "catmul-rom: Cubic filter(0.0, 0.5)\n"
                    "keys: Cubic filter(0.0, 0.5)\n"
                    "simon: Cubic filter(0.0, 0.75)\n"
                    "rifman: Cubic filter(0.0, 1.0)\n"
                    "mitchell: Cubic filter(1/3, 1/3)\n"
                    "parzen: Cubic filter(1.0, 0.0)\n"
                    "lanczos: Lanczos sampler with parametrable filter size\n"
                    "lanczos3: Lanczos sampler with filter size = 3, sharpen = 2 \n"
                    "lanczos4: Lanczos sampler with filter size = 4, sharpen = 2\n"
                    "lanczos6: Lanczos sampler with filter size = 6, sharpen = 2\n"
                    "lanczos12: Lanczos sampler with filter size = 12, sharpen = 2 \n"
#if(TUTTLE_EXPERIMENTAL)
                    "gaussian: Gaussian sampler with parametrable filter size and sigma value\n"
#endif
                    );

    OFX::DoubleParamDescriptor* B = desc.defineDoubleParam(kParamFilterB);
    B->setLabel("B value");
    B->setDefault(1.0);
    B->setRange(0.0, 1.0);
    B->setDisplayRange(0.0, 1.0);
    B->setHint("Adjust the B valuye of the cubic filter.");

    OFX::DoubleParamDescriptor* C = desc.defineDoubleParam(kParamFilterC);
    C->setLabel("C value");
    C->setDefault(0.0);
    C->setRange(0.0, 1.0);
    C->setDisplayRange(0.0, 1.0);
    C->setHint("Adjust the C valuye of the cubic filter.");

    OFX::IntParamDescriptor* filterSize = desc.defineIntParam(kParamFilterSize);
    filterSize->setLabel("Filter size");
    filterSize->setDefault(3.0);
    filterSize->setRange(1.0, 30.0);
    filterSize->setDisplayRange(1.0, 30.0);
    filterSize->setHint("Set the filter size.");

    OFX::DoubleParamDescriptor* filterSigma = desc.defineDoubleParam(kParamFilterSigma);
    filterSigma->setLabel("Sigma");
    filterSigma->setDefault(1.0);
    filterSigma->setRange(0.0001, 30.0);
    filterSigma->setDisplayRange(0.0001, 4.0);
    filterSigma->setHint("Set the gaussian sigma coefficient.");

    OFX::DoubleParamDescriptor* filterSharpen = desc.defineDoubleParam(kParamFilterSharpen);
    filterSharpen->setLabel("Sharpen");
    filterSharpen->setDefault(2.0);
    filterSharpen->setRange(0.0001, 30.0);
    filterSharpen->setDisplayRange(0.0001, 4.0);
    filterSharpen->setHint("Set the lanczos detail coefficient.");

    OFX::ChoiceParamDescriptor* outOfImage = desc.defineChoiceParam(kParamFilterOutOfImage);
    outOfImage->setLabel("Out of Image");
    outOfImage->appendOption(kParamFilterOutBlack);
    outOfImage->appendOption(kParamFilterOutTransparency);
    outOfImage->appendOption(kParamFilterOutCopy);
#if(TUTTLE_EXPERIMENTAL)
    outOfImage->appendOption(kParamFilterOutMirror);
#endif
    outOfImage->setHint("Select the filtering method out of the image.");
    outOfImage->setDefault(terry::sampler::eParamFilterOutCopy);
}
}
}
#endif
