#include "ColorTransformPlugin.hpp"
#include "ColorTransformProcess.hpp"
#include "ColorTransformAlgorithm.hpp"
#include "ColorTransformDefinitions.hpp"

#include <boost/gil/gil_all.hpp>
#include <boost/numeric/ublas/traits.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorTransform
{

ColorTransformPlugin::ColorTransformPlugin(OfxImageEffectHandle handle)
    : ImageEffectGilPlugin(handle)
{
    _paramHueShift = fetchDoubleParam(kParamHueShift);
    _paramSaturation = fetchDoubleParam(kParamSaturation);

    _paramContrastRGB = fetchDoubleParam(kParamContrastRGB);
    _paramContrast = fetchRGBAParam(kParamContrast);
    _paramBrightnessRGB = fetchDoubleParam(kParamBrightnessRGB);
    _paramBrightness = fetchRGBAParam(kParamBrightness);
    _paramOffsetRGB = fetchDoubleParam(kParamOffsetRGB);
    _paramOffset = fetchRGBAParam(kParamOffset);
    _paramPivotRGB = fetchDoubleParam(kParamPivotRGB);
    _paramPivot = fetchRGBAParam(kParamPivot);
}

ColorTransformParams ColorTransformPlugin::getProcessParams(const OfxPointD& renderScale) const
{
    ColorTransformParams params;

    params._hueShift = _paramHueShift->getValue();
    params._saturation = _paramSaturation->getValue();

    params._contrastRGB = _paramContrastRGB->getValue();
    params._contrast = _paramContrast->getValue();
    params._brightnessRGB = _paramBrightnessRGB->getValue();
    params._brightness = _paramBrightness->getValue();
    params._offsetRGB = _paramOffsetRGB->getValue();
    params._offset = _paramOffset->getValue();
    params._pivotRGB = _paramPivotRGB->getValue();
    params._pivot = _paramPivot->getValue();

    return params;
}

void ColorTransformPlugin::changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName)
{
    //    if( paramName == kParamHelpButton )
    //    {
    //        sendMessage( OFX::Message::eMessageMessage,
    //                     "", // No XML resources
    //                     kParamHelpString );
    //    }
}

bool ColorTransformPlugin::isIdentity(const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime)
{
    using namespace boost::numeric::ublas;
    ColorTransformParams params = getProcessParams();

    typedef terry::math::BoundedMatrix<Scalar, 5, 5>::Type BoundedMatrix5x5;
    BoundedMatrix5x5 matrix = colorTransformMatrix<BoundedMatrix5x5>(params);

    //	if( equals( matrix, identity_matrix<Scalar>(5) ) )
    //	{
    //		identityClip = _clipSrc;
    //		identityTime = args.time;
    //		return true;
    //	}
    return false;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void ColorTransformPlugin::render(const OFX::RenderArguments& args)
{
    doGilRender<ColorTransformProcess>(*this, args);
    /*
            // instantiate the render code based on the pixel depth of the dst clip
            OFX::EBitDepth bitDepth         = _clipDst->getPixelDepth();
            OFX::EPixelComponent components = _clipDst->getPixelComponents();

            switch( components )
            {
                    case OFX::ePixelComponentRGBA:
                    {
                            doGilRender<ColorTransformProcess, false, boost::gil::rgba_layout_t>( *this, args, bitDepth );
                            return;
                    }
                    case OFX::ePixelComponentRGB:
                    {
                            doGilRender<ColorTransformProcess, false, boost::gil::rgb_layout_t>( *this, args, bitDepth );
                            return;
                    }
                    case OFX::ePixelComponentAlpha:
                    case OFX::ePixelComponentCustom:
                    case OFX::ePixelComponentNone:
                    {
                            BOOST_THROW_EXCEPTION( exception::Unsupported()
                                    << exception::user() + "Pixel components (" + mapPixelComponentEnumToString(components) +
       ") not supported by the plugin." );
                    }
            }
            BOOST_THROW_EXCEPTION( exception::Unknown() );
    */
}
}
}
}
