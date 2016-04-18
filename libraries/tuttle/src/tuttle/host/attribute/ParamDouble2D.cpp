#include "ParamDouble2D.hpp"

#include <tuttle/host/INode.hpp>

namespace tuttle
{
namespace host
{
namespace attribute
{

ParamDouble2D::ParamDouble2D(INode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor)
    : Param(effect)
    , ParamDoubleMultiDim<2>(descriptor, name, effect.getParamSet())
{
    _controls.replace<0>(new ParamDouble(effect, name + ".x", descriptor, 0));
    _controls.replace<1>(new ParamDouble(effect, name + ".y", descriptor, 1));
}

OfxPointD ParamDouble2D::getDefault() const
{
    OfxPointD point;

    point.x = _controls.at<0>().getDefault();
    point.y = _controls.at<1>().getDefault();
    return point;
}

void ParamDouble2D::getValue(double& x, double& y) const OFX_EXCEPTION_SPEC
{
    _controls.at<0>().getValue(x);
    _controls.at<1>().getValue(y);
}

void ParamDouble2D::getValueAtTime(const OfxTime time, double& x, double& y) const OFX_EXCEPTION_SPEC
{
    _controls.at<0>().getValueAtTime(time, x);
    _controls.at<1>().getValueAtTime(time, y);
}

void ParamDouble2D::setValue(const double& x, const double& y, const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
{
    _controls.at<0>().setValue(x, change);
    _controls.at<1>().setValue(y, change);
    this->paramChanged(change);
}

void ParamDouble2D::setValueAtTime(const OfxTime time, const double& x, const double& y,
                                   const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
{
    _controls.at<0>().setValueAtTime(time, x, change);
    _controls.at<1>().setValueAtTime(time, y, change);
    this->paramChanged(change);
}
}
}
}
