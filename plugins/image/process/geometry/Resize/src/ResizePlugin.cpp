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
	_paramType		= fetchChoiceParam	( kParamType );
	_paramOutputFormat	= fetchDouble2DParam	( kParamOutputFormat );
	_paramScale		= fetchDoubleParam	( kParamScale );
	_paramFilter		= fetchChoiceParam	( kParamFilter );
}

ResizeProcessParams<ResizePlugin::Scalar> ResizePlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	ResizeProcessParams<Scalar> params;

	OfxPointD size = _paramOutputFormat -> getValue();
	params._size.x = size.x;
	params._size.y = size.y;

	return params;
}

void ResizePlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
	if( paramName == kParamOutputFormat )
	{
		//_paramScale -> setValue( 1 );
	}
	else if( paramName == kParamScale )
	{
		//_paramOutputFormat -> setValue( 1, 1 );
	}
	if( _paramType->getValue() == eParamFilterToFormat )
	{
		_paramScale		-> setEnabled( false );

		_paramOutputFormat	-> setEnabled( true );
		_paramOutputFormat	-> setIsSecret( false );
	}
	else
	{
		_paramOutputFormat	-> setEnabled( false );

		_paramScale		-> setIsSecret( false );
		_paramScale		-> setEnabled( true );

	}
}

bool ResizePlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	using namespace bgil;
	const OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
	const Point2 srcRodCorner( srcRod.x1, srcRod.y1 );
	const Point2 srcRodSize( srcRod.x2 - srcRod.x1, srcRod.y2 - srcRod.y1 );
	const OfxRectD srcRodInDstFrame = { 0, 0, srcRodSize.x, srcRodSize.y };

	bool modified = false;

	//LensDistortProcessParams<Scalar> params( getProcessParams( srcRod, srcRod, _clipDst->getPixelAspectRatio(), true ) );
	ResizeProcessParams<Scalar> params = getProcessParams();
	switch( static_cast<EParamType>( _paramType->getValue() ) )
	{
		case eParamFilterToFormat:
		{			float scaleX = params._size.x/srcRodSize.x;
			float scaleY = params._size.y/srcRodSize.y;
			rod.x1   = 0;
			rod.y1   = 0;
			rod.x2   = params._size.x;
			rod.y2   = params._size.y;
			/*
			rod.x1   = scaleX * 0.5;
			rod.y1   = scaleY * 0.5;
			rod.x2   = params._size.x-rod.x1;
			rod.y2   = params._size.y-rod.y1;*/
			modified = true;
			return true;
		}
		case eParamFilterScale:
		{
			double scale = _paramScale->getValue();
			if( scale == 1.0 || scale == 0.0 )
				return false;

			point2<double> pMin( srcRodInDstFrame.x1, srcRodInDstFrame.y1 ); // top left corner
			point2<double> pMax( srcRodInDstFrame.x2, srcRodInDstFrame.y2 ); // down right corner
			point2<double> center( srcRodSize * 0.5 );
			//pMin     = ( ( pMin - center ) * scale ) + center;
			//pMax     = ( ( pMax - center ) * scale ) + center;
			rod.x1   = pMin.x * scale;
			rod.y1   = pMin.y * scale;
			rod.x2   = pMax.x * scale;
			rod.y2   = pMax.y * scale;
			modified = true;
		}
	}
	rod.x1 += srcRodCorner.x;
	rod.y1 += srcRodCorner.y;
	rod.x2 += srcRodCorner.x;
	rod.y2 += srcRodCorner.y;
	return modified;
}

void ResizePlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois )
{
	/*ResizeProcessParams<Scalar> params = getProcessParams();
	rois.x1   = 0;
	rois.y1   = 0;
	rois.x2   = params._size.x;
	rois.y2   = params._size.y;*/

	/*ResizeProcessParams<Scalar> params = getProcessParams();
	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
//
	OfxRectD srcRoi;
	srcRoi.x1 = srcRod.x1 - 1;
	srcRoi.y1 = srcRod.y1 - 1;
	srcRoi.x2 = srcRod.x2 + 1;
	srcRoi.y2 = srcRod.y2 + 1;
	rois.setRegionOfInterest( *_clipSrc, srcRoi );*/
	/*
	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
	OfxRectD dstRod = _clipDst->getCanonicalRod( args.time );

	ResizeProcessParams<Scalar> params = getProcessParams();

	OfxRectD outputRoi = args.regionOfInterest;
	outputRoi.x1 -= dstRod.x1; // to dest rod coordinates
	outputRoi.y1 -= dstRod.y1;
	outputRoi.x2 -= dstRod.x1;
	outputRoi.y2 -= dstRod.y1;
	OfxRectD srcRoi = ;
	srcRoi.x1    += srcRod.x1; // to RoW coordinates
	srcRoi.y1    += srcRod.y1;
	srcRoi.x2    += srcRod.x1;
	srcRoi.y2    += srcRod.y1;
	outputRoi.x1 += dstRod.x1; // to RoW coordinates
	outputRoi.y1 += dstRod.y1;
	outputRoi.x2 += dstRod.x1;
	outputRoi.y2 += dstRod.y1;
	//    srcRoi.x1 += 2; // if we remove 2 pixels to the needed RoI the plugin crash, because it tries to access to this pixels
	//    srcRoi.y1 += 2; // so the calcul of the RoI has a precision of one pixel
	//    srcRoi.x2 -= 2;
	//    srcRoi.y2 -= 2;
	OfxRectD srcRealRoi = rectanglesIntersection( srcRoi, srcRod );
	srcRealRoi = srcRod;*/

	//rois.setRegionOfInterest( *_clipSrc, outputRoi );
/*
	if( _debugDisplayRoi->getValue() )
	{
		_srcRealRoi = srcRealRoi;
		_srcRoi     = srcRoi;
		_dstRoi     = outputRoi;
	}*/
}

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
