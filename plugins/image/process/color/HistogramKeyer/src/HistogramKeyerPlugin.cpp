#include "HistogramKeyerPlugin.hpp"
#include "HistogramKeyerProcess.hpp"
#include "HistogramKeyerDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace histogramKeyer {


HistogramKeyerPlugin::HistogramKeyerPlugin( OfxImageEffectHandle handle )
: ImageEffectGilPlugin( handle )
{
//    _clipSrcMatte = fetchClip( kClipMatte );

	if( OFX::getImageEffectHostDescription()->supportsParametricParameter )
	{
//		_paramColorSelection = fetchParametricParam( kParamColorSelection );
	}
}

HistogramKeyerProcessParams<HistogramKeyerPlugin::Scalar> HistogramKeyerPlugin::getProcessParams( const OfxTime time, const OfxPointD& renderScale ) const
{
	HistogramKeyerProcessParams<Scalar> params;
//	for( int curveId = 0; curveId < nbCurves; ++curveId )
//	{
//		int n = _paramColorSelection->getNControlPoints( curveId, time );
//		Curve& c = params._curves[curveId];
//		for( int i = 0; i < n; ++i )
//		{
//			std::pair<double, double> v = _paramColorSelection->getNthControlPoints( curveId, time, i );
//			c[v.first] = v.second;
//		}
//	}
	return params;
}

void HistogramKeyerPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
}

//void HistogramKeyerPlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois )
//{
//	HistogramKeyerProcessParams<Scalar> params = getProcessParams();
//	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
//
//	OfxRectD srcRoi;
//	srcRoi.x1 = srcRod.x1 - 1;
//	srcRoi.y1 = srcRod.y1 - 1;
//	srcRoi.x2 = srcRod.x2 + 1;
//	srcRoi.y2 = srcRod.y2 + 1;
//	rois.setRegionOfInterest( *_clipSrc, srcRoi );
//}

bool HistogramKeyerPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
//	HistogramKeyerProcessParams<Scalar> params = getProcessParams();
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
void HistogramKeyerPlugin::render( const OFX::RenderArguments &args )
{
	doGilRender<HistogramKeyerProcess>( *this, args );
}


}
}
}
