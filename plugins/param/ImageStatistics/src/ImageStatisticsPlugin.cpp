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

ImageStatisticsPlugin::ImageStatisticsPlugin( OfxImageEffectHandle handle ) :
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

ImageStatisticsProcessParams ImageStatisticsPlugin::getProcessParams( const OfxRectI& srcRod ) const
{
	ImageStatisticsProcessParams params;
	OfxPointD cornerA = _cornerA->getValue();
	OfxPointD cornerB = _cornerB->getValue();
	OfxPointD rodSize;
	rodSize.x = srcRod.x2 - srcRod.x1;
	rodSize.y = srcRod.y2 - srcRod.y1;
	cornerA = interact::pointNormalizedXXcToCanonicalXY( cornerA, rodSize); // XX centered normalized to canonical
	cornerB = interact::pointNormalizedXXcToCanonicalXY( cornerB, rodSize);
	params._rect.x1 = boost::numeric_cast<int>( std::min( cornerA.x, cornerB.x ) );
	params._rect.y1 = boost::numeric_cast<int>( std::min( cornerA.y, cornerB.y ) );
	params._rect.x2 = boost::numeric_cast<int>( std::max( cornerA.x, cornerB.x ) );
	params._rect.y2 = boost::numeric_cast<int>( std::max( cornerA.y, cornerB.y ) );
	params._rect = rectanglesIntersection( params._rect, srcRod );
	params._chooseOutput = static_cast<EChooseOutput>( _chooseOutput->getValue() );
	return params;
}

void ImageStatisticsPlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments &args, OFX::RegionOfInterestSetter &rois )
{
    OfxRectI srcRealRoi = getProcessParams( _srcClip->getPixelRod(args.time) )._rect; // we need the selected rectangle
    OfxRectD srcRealRoiD;
	srcRealRoiD.x1 = srcRealRoi.x1;
	srcRealRoiD.y1 = srcRealRoi.y1;
	srcRealRoiD.x2 = srcRealRoi.x2;
	srcRealRoiD.y2 = srcRealRoi.y2;
    rois.setRegionOfInterest( *_srcClip, srcRealRoiD );
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void ImageStatisticsPlugin::render( const OFX::RenderArguments &args )
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
                ImageStatisticsProcess<rgba8_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthUShort :
            {
                ImageStatisticsProcess<rgba16_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthFloat :
            {
                ImageStatisticsProcess<rgba32f_view_t> p( *this );
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
}

void ImageStatisticsPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
}

}
}
}
