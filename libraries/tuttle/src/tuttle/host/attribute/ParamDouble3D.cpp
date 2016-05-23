#include "ParamDouble3D.hpp"

#include <tuttle/host/INode.hpp>

namespace tuttle
{
namespace host
{
namespace attribute
{

ParamDouble3D::ParamDouble3D(INode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor)
    : Param(effect)
    , ParamDoubleMultiDim<3>(descriptor, name, effect.getParamSet())
{
    _controls.replace<0>(new ParamDouble(effect, name + ".x", descriptor, 0));
    _controls.replace<1>(new ParamDouble(effect, name + ".y", descriptor, 1));
    _controls.replace<2>(new ParamDouble(effect, name + ".z", descriptor, 2));
}

Ofx3DPointD ParamDouble3D::getDefault() const
{
    Ofx3DPointD point;

    point.x = _controls.at<0>().getDefault();
    point.y = _controls.at<1>().getDefault();
    point.z = _controls.at<2>().getDefault();
    return point;
}

void ParamDouble3D::getValue(double& x, double& y, double& z) const OFX_EXCEPTION_SPEC
{
    _controls.at<0>().getValue(x);
    _controls.at<1>().getValue(y);
    _controls.at<2>().getValue(z);
}

void ParamDouble3D::getValueAtTime(const OfxTime time, double& x, double& y, double& z) const OFX_EXCEPTION_SPEC
{
    _controls.at<0>().getValueAtTime(time, x);
    _controls.at<1>().getValueAtTime(time, y);
    _controls.at<2>().getValueAtTime(time, z);
}

void ParamDouble3D::setValue(const double& x, const double& y, const double& z,
                             const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
{
    _controls.at<0>().setValue(x, change);
    _controls.at<1>().setValue(y, change);
    _controls.at<2>().setValue(z, change);
    this->paramChanged(change);
}

void ParamDouble3D::setValueAtTime(const OfxTime time, const double& x, const double& y, const double& z,
                                   const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
{
    _controls.at<0>().setValueAtTime(time, x, change);
    _controls.at<1>().setValueAtTime(time, y, change);
    _controls.at<2>().setValueAtTime(time, z, change);
    this->paramChanged(change);
}
}
}
}
