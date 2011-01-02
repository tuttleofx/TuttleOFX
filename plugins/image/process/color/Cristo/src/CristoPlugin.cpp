#include "CristoPlugin.hpp"
#include "CristoProcess.hpp"
#include "CristoDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace cristo {


CristoPlugin::CristoPlugin( OfxImageEffectHandle handle )
: ImageEffectGilPlugin( handle )
{
//    _clipSrcMatte = fetchClip( kClipMatte );
	_paramColorSelection = fetchParametricParam( kParamColorSelection );
}

CristoProcessParams<CristoPlugin::Scalar> CristoPlugin::getProcessParams( const OfxTime time, const OfxPointD& renderScale ) const
{
	CristoProcessParams<Scalar> params;
	for( int curveId = 0; curveId < nbCurves; ++curveId )
	{
		int n = _paramColorSelection->getNControlPoints( curveId, time );
		Curve& c = params._curves[curveId];
		for( int i = 0; i < n; ++i )
		{
			std::pair<double, double> v = _paramColorSelection->getNthControlPoints( curveId, time, i );
			c[v.first] = v.second;
		}
	}
	return params;
}

void CristoPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
    if( paramName == kParamHelpButton )
    {
        sendMessage( OFX::Message::eMessageMessage,
                     "", // No XML resources
                     kParamHelpString );
    }
}

//bool CristoPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
//{
//	CristoProcessParams<Scalar> params = getProcessParams();
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
//void CristoPlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois )
//{
//	CristoProcessParams<Scalar> params = getProcessParams();
//	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
//
//	OfxRectD srcRoi;
//	srcRoi.x1 = srcRod.x1 - 1;
//	srcRoi.y1 = srcRod.y1 - 1;
//	srcRoi.x2 = srcRod.x2 + 1;
//	srcRoi.y2 = srcRod.y2 + 1;
//	rois.setRegionOfInterest( *_clipSrc, srcRoi );
//}

bool CristoPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
//	CristoProcessParams<Scalar> params = getProcessParams();
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
void CristoPlugin::render( const OFX::RenderArguments &args )
{
	doGilRender<CristoProcess>( *this, args );
}


}
}
}
