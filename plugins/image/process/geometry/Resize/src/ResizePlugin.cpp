#include "ResizePlugin.hpp"
#include "ResizeProcess.hpp"
#include "ResizeDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace resize {


ResizePlugin::ResizePlugin( OfxImageEffectHandle handle )
: ImageEffectGilPlugin( handle )
{
//    _clipSrcMatte = fetchClip( kClipMatte );
	_paramSize = fetchDouble2DParam( kParamSize );
}

ResizeProcessParams<ResizePlugin::Scalar> ResizePlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	ResizeProcessParams<Scalar> params;

	OfxPointD size = _paramSize->getValue();
	params._size.x = size.x;
	params._size.y = size.y;
	
	return params;
}

void ResizePlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
}

bool ResizePlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	ResizeProcessParams<Scalar> params = getProcessParams();
//	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );

//	switch( params._border )
//	{
//		case eParamBorderPadded:
			rod.x1 = 0;
			rod.y1 = 0;
			rod.x2 = params._size.x;
			rod.y2 = params._size.y;
			return true;
//		default:
//			break;
//	}
//	return false;
}

//void ResizePlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois )
//{
//	ResizeProcessParams<Scalar> params = getProcessParams();
//	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
//
//	OfxRectD srcRoi;
//	srcRoi.x1 = srcRod.x1 - 1;
//	srcRoi.y1 = srcRod.y1 - 1;
//	srcRoi.x2 = srcRod.x2 + 1;
//	srcRoi.y2 = srcRod.y2 + 1;
//	rois.setRegionOfInterest( *_clipSrc, srcRoi );
//}

bool ResizePlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
//	ResizeProcessParams<Scalar> params = getProcessParams();
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
void ResizePlugin::render( const OFX::RenderArguments &args )
{
	doGilRender<ResizeProcess>( *this, args );
}


}
}
}
