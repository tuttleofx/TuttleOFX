#include "ImageStatisticsPlugin.hpp"
#include "ImageStatisticsProcess.hpp"
#include "ImageStatisticsDefinitions.hpp"

#include <boost/numeric/conversion/cast.hpp>

#include <tuttle/plugin/interact/InteractInfos.hpp>
#include <tuttle/common/utils/global.hpp>
#include <tuttle/common/math/rectOp.hpp>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace imageStatistics {

using namespace boost::gil;

ImageStatisticsPlugin::ImageStatisticsPlugin( OfxImageEffectHandle handle )
	: ImageEffect( handle )
{
	_clipSrc = fetchClip( kOfxImageEffectSimpleSourceClipName );
	_clipDst = fetchClip( kOfxImageEffectOutputClipName );

	_paramCoordinateSystem = fetchChoiceParam( kParamCoordinateSystem );
	_paramRectCenter       = fetchDouble2DParam( kParamRectCenter );
	_paramRectSize         = fetchDouble2DParam( kParamRectSize );
	_paramChooseOutput     = fetchChoiceParam( kParamChooseOutput );

	_paramOutputAverage       = fetchRGBAParam( kParamOutputAverage );
	_paramOutputChannelMin    = fetchRGBAParam( kParamOutputChannelMin );
	_paramOutputChannelMax    = fetchRGBAParam( kParamOutputChannelMax );
	_paramOutputLuminosityMin = fetchRGBAParam( kParamOutputLuminosityMin );
	_paramOutputLuminosityMax = fetchRGBAParam( kParamOutputLuminosityMax );
	_paramOutputKurtosis      = fetchRGBAParam( kParamOutputKurtosis );
	_paramOutputSkewness      = fetchRGBAParam( kParamOutputSkewness );

	_paramOutputAverageHSL       = fetchDouble3DParam( kParamOutputAverageHSL );
	_paramOutputChannelMinHSL    = fetchDouble3DParam( kParamOutputChannelMinHSL );
	_paramOutputChannelMaxHSL    = fetchDouble3DParam( kParamOutputChannelMaxHSL );
	_paramOutputLuminosityMinHSL = fetchDouble3DParam( kParamOutputLuminosityMinHSL );
	_paramOutputLuminosityMaxHSL = fetchDouble3DParam( kParamOutputLuminosityMaxHSL );
	_paramOutputKurtosisHSL      = fetchDouble3DParam( kParamOutputKurtosisHSL );
	_paramOutputSkewnessHSL      = fetchDouble3DParam( kParamOutputSkewnessHSL );
}

ImageStatisticsProcessParams ImageStatisticsPlugin::getProcessParams( const OfxRectI& srcRod ) const
{
	ImageStatisticsProcessParams params;
	OfxPointD rectCenter = _paramRectCenter->getValue();
	OfxPointD rectSize   = _paramRectSize->getValue();

	rectSize.x = std::abs( rectSize.x );
	rectSize.y = std::abs( rectSize.y );
	if( _paramCoordinateSystem->getValue() == eParamCoordinateSystemNormalized )
	{
		OfxPointD projectSize = this->getProjectSize();
		rectCenter *= projectSize;
		rectSize   *= projectSize;
	}

	params._rect.x1      = boost::numeric_cast<int>( rectCenter.x - rectSize.x );
	params._rect.y1      = boost::numeric_cast<int>( rectCenter.y - rectSize.y );
	params._rect.x2      = boost::numeric_cast<int>( std::ceil( rectCenter.x + rectSize.x ) );
	params._rect.y2      = boost::numeric_cast<int>( std::ceil( rectCenter.y + rectSize.y ) );
	params._rect         = rectanglesIntersection( params._rect, srcRod );
	params._chooseOutput = static_cast<EParamChooseOutput>( _paramChooseOutput->getValue() );

	return params;
}

void ImageStatisticsPlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois )
{
	OfxRectI srcRealRoi = getProcessParams( _clipSrc->getPixelRod( args.time ) )._rect; // we need the selected rectangle
	OfxRectD srcRealRoiD;

	srcRealRoiD.x1 = srcRealRoi.x1;
	srcRealRoiD.y1 = srcRealRoi.y1;
	srcRealRoiD.x2 = srcRealRoi.x2;
	srcRealRoiD.y2 = srcRealRoi.y2;
	COUT_VAR( srcRealRoiD );
	rois.setRegionOfInterest( *_clipSrc, srcRealRoiD );
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void ImageStatisticsPlugin::render( const OFX::RenderArguments& args )
{
	// instantiate the render code based on the pixel depth of the dst clip
	OFX::EBitDepth dstBitDepth         = _clipDst->getPixelDepth();
	OFX::EPixelComponent dstComponents = _clipDst->getPixelComponents();

	// do the rendering
	if( dstComponents == OFX::ePixelComponentRGBA )
	{
		switch( dstBitDepth )
		{
		   case OFX::eBitDepthUByte :
		   {
				COUT_FATALERROR( "BitDepthUByte not recognize." );
//				ImageStatisticsProcess<rgba8_view_t> p( *this );
//				p.setupAndProcess( args );
				break;
		   }
		   case OFX::eBitDepthUShort :
		   {
				COUT_FATALERROR( "BitDepthUShort not recognize." );
//				ImageStatisticsProcess<rgba16_view_t> p( *this );
//				p.setupAndProcess( args );
				break;
		   }
			case OFX::eBitDepthFloat:
			{
				ImageStatisticsProcess<rgba32f_view_t> p( *this );
				p.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthNone:
				COUT_FATALERROR( "BitDepthNone not recognize." );
				return;
			case OFX::eBitDepthCustom:
				COUT_FATALERROR( "BitDepthCustom not recognize." );
				return;
		}
	}
	/*
	     else if( dstComponents == OFX::ePixelComponentAlpha )
	     {
			 switch( dstBitDepth )
			 {
				case OFX::eBitDepthUByte :
				{
					ImageStatisticsProcess<gray8_view_t> p( *this );
					p.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthUShort :
				{
					ImageStatisticsProcess<gray16_view_t> p( *this );
					p.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthFloat :
				{
					ImageStatisticsProcess<gray32f_view_t> p( *this );
					p.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthNone :
					COUT_FATALERROR( "BitDepthNone not recognize." );
					return;
				case OFX::eBitDepthCustom :
					COUT_FATALERROR( "BitDepthCustom not recognize." );
					return;
			 }
	     }
	 */
	else
	{
		COUT_FATALERROR( "Unrecognized components." );
	}
}

void ImageStatisticsPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kParamCoordinateSystem )
	{
		OfxPointD projectSize = this->getProjectSize();
		OfxPointD rectCenter  = _paramRectCenter->getValue();
		OfxPointD rectSize    = _paramRectSize->getValue();
		switch( _paramCoordinateSystem->getValue() )
		{
			case eParamCoordinateSystemCanonical:
			{
				rectCenter *= projectSize;
				rectSize   *= projectSize;
				break;
			}
			case eParamCoordinateSystemNormalized:
			{
				rectCenter /= projectSize;
				rectSize   /= projectSize;
				break;
			}
			default:
				BOOST_THROW_EXCEPTION( exception::Value() << exception::user( "Unrecognized coordinate system option." ) );
		}
		_paramRectCenter->setValue( rectCenter );
		_paramRectSize->setValue( rectSize );
	}
}

}
}
}
