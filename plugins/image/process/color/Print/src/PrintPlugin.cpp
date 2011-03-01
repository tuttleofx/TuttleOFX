#include "PrintPlugin.hpp"
#include "PrintProcess.hpp"
#include "PrintDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace print {


PrintPlugin::PrintPlugin( OfxImageEffectHandle handle )
: ImageEffectGilPlugin( handle )
{
	_paramMode = fetchChoiceParam( kParamMode );
	_paramOutput = fetchChoiceParam( kParamOutput );
	_paramPixel = fetchInt2DParam( kParamPixel );
	_paramRegionMin = fetchInt2DParam( kParamRegionMin );
	_paramRegionMax = fetchInt2DParam( kParamRegionMax );
	_paramFlip = fetchBooleanParam( kParamFlip );
}

PrintProcessParams<PrintPlugin::Scalar> PrintPlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	PrintProcessParams<Scalar> params;

	params._mode = static_cast<EParamMode>( _paramMode->getValue() );
	params._output = static_cast<EParamOutput>( _paramOutput->getValue() );
	OfxPointI pix = _paramPixel->getValue();
	params._pixel.x = pix.x;
	params._pixel.y = pix.y;
	OfxPointI rMin = _paramRegionMin->getValue();
	OfxPointI rMax = _paramRegionMax->getValue();
	params._region.x1 = rMin.x;
	params._region.y1 = rMin.y;
	params._region.x2 = rMax.x;
	params._region.y2 = rMax.y;
	params._flip = _paramFlip->getValue();
	
	return params;
}

void PrintPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
//    if( paramName == kParamHelpButton )
//    {
//        sendMessage( OFX::Message::eMessageMessage,
//                     "", // No XML resources
//                     kParamHelpString );
//    }
}

//bool PrintPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
//{
//	PrintProcessParams<Scalar> params = getProcessParams();
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
//void PrintPlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois )
//{
//	PrintProcessParams<Scalar> params = getProcessParams();
//	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
//
//	OfxRectD srcRoi;
//	srcRoi.x1 = srcRod.x1 - 1;
//	srcRoi.y1 = srcRod.y1 - 1;
//	srcRoi.x2 = srcRod.x2 + 1;
//	srcRoi.y2 = srcRod.y2 + 1;
//	rois.setRegionOfInterest( *_clipSrc, srcRoi );
//}

bool PrintPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
//	PrintProcessParams<Scalar> params = getProcessParams();
//	if( params._in == params._out )
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
void PrintPlugin::render( const OFX::RenderArguments &args )
{
	doGilRender<PrintProcess>( *this, args );
}


}
}
}
