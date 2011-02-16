#include "NormalizePlugin.hpp"
#include "NormalizeProcess.hpp"
#include "NormalizeDefinitions.hpp"
#include "NormalizeAlgorithm.hpp"

#include <tuttle/plugin/NoProgress.hpp>
#include <tuttle/plugin/param/gilColor.hpp>

#include <boost/gil/extension/color/convert.hpp>
#include <boost/gil/extension/numeric/pixel_numeric_operations.hpp>
#include <boost/gil/extension/numeric/pixel_numeric_operations_assign.hpp>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace normalize {

NormalizePlugin::NormalizePlugin( OfxImageEffectHandle handle )
: ImageEffectGilPlugin( handle )
{
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
	using namespace boost::gil;
	NormalizeProcessParams<Scalar> params;

	params._mode         = static_cast<EParamMode>( _mode->getValue() );
	params._analyseMode  = static_cast<EParamAnalyseMode>( _analyseMode->getValue() );

	color_convert( ofxToGil( _srcMinColor->getValue() ), params._srcMinColor );
	color_convert( ofxToGil( _srcMaxColor->getValue() ), params._srcMaxColor );
	color_convert( ofxToGil( _dstMinColor->getValue() ), params._dstMinColor );
	color_convert( ofxToGil( _dstMaxColor->getValue() ), params._dstMaxColor );

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

		boost::scoped_ptr<OFX::Image> src( _clipSrc->fetchImage( args.time ) );
		if( ! src.get() )
			BOOST_THROW_EXCEPTION( exception::ImageNotReady() );
		if( src->getRowBytes() == 0 )
			BOOST_THROW_EXCEPTION( exception::WrongRowBytes() );
		OfxRectI srcPixelRod = _clipSrc->getPixelRod( args.time, args.renderScale );

		EParamAnalyseMode mode = static_cast<EParamAnalyseMode>( _analyseMode->getValue() );
		NoProgress progress;
		rgba32f_pixel_t min, max;

		switch( _clipSrc->getPixelComponents() )
		{
			case OFX::ePixelComponentRGBA:
			{
				switch( _clipSrc->getPixelDepth() )
				{
					case OFX::eBitDepthFloat:
					{
						typedef rgba32f_view_t View;
						typedef View::value_type Pixel;
						View srcView = getView<View>( src.get(), srcPixelRod );
						analyseInputMinMax<View>( srcView, mode, min, max, progress );
						break;
					}
					case OFX::eBitDepthUShort:
					{
						typedef rgba16_view_t View;
						typedef View::value_type Pixel;
						View srcView = getView<View>( src.get(), srcPixelRod );
						Pixel smin, smax;
						analyseInputMinMax<View>( srcView, mode, smin, smax, progress );
						color_convert(smin, min);
						color_convert(smax, max);
						break;
					}
					case OFX::eBitDepthUByte:
					{
						typedef rgba8_view_t View;
						typedef View::value_type Pixel;
						View srcView = getView<View>( src.get(), srcPixelRod );
						Pixel smin, smax;
						analyseInputMinMax<View>( srcView, mode, smin, smax, progress );
						color_convert(smin, min);
						color_convert(smax, max);
						break;
					}
					case OFX::eBitDepthNone:
					case OFX::eBitDepthCustom:
					{
						BOOST_THROW_EXCEPTION( exception::Unsupported() );
					}
				}
				break;
			}
			case OFX::ePixelComponentRGB:
			{
			}
			case OFX::ePixelComponentAlpha:
			{
			}
			case OFX::ePixelComponentCustom:
			case OFX::ePixelComponentNone:
			{
				BOOST_THROW_EXCEPTION( exception::Unsupported() );
			}
		}
		setRGBAParamValues( _srcMinColor, min );
		setRGBAParamValues( _srcMaxColor, max );

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
	OFX::EBitDepth bitDepth = _clipDst->getPixelDepth( );
	OFX::EPixelComponent components = _clipDst->getPixelComponents( );

	// do the rendering
	switch( components )
	{
		case OFX::ePixelComponentRGBA:
		{
			switch( bitDepth )
			{
				case OFX::eBitDepthFloat:
				{
					doGilRender<NormalizeProcess, false, rgba_layout_t, bits32f>( *this, args );
					return;
				}
				case OFX::eBitDepthUByte:
				case OFX::eBitDepthUShort:
				case OFX::eBitDepthCustom:
				case OFX::eBitDepthNone:
				{
					BOOST_THROW_EXCEPTION( exception::Unsupported()
						<< exception::user() + "Bit depth (" + mapBitDepthEnumToString(bitDepth) + ") not recognized by the plugin." );
				}
			}
			break;
		}
		case OFX::ePixelComponentRGB:
		case OFX::ePixelComponentAlpha:
		case OFX::ePixelComponentCustom:
		case OFX::ePixelComponentNone:
		{
			BOOST_THROW_EXCEPTION( exception::Unsupported()
				<< exception::user() + "Pixel components (" + mapPixelComponentEnumToString(components) + ") not supported by the plugin." );
		}
	}
	BOOST_THROW_EXCEPTION( exception::Unknown() );
}


}
}
}
