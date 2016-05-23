#include "ComponentPlugin.hpp"
#include "ComponentProcess.hpp"
#include "ComponentDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle
{
namespace plugin
{
namespace component
{

ComponentPlugin::ComponentPlugin(OfxImageEffectHandle handle)
    : ImageEffectGilPlugin(handle)
{
    //    _clipSrcMatte = fetchClip( kClipMatte );
    _paramToComponent = fetchChoiceParam(kParamTo);
    _paramToGrayMethod = fetchChoiceParam(kParamToGray);
    _paramPremultiplied = fetchBooleanParam(kParamPremutliplied);
}

void ComponentPlugin::changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName)
{
}

// bool ComponentPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
//{
//	ComponentProcessParams<Scalar> params = getProcessParams();
//	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
//
//	switch( params._border )
//	{
//		case eParamBorderPadded:
//			rod.x1 = srcRod.x1 + 1;
//			rod.y1 = srcRod.y1 + 1;
//			rod.x2 = srcRod.x2 - 1;
//			rod.y2 = srcRod.y2 - 1;
//			return true;
//		default:
//			break;
//	}
//	return false;
//}
//
// void ComponentPlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois
// )
//{
//	ComponentProcessParams<Scalar> params = getProcessParams();
//	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
//
//	OfxRectD srcRoi;
//	srcRoi.x1 = srcRod.x1 - 1;
//	srcRoi.y1 = srcRod.y1 - 1;
//	srcRoi.x2 = srcRod.x2 + 1;
//	srcRoi.y2 = srcRod.y2 + 1;
//	rois.setRegionOfInterest( *_clipSrc, srcRoi );
//}

bool ComponentPlugin::isIdentity(const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime)
{
    //	ComponentProcessParams<Scalar> params = getProcessParams();
    //	if( params._in == params._out )
    //	{
    //		identityClip = _clipSrc;
    //		identityTime = args.time;
    //		return true;
    //	}
    return false;
}

void ComponentPlugin::getClipPreferences(OFX::ClipPreferencesSetter& clipPreferences)
{
    switch(_paramToComponent->getValue())
    {
        case eConvertToGray:
            clipPreferences.setClipComponents(*this->_clipDst, OFX::ePixelComponentAlpha);
            break;
        case eConvertToRGB:
            clipPreferences.setClipComponents(*this->_clipDst, OFX::ePixelComponentRGB);
            break;
        case eConvertToRGBA:
            clipPreferences.setClipComponents(*this->_clipDst, OFX::ePixelComponentRGBA);
            break;
    }
}
/*
template< typename dstComponents >
void ComponentPlugin::renderWithDstComponent( const OFX::RenderArguments &args, OFX::EPixelComponent srcComponents,
OFX::EBitDepth bitDepth )
{
        using namespace boost::gil;
        switch( srcComponents )
        {
                case OFX::ePixelComponentRGBA:
                {
                        doGilRender<ComponentProcess, false, rgba_layout_t>( *this, args, bitDepth );
                        return;
                }
                case OFX::ePixelComponentRGB:
                {
                        doGilRender<ComponentProcess, false, rgb_layout_t>( *this, args, bitDepth );
                        return;
                }
                case OFX::ePixelComponentAlpha:
                {
                        doGilRender<ComponentProcess, false, gray_layout_t>( *this, args, bitDepth );
                        return;
                }
                case OFX::ePixelComponentCustom:
                case OFX::ePixelComponentNone:
                {
                        BOOST_THROW_EXCEPTION( exception::Unsupported()
                                << exception::user() + "Pixel components (" + mapPixelComponentEnumToString(srcComponents) +
") not supported by the plugin." );
                }
        }
}*/

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void ComponentPlugin::render(const OFX::RenderArguments& args)
{
    // doGilRender<ComponentProcess>( *this, args );
    // instantiate the render code based on the pixel depth of the dst clip
    /*OFX::EBitDepth bitDepth            = _clipDst->getPixelDepth();
    OFX::EPixelComponent dstComponents = _clipDst->getPixelComponents();
    OFX::EPixelComponent srcComponents = _clipSrc->getPixelComponents();*/

    //	TUTTLE_LOG_INFO( _clipSrc->getPixelComponents() << " $$ " << _clipDst->getPixelComponents() );

    doGilRender2<ComponentProcess>(*this, args, *_clipSrc, *_clipDst);
    /*
    switch( dstComponents )
    {
            case OFX::ePixelComponentRGBA:
            {
                    renderWithDstComponent<rgba_layout_t>( args, srcComponents, bitDepth );
                    return;
            }
            case OFX::ePixelComponentRGB:
            {
                    renderWithDstComponent<rgb_layout_t>( args, srcComponents, bitDepth );
                    return;
            }
            case OFX::ePixelComponentAlpha:
            {
                    renderWithDstComponent<gray_layout_t>( args, srcComponents, bitDepth );
                    return;
            }
            case OFX::ePixelComponentCustom:
            case OFX::ePixelComponentNone:
            {
                    BOOST_THROW_EXCEPTION( exception::Unsupported()
                            << exception::user() + "Pixel components (" + mapPixelComponentEnumToString(dstComponents) + ")
    not supported by the plugin." );
            }
    }
    BOOST_THROW_EXCEPTION( exception::Unknown() );*/
}
}
}
}
