#include "AveragePlugin.hpp"
#include "AverageProcess.hpp"
#include "AverageDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <tuttle/common/math/rectOp.hpp>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace average {

using namespace boost::gil;

AveragePlugin::AveragePlugin( OfxImageEffectHandle handle ) :
ImageEffect( handle )
{
    _srcClip = fetchClip( kOfxImageEffectSimpleSourceClipName );
    _dstClip = fetchClip( kOfxImageEffectOutputClipName );

	_cornerA = fetchDouble2DParam( kCornerA );
	_cornerB = fetchDouble2DParam( kCornerB );
	_chooseOutput = fetchChoiceParam( kChooseOutput );
	_outputAverage = fetchRGBAParam( kOutputAverage );
	_outputChannelMin = fetchRGBAParam( kOutputChannelMin );
	_outputChannelMax = fetchRGBAParam( kOutputChannelMax );
	_outputLuminosityMin = fetchRGBAParam( kOutputLuminosityMin );
	_outputLuminosityMax = fetchRGBAParam( kOutputLuminosityMax );
}

AverageProcessParams AveragePlugin::getProcessParams( const OfxRectD& srcRod ) const
{
	AverageProcessParams params;
	OfxPointD cornerA = _cornerA->getValue();
	OfxPointD cornerB = _cornerB->getValue();
	OfxPointD rodSize;
	rodSize.x = srcRod.x2 - srcRod.x1;
	rodSize.y = srcRod.y2 - srcRod.y1;
	cornerA *= rodSize; // XY normalized to canonical
	cornerB *= rodSize;
	params._rect.x1 = std::min( cornerA.x, cornerB.x );
	params._rect.y1 = std::min( cornerA.y, cornerB.y );
	params._rect.x2 = std::max( cornerA.x, cornerB.x );
	params._rect.y2 = std::max( cornerA.y, cornerB.y );
	params._rect = rectanglesIntersection( params._rect, srcRod );
	params._chooseOutput = static_cast<EChooseOutput>(_chooseOutput->getValue());
	return params;
}

//bool AveragePlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
//{
//	identityClip = _srcClip;
//	identityTime = args.time;
//	return true;
//}


void AveragePlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments &args, OFX::RegionOfInterestSetter &rois )
{
    OfxRectD srcRealRoi = getProcessParams( _srcClip->getCanonicalRod(args.time) )._rect;
	
    rois.setRegionOfInterest( *_srcClip, srcRealRoi );
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void AveragePlugin::render( const OFX::RenderArguments &args )
{
    // instantiate the render code based on the pixel depth of the dst clip
    OFX::BitDepthEnum dstBitDepth = _dstClip->getPixelDepth( );
    OFX::PixelComponentEnum dstComponents = _dstClip->getPixelComponents( );

    // do the rendering
    if( dstComponents == OFX::ePixelComponentRGBA )
    {
        switch( dstBitDepth )
        {
            case OFX::eBitDepthUByte :
            {
                AverageProcess<rgba8_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthUShort :
            {
                AverageProcess<rgba16_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthFloat :
            {
                AverageProcess<rgba32f_view_t> p( *this );
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
    else if( dstComponents == OFX::ePixelComponentAlpha )
    {
        switch( dstBitDepth )
        {
            case OFX::eBitDepthUByte :
            {
                AverageProcess<gray8_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthUShort :
            {
                AverageProcess<gray16_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthFloat :
            {
                AverageProcess<gray32f_view_t> p( *this );
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
}

void AveragePlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
}

}
}
}
