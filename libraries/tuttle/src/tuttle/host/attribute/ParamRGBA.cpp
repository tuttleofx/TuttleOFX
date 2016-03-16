#include "ParamRGBA.hpp"

#include <tuttle/host/INode.hpp>

namespace tuttle
{
namespace host
{
namespace attribute
{

ParamRGBA::ParamRGBA(INode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor)
    : Param(effect)
    , ParamDoubleMultiDim<4>(descriptor, name, effect.getParamSet())
{
    _controls.replace<0>(new ParamDouble(effect, name + ".r", descriptor, 0));
    _controls.replace<1>(new ParamDouble(effect, name + ".g", descriptor, 1));
    _controls.replace<2>(new ParamDouble(effect, name + ".b", descriptor, 2));
    _controls.replace<3>(new ParamDouble(effect, name + ".a", descriptor, 3));
}

OfxRGBAColourD ParamRGBA::getDefault() const
{
    OfxRGBAColourD rgb;

    rgb.r = _controls.at<0>().getDefault();
    rgb.g = _controls.at<1>().getDefault();
    rgb.b = _controls.at<2>().getDefault();
    rgb.a = _controls.at<3>().getDefault();
    return rgb;
}

void ParamRGBA::getValue(double& r, double& g, double& b, double& a) const OFX_EXCEPTION_SPEC
{
    _controls.at<0>().getValue(r);
    _controls.at<1>().getValue(g);
    _controls.at<2>().getValue(b);
    _controls.at<3>().getValue(a);
}

void ParamRGBA::getValueAtTime(const OfxTime time, double& r, double& g, double& b, double& a) const OFX_EXCEPTION_SPEC
{
    _controls.at<0>().getValueAtTime(time, r);
    _controls.at<1>().getValueAtTime(time, g);
    _controls.at<2>().getValueAtTime(time, b);
    _controls.at<3>().getValueAtTime(time, a);
}

void ParamRGBA::setValue(const double& r, const double& g, const double& b, const double& a,
                         const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
{
    _controls.at<0>().setValue(r, change);
    _controls.at<1>().setValue(g, change);
    _controls.at<2>().setValue(b, change);
    _controls.at<3>().setValue(a, change);
    this->paramChanged(change);
}

void ParamRGBA::setValueAtTime(const OfxTime time, const double& r, const double& g, const double& b, const double& a,
                               const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
{
    _controls.at<0>().setValueAtTime(time, r, change);
    _controls.at<1>().setValueAtTime(time, g, change);
    _controls.at<2>().setValueAtTime(time, b, change);
    _controls.at<3>().setValueAtTime(time, a, change);
    this->paramChanged(change);
}
}
}
}
