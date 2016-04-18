#include "ParamInteger3D.hpp"

#include <tuttle/host/INode.hpp>

namespace tuttle
{
namespace host
{
namespace attribute
{

ParamInteger3D::ParamInteger3D(INode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor)
    : Param(effect)
    , ofx::attribute::OfxhMultiDimParam<ParamInteger, 3>(descriptor, name, effect.getParamSet())
{
    _controls.replace<0>(new ParamInteger(effect, name + ".x", descriptor, 0));
    _controls.replace<1>(new ParamInteger(effect, name + ".y", descriptor, 1));
    _controls.replace<2>(new ParamInteger(effect, name + ".z", descriptor, 2));
}

Ofx3DPointI ParamInteger3D::getDefault() const
{
    Ofx3DPointI point;

    point.x = _controls.at<0>().getDefault();
    point.y = _controls.at<1>().getDefault();
    point.z = _controls.at<2>().getDefault();
    return point;
}

void ParamInteger3D::getValue(int& x, int& y, int& z) const OFX_EXCEPTION_SPEC
{
    _controls.at<0>().getValue(x);
    _controls.at<1>().getValue(y);
    _controls.at<2>().getValue(z);
}

void ParamInteger3D::getValueAtTime(const OfxTime time, int& x, int& y, int& z) const OFX_EXCEPTION_SPEC
{
    _controls.at<0>().getValueAtTime(time, x);
    _controls.at<1>().getValueAtTime(time, y);
    _controls.at<2>().getValueAtTime(time, z);
}

void ParamInteger3D::setValue(const int& x, const int& y, const int& z,
                              const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
{
    _controls.at<0>().setValue(x, change);
    _controls.at<1>().setValue(y, change);
    _controls.at<2>().setValue(z, change);
    this->paramChanged(change);
}

void ParamInteger3D::setValueAtTime(const OfxTime time, const int& x, const int& y, const int& z,
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
