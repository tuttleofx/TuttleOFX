#include "ConvolutionPlugin.hpp"
#include "ConvolutionProcess.hpp"
#include "ConvolutionDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>

#include <boost/numeric/conversion/cast.hpp>

namespace tuttle {
namespace plugin {
namespace convolution {

ConvolutionPlugin::ConvolutionPlugin( OfxImageEffectHandle handle )
	: ImageEffect( handle )
{
	_clipSrc = fetchClip( kOfxImageEffectSimpleSourceClipName );
	_clipDst = fetchClip( kOfxImageEffectOutputClipName );

	_paramSize = fetchInt2DParam( kParamSize );

	_paramCoef.resize( kParamSizeMax );
	for( unsigned int y = 0; y < kParamSizeMax; ++y )
	{
		_paramCoef[y].resize( kParamSizeMax );
		for( unsigned int x = 0; x < kParamSizeMax; ++x )
		{
			_paramCoef[y][x] = fetchDoubleParam( getCoefName( y, x ) );
		}
	}
}

ConvolutionProcessParams ConvolutionPlugin::getProcessParams() const
{
	ConvolutionProcessParams params;

	OfxPointI size = _paramSize->getValue();

	params._size.x = boost::numeric_cast<unsigned int>( size.x );
	params._size.y = boost::numeric_cast<unsigned int>( size.y );

	params._convMatrix.resize( params._size.x, params._size.y );
	for( unsigned int y = 0; y < params._size.y; ++y )
	{
//		unsigned int yy = y * params._size.x;
		for( unsigned int x = 0; x < params._size.x; ++x )
		{
			params._convMatrix(x, y) = _paramCoef[y][x]->getValue();
			COUT( "coef[" << y << "][" << x << "] = " << params._convMatrix(x, y) );
		}
	}
	return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void ConvolutionPlugin::render( const OFX::RenderArguments& args )
{
	using namespace boost::gil;
	// instantiate the render code based on the pixel depth of the dst clip
	OFX::EBitDepth dstBitDepth         = _clipDst->getPixelDepth();
	OFX::EPixelComponent dstComponents = _clipDst->getPixelComponents();

	// do the rendering
	if( dstComponents == OFX::ePixelComponentRGBA )
	{
		switch( dstBitDepth )
		{
			case OFX::eBitDepthUByte:
			{
				ConvolutionProcess<rgba8_view_t> p( *this );
				p.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthUShort:
			{
				ConvolutionProcess<rgba16_view_t> p( *this );
				p.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthFloat:
			{
				ConvolutionProcess<rgba32f_view_t> p( *this );
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
	else if( dstComponents == OFX::ePixelComponentAlpha )
	{
		switch( dstBitDepth )
		{
			case OFX::eBitDepthUByte:
			{
				ConvolutionProcess<gray8_view_t> p( *this );
				p.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthUShort:
			{
				ConvolutionProcess<gray16_view_t> p( *this );
				p.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthFloat:
			{
				ConvolutionProcess<gray32f_view_t> p( *this );
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
}

void ConvolutionPlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois )
{
	OfxRectD srcRoi = args.regionOfInterest;
	OfxPointI size  = _paramSize->getValue();
	OfxPointD halfSize;

	halfSize.x = size.x * 0.5;
	halfSize.y = size.y * 0.5;
	srcRoi.x1 -= halfSize.x;
	srcRoi.x2 += halfSize.x;
	srcRoi.y1 -= halfSize.y;
	srcRoi.y2 += halfSize.y;
	rois.setRegionOfInterest( *_clipSrc, srcRoi );
}

void ConvolutionPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kParamSize && args.reason == OFX::eChangeUserEdit )
	{
		OfxPointI v = _paramSize->getValue();
		OfxPointI oddNumber;
		oddNumber.x = v.x | 1; // odd number
		oddNumber.y = v.y | 1; // odd number
		if( oddNumber != v )
			_paramSize->setValue( oddNumber );
	}
	else if( paramName == kHelpButton )
	{
		sendMessage( OFX::Message::eMessageMessage,
		             "", // No XML resources
		             kHelpString );
	}
}

}
}
}

