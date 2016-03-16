#include "IdKeyerPlugin.hpp"
#include "IdKeyerProcess.hpp"
#include "IdKeyerAlgorithm.hpp"
#include "IdKeyerDefinitions.hpp"

#include <tuttle/plugin/global.hpp>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>
#include <boost/algorithm/string/predicate.hpp>

namespace tuttle
{
namespace plugin
{
namespace idKeyer
{

using namespace terry;

IdKeyerPlugin::IdKeyerPlugin(OfxImageEffectHandle handle)
    : ImageEffectGilPlugin(handle)
{
    _paramNbPoints = fetchIntParam(kParamNbPoints);
    _paramUseAlpha = fetchBooleanParam(kParamUseAlpha);
    _paramTolerance = fetchDoubleParam(kParamTolerance);

    _paramColors.reserve(kMaxNbPoints);
    for(size_t i = 0; i < kMaxNbPoints; ++i)
    {
        _paramColors.push_back(fetchRGBAParam(getColorParamName(i)));
    }

    changedParam(_instanceChangedArgs, kParamNbPoints);
}

template <class View>
IdKeyerProcessParams<View> IdKeyerPlugin::getProcessParams() const
{
    IdKeyerProcessParams<View> params;

    params._useAlpha = _paramUseAlpha->getValue();
    params._tolerance = _paramTolerance->getValue();

    RGBAParamVector::const_iterator it_color = _paramColors.begin();

    size_t nbPoints = boost::numeric_cast<size_t>(_paramNbPoints->getValue());

    for(size_t i = 0; i < nbPoints; ++i, ++it_color)
    {
        OfxRGBAColourD c = (*it_color)->getValue();
        params._colors.push_back(rgba32f_pixel_t(c.r, c.g, c.b, c.a));

        // TUTTLE_LOG_WARNING( "key color " <<  c.r << ", " <<  c.g << ", " <<  c.b << ", " << c.a );
    }
    return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void IdKeyerPlugin::render(const OFX::RenderArguments& args)
{
    // instantiate the render code based on the pixel depth of the dst clip
    OFX::EBitDepth bitDepth = _clipDst->getPixelDepth();
    OFX::EPixelComponent components = _clipDst->getPixelComponents();

    switch(components)
    {
        case OFX::ePixelComponentRGBA:
        {
            doGilRender<IdKeyerProcess, false, boost::gil::rgba_layout_t>(*this, args, bitDepth);
            return;
        }
        case OFX::ePixelComponentRGB:
        case OFX::ePixelComponentAlpha:
        case OFX::ePixelComponentCustom:
        case OFX::ePixelComponentNone:
        {
            BOOST_THROW_EXCEPTION(exception::Unsupported() << exception::user() + "Pixel components (" +
                                                                  mapPixelComponentEnumToString(components) +
                                                                  ") not supported by the plugin.");
        }
    }
    BOOST_THROW_EXCEPTION(exception::Unknown());
}

void IdKeyerPlugin::changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName)
{
    if(paramName == kParamNbPoints)
    {
        const size_t nbPoints = boost::numeric_cast<size_t>(_paramNbPoints->getValue());
        RGBAParamVector::iterator it_color = _paramColors.begin();
        for(size_t i = 0; i < kMaxNbPoints; ++i, ++it_color)
        {
            (*it_color)->setIsSecret(i < nbPoints ? false : true);
        }
    }
}
}
}
}
