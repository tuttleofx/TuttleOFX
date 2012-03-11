#include "FlipDefinitions.hpp"
#include "FlipPlugin.hpp"
#include "FlipProcess.hpp"

#include <tuttle/plugin/ofxToGil/point.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/math/special_functions/round.hpp>

namespace tuttle {
namespace plugin {
namespace flip {

using namespace boost::math;
using namespace boost::gil;

FlipPlugin::FlipPlugin( OfxImageEffectHandle handle )
	: ImageEffectGilPlugin( handle )
{
        _clipSrcRef = this->fetchClip( kOfxImageEffectSimpleSourceClipName );
        _clipDstRef = this->fetchClip( kOfxImageEffectOutputClipName );
        _paramFlip   = fetchBooleanParam ( kParamFlip );
        _paramFlop   = fetchBooleanParam ( kParamFlop );
}


OfxRectI FlipPlugin::computeFlipRegion( const OfxTime time, const bool fromRatio ) const
{
        const OfxRectI sRod = _clipSrc->getPixelRod ( time );
        return sRod;
}


FlipProcessParams FlipPlugin::getProcessParams( const OfxTime time, const OfxPointD& renderScale ) const
{
        FlipProcessParams params;

        params.flip = _paramFlip->getValue();
        params.flop = _paramFlop->getValue();

        return params;
}

void FlipPlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois )
{
       if( _clipSrcRef->isConnected() )
       {
               OfxRectD srcRod = args.regionOfInterest;
               rois.setRegionOfInterest( *this->_clipSrc, srcRod );
       }
       else
       {
               OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
               rois.setRegionOfInterest( *this->_clipSrc, srcRod );
       }

       OfxRectD srcRefRod = _clipSrcRef->getCanonicalRod( args.time );
       rois.setRegionOfInterest( *_clipSrcRef, srcRefRod );

       OfxRectD dstRefRod = _clipDstRef->getCanonicalRod( args.time );
       rois.setRegionOfInterest( *_clipDstRef, dstRefRod );
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void FlipPlugin::render( const OFX::RenderArguments& args )
{
        doGilRender<FlipProcess>( *this, args );
}

}
}
}
