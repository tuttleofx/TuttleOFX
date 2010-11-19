#include "NormalizePlugin.hpp"
#include "NormalizeProcess.hpp"
#include "NormalizeDefinitions.hpp"
#include "NormalizeAlgorithm.hpp"

#include <tuttle/common/utils/global.hpp>
#include <tuttle/plugin/NoProgress.hpp>
#include <tuttle/plugin/image/ofxToGil.hpp>
#include <tuttle/plugin/param/gilColor.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/numeric/pixel_numeric_operations.hpp>
#include <boost/gil/extension/numeric/pixel_numeric_operations2.hpp>

namespace tuttle {
namespace plugin {
namespace normalize {


NormalizePlugin::NormalizePlugin( OfxImageEffectHandle handle ) :
ImageEffect( handle )
{
    _clipSrc = fetchClip( kOfxImageEffectSimpleSourceClipName );
    _clipDst = fetchClip( kOfxImageEffectOutputClipName );

	_mode = fetchChoiceParam( kParamMode );
	_analyseMode = fetchChoiceParam( kParamAnalyseMode );
	_analyseNow = fetchPushButtonParam( kParamAnalyseNow );
	_srcGroup = fetchGroupParam( kParamSrcGroup );
	_srcMinColor = fetchRGBAParam( kParamSrcCustomColorMin );
	_srcMaxColor = fetchRGBAParam( kParamSrcCustomColorMax );
	_dstGroup = fetchGroupParam( kParamDstGroup );
	_dstMinColor = fetchRGBAParam( kParamDstCustomColorMin );
	_dstMaxColor = fetchRGBAParam( kParamDstCustomColorMax );
	_processGroup = fetchGroupParam( kParamProcessGroup );
	_processR = fetchBooleanParam( kParamProcessR );
	_processG = fetchBooleanParam( kParamProcessG );
	_processB = fetchBooleanParam( kParamProcessB );
	_processA = fetchBooleanParam( kParamProcessA );

	// init param props
	static const OFX::InstanceChangedArgs args;
	changedParam( args, kParamMode );
}

NormalizeProcessParams<NormalizePlugin::Scalar> NormalizePlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	NormalizeProcessParams<Scalar> params;

	params._mode         = static_cast<EParamMode>( _mode->getValue() );
	params._analyseMode  = static_cast<EParamAnalyseMode>( _analyseMode->getValue() );
	
	params._srcMinColor  = ofxToGil<boost::gil::bits32f>( _srcMinColor->getValue() );
	params._srcMaxColor  = ofxToGil<boost::gil::bits32f>( _srcMaxColor->getValue() );
	params._dstMinColor  = ofxToGil<boost::gil::bits32f>( _dstMinColor->getValue() );
	params._dstMaxColor  = ofxToGil<boost::gil::bits32f>( _dstMaxColor->getValue() );

	params._processR     = _processR->getValue();
	params._processG     = _processG->getValue();
	params._processB     = _processB->getValue();
	params._processA     = _processA->getValue();

