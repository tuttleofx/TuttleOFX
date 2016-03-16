#include "ParamRGB.hpp"

#include <tuttle/host/INode.hpp>

namespace tuttle
{
namespace host
{
namespace attribute
{

ParamRGB::ParamRGB(INode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor)
    : Param(effect)
    , ParamDoubleMultiDim<3>(descriptor, name, effect.getParamSet())
{
    _controls.replace<0>(new ParamDouble(effect, name + ".r", descriptor, 0));
    _controls.replace<1>(new ParamDouble(effect, name + ".g", descriptor, 1));
    _controls.replace<2>(new ParamDouble(effect, name + ".b", descriptor, 2));
}

OfxRGBColourD ParamRGB::getDefault() const
{
    OfxRGBColourD rgb;

    rgb.r = _controls.at<0>().getDefault();
    rgb.g = _controls.at<1>().getDefault();
    rgb.b = _controls.at<2>().getDefault();
    return rgb;
}

void ParamRGB::getValue(double& r, double& g, double& b) const OFX_EXCEPTION_SPEC
{
    _controls.at<0>().getValue(r);
    _controls.at<1>().getValue(g);
    _controls.at<2>().getValue(b);
}

void ParamRGB::getValueAtTime(const OfxTime time, double& r, double& g, double& b) const OFX_EXCEPTION_SPEC
{
    _controls.at<0>().getValueAtTime(time, r);
    _controls.at<1>().getValueAtTime(time, g);
    _controls.at<2>().getValueAtTime(time, b);
}

void ParamRGB::setValue(const double& r, const double& g, const double& b,
                        const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
{
    _controls.at<0>().setValue(r, change);
    _controls.at<1>().setValue(g, change);
    _controls.at<2>().setValue(b, change);
    this->paramChanged(change);
}

void ParamRGB::setValueAtTime(const OfxTime time, const double& r, const double& g, const double& b,
                              const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
{
    _controls.at<0>().setValueAtTime(time, r, change);
    _controls.at<1>().setValueAtTime(time, g, change);
    _controls.at<2>().setValueAtTime(time, b, change);
    this->paramChanged(change);
}
}
}
}
