#include "Move2DPlugin.hpp"
#include "Move2DProcess.hpp"
#include "Move2DDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace move2D {


Move2DPlugin::Move2DPlugin( OfxImageEffectHandle handle )
: ImageEffectGilPlugin( handle )
{
//    _clipSrcMatte = fetchClip( kClipMatte );
	_paramTranslation = fetchDouble2DParam( kParamTranslation );
}

Move2DProcessParams<Move2DPlugin::Scalar> Move2DPlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	Move2DProcessParams<Scalar> params;

	OfxPointD translation = _paramTranslation->getValue();
	params._translation.x = translation.x;
	params._translation.y = translation.y;
	
	return params;
}

void Move2DPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
}

bool Move2DPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	Move2DProcessParams<Scalar> params = getProcessParams();
	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );

//	switch( params._border )
//	{
//		case eParamBorderPadded:
			rod.x1 = srcRod.x1 + params._translation.x;
			rod.y1 = srcRod.y1 + params._translation.y;
			rod.x2 = srcRod.x2 + params._translation.x;
			rod.y2 = srcRod.y2 + params._translation.y;
			return true;
//		default:
//			break;
//	}
//	return false;
}

//void Move2DPlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois )
//{
//	Move2DProcessParams<Scalar> params = getProcessParams();
//	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
//
//	OfxRectD srcRoi;
//	srcRoi.x1 = srcRod.x1 - 1;
//	srcRoi.y1 = srcRod.y1 - 1;
//	srcRoi.x2 = srcRod.x2 + 1;
//	srcRoi.y2 = srcRod.y2 + 1;
//	rois.setRegionOfInterest( *_clipSrc, srcRoi );
//}

bool Move2DPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
//	Move2DProcessParams<Scalar> params = getProcessParams();
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
void Move2DPlugin::render( const OFX::RenderArguments &args )
{
	doGilRender<Move2DProcess>( *this, args );
}


}
}
}