	return params;
}

void NormalizePlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
    if( paramName == kParamMode )
    {
		switch( static_cast<EParamMode>( _mode->getValue() ) )
		{
			case eParamModeAnalyse:
			{
				_srcGroup->setIsSecretAndDisabled( true );
				_srcMinColor->setIsSecretAndDisabled( true );
				_srcMaxColor->setIsSecretAndDisabled( true );
				_analyseNow->setIsSecretAndDisabled( true );
				break;
			}
			case eParamModeCustom:
			{
				_srcGroup->setIsSecretAndDisabled( false );
				_srcMinColor->setIsSecretAndDisabled( false );
				_srcMaxColor->setIsSecretAndDisabled( false );
				_analyseNow->setIsSecretAndDisabled( false );
				break;
			}
		}
    }
	else if( paramName == kParamAnalyseNow )
	{
		using namespace boost::gil;
		if( ! _clipSrc->isConnected() )
			return;

		switch( _clipSrc->getPixelDepth() )
		{
			case OFX::eBitDepthFloat:
			{
				typedef rgba32f_view_t View;
				typedef View::value_type Pixel;
				boost::scoped_ptr<OFX::Image> src( _clipSrc->fetchImage( args.time ) );
				if( ! src.get() )
					BOOST_THROW_EXCEPTION( exception::ImageNotReady() );
				if( src->getRowBytes() <= 0 )
					BOOST_THROW_EXCEPTION( exception::WrongRowBytes() );
				OfxRectI srcPixelRod = _clipSrc->getPixelRod( args.time, args.renderScale );
				View srcView = getView<View>( src.get(), srcPixelRod );

				Pixel smin;
				Pixel smax;
				NoProgress progress;
				analyseInputMinMax<View>( srcView, static_cast<EParamAnalyseMode>( _analyseMode->getValue() ), smin, smax, progress );
				setRGBAParamValues( _srcMinColor, smin );
				setRGBAParamValues( _srcMaxColor, smax );
				break;
			}
		}

	}
}

void NormalizePlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois )
{
	NormalizeProcessParams<Scalar> params = getProcessParams();
	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
	switch( params._mode )
	{
		case eParamModeAnalyse:
		{
			// in this case, we need the full input image to do the analyse.
			rois.setRegionOfInterest( *_clipSrc, srcRod );
			break;
		}
		case eParamModeCustom:
		{
			rois.setRegionOfInterest( *_clipSrc, args.regionOfInterest );
			break;
		}
	}

}

bool NormalizePlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
	NormalizeProcessParams<Scalar> params = getProcessParams();
	if( params._mode == eParamModeCustom &&
		params._srcMinColor == params._dstMinColor &&
	    params._srcMaxColor == params._dstMaxColor )
	{
		identityClip = _clipSrc;
		identityTime = args.time;
		return true;
	}
	return false;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void NormalizePlugin::render( const OFX::RenderArguments &args )
{
	using namespace boost::gil;
    // instantiate the render code based on the pixel depth of the dst clip
    OFX::EBitDepth dstBitDepth = _clipDst->getPixelDepth( );
    OFX::EPixelComponent dstComponents = _clipDst->getPixelComponents( );

    // do the rendering
    if( dstComponents == OFX::ePixelComponentRGBA )
    {
        switch( dstBitDepth )
        {
//            case OFX::eBitDepthUByte :
//            {
//                NormalizeProcess<rgba8_view_t> p( *this );
//                p.setupAndProcess( args );
//                break;
//            }
//            case OFX::eBitDepthUShort :
//            {
//                NormalizeProcess<rgba16_view_t> p( *this );
//                p.setupAndProcess( args );
//                break;
//            }
            case OFX::eBitDepthFloat :
            {
                NormalizeProcess<rgba32f_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
			default:
			{
				COUT_ERROR( "Bit depth (" << mapBitDepthEnumToString(dstBitDepth) << ") not recognized by the plugin." );
				break;
			}
        }
    }
    else if( dstComponents == OFX::ePixelComponentAlpha )
    {
        switch( dstBitDepth )
        {
//            case OFX::eBitDepthUByte :
//            {
//                NormalizeProcess<gray8_view_t> p( *this );
//                p.setupAndProcess( args );
//                break;
//            }
//            case OFX::eBitDepthUShort :
//            {
//                NormalizeProcess<gray16_view_t> p( *this );
//                p.setupAndProcess( args );
//                break;
//            }
//            case OFX::eBitDepthFloat :
//            {
//                NormalizeProcess<gray32f_view_t> p( *this );
//                p.setupAndProcess( args );
//                break;
//            }
			default:
			{
				COUT_ERROR( "Bit depth (" << mapBitDepthEnumToString(dstBitDepth) << ") not recognized by the plugin." );
				break;
			}
        }
    }
	else
	{
		COUT_ERROR( "Pixel components (" << mapPixelComponentEnumToString(dstComponents) << ") not supported by the plugin." );
	}
}


}
}
}
