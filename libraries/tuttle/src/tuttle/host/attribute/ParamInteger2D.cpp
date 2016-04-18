#include "ParamInteger2D.hpp"

#include <tuttle/host/INode.hpp>

namespace tuttle
{
namespace host
{
namespace attribute
{

ParamInteger2D::ParamInteger2D(INode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor)
    : Param(effect)
    , ofx::attribute::OfxhMultiDimParam<ParamInteger, 2>(descriptor, name, effect.getParamSet())
{
    _controls.replace<0>(new ParamInteger(effect, name + ".x", descriptor, 0));
    _controls.replace<1>(new ParamInteger(effect, name + ".y", descriptor, 1));
}

OfxPointI ParamInteger2D::getDefault() const
{
    OfxPointI point;

    point.x = _controls.at<0>().getDefault();
    point.y = _controls.at<1>().getDefault();
    return point;
}

void ParamInteger2D::getValue(int& x, int& y) const OFX_EXCEPTION_SPEC
{
    _controls.at<0>().getValue(x);
    _controls.at<1>().getValue(y);
}

void ParamInteger2D::getValueAtTime(const OfxTime time, int& x, int& y) const OFX_EXCEPTION_SPEC
{
    _controls.at<0>().getValueAtTime(time, x);
    _controls.at<1>().getValueAtTime(time, y);
}

void ParamInteger2D::setValue(const int& x, const int& y, const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
{
    _controls.at<0>().setValue(x, change);
    _controls.at<1>().setValue(y, change);
    this->paramChanged(change);
}

void ParamInteger2D::setValueAtTime(const OfxTime time, const int& x, const int& y,
                                    const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
{
    _controls.at<0>().setValueAtTime(time, x, change);
    _controls.at<1>().setValueAtTime(time, y, change);
    this->paramChanged(change);
}
}
}
}
