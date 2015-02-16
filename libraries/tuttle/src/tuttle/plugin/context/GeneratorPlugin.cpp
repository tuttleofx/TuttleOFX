#include "GeneratorPlugin.hpp"
#include <boost/numeric/conversion/cast.hpp>

namespace tuttle {
namespace plugin {

using boost::numeric_cast;

GeneratorPlugin::GeneratorPlugin( OfxImageEffectHandle handle )
	: OFX::ImageEffect( handle )
{
	_clipSrc                = fetchClip         ( kOfxImageEffectSimpleSourceClipName );
	_clipDst                = fetchClip         ( kOfxImageEffectOutputClipName );
	_paramExplicitConv      = fetchChoiceParam  ( kParamGeneratorExplicitConversion );
	_paramComponents        = fetchChoiceParam  ( kTuttlePluginChannel );

	_paramMode              = fetchChoiceParam  ( kParamMode );
	_paramFormat            = fetchChoiceParam  ( kParamFormat );
	_paramSize              = fetchInt2DParam   ( kParamSize );
	_paramSizeWidth         = fetchIntParam     ( kParamSizeWidth );
	_paramSizeHeight        = fetchIntParam     ( kParamSizeHeight );
	_paramSizeOrientation   = fetchChoiceParam  ( kParamSizeOrientation );
	_paramSizeSpecificRatio = fetchBooleanParam ( kParamSizeSpecificRatio );
	_paramSizeRatioValue    = fetchDoubleParam  ( kParamSizeRatioValue );

	updateVisibleTools();
}

void GeneratorPlugin::updateVisibleTools()
{
	OFX::InstanceChangedArgs args( this->timeLineGetTime() );
	changedParam( args, kParamMode );
	//changedParam( args, kParamSizeKeepRatio );
	changedParam( args, kParamSizeOrientation );
}

GeneratorPlugin::~GeneratorPlugin()
{}

void GeneratorPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kParamMode )
	{
		switch( _paramMode->getValue() )
		{
			case eParamModeFormat:
			{
				_paramSize              -> setIsSecretAndDisabled( true );
				_paramSizeWidth         -> setIsSecretAndDisabled( true );
				_paramSizeHeight        -> setIsSecretAndDisabled( true );
				_paramSizeOrientation   -> setIsSecretAndDisabled( true );
				_paramSizeSpecificRatio -> setIsSecretAndDisabled( true );
				_paramSizeRatioValue    -> setIsSecretAndDisabled( true );

				_paramFormat            -> setIsSecretAndDisabled( false );
				break;
			}
			case eParamModeSize:
			{
				const bool                  specificRatio   = _paramSizeSpecificRatio->getValue();
				const EParamSizeOrientation orientation = static_cast<EParamSizeOrientation>(_paramSizeOrientation->getValue());

				_paramFormat            -> setIsSecretAndDisabled( true );
				_paramSizeSpecificRatio -> setIsSecretAndDisabled( false );

				_paramSizeWidth         -> setIsSecretAndDisabled( ! specificRatio || orientation != eParamSizeOrientationX );
				_paramSizeHeight        -> setIsSecretAndDisabled( ! specificRatio || orientation != eParamSizeOrientationY );
				_paramSizeOrientation   -> setIsSecretAndDisabled( ! specificRatio );
				_paramSizeRatioValue    -> setIsSecretAndDisabled( ! specificRatio );

				_paramSize              -> setIsSecretAndDisabled( specificRatio );
				break;
			}
		}
	}
	else if( paramName == kParamFormat && args.reason == OFX::eChangeUserEdit )
	{
		std::size_t width  = 0;
		std::size_t height = 0;
		getFormatResolution( static_cast<EParamFormat>(_paramFormat->getValue()), width, height );

		_paramMode            -> setValue( eParamModeFormat );
		_paramSize            -> setValue( numeric_cast<int>(width), numeric_cast<int>(height) );
		_paramSizeWidth       -> setValue( numeric_cast<int>(width) );
		_paramSizeHeight      -> setValue( numeric_cast<int>(height) );
	}
	else if( paramName == kParamSize && args.reason == OFX::eChangeUserEdit )
	{
		const OfxPointI s = _paramSize->getValue();

		_paramMode->setValue( eParamModeSize );
		_paramSizeWidth       -> setValue( s.x );
		_paramSizeHeight      -> setValue( s.y );
	}
	else if( paramName == kParamSizeSpecificRatio && args.reason == OFX::eChangeUserEdit )
	{
		const bool                  specificRatio   = _paramSizeSpecificRatio->getValue();
		const EParamSizeOrientation orientation = static_cast<EParamSizeOrientation>(_paramSizeOrientation->getValue());

		_paramSizeWidth       -> setIsSecretAndDisabled( ! specificRatio || orientation != eParamSizeOrientationX );
		_paramSizeHeight      -> setIsSecretAndDisabled( ! specificRatio || orientation != eParamSizeOrientationY );
		_paramSizeOrientation -> setIsSecretAndDisabled( ! specificRatio );
		_paramSizeRatioValue  -> setIsSecretAndDisabled( ! specificRatio );
		_paramSize            -> setIsSecretAndDisabled( specificRatio );
	}
	else if( paramName == kParamSizeOrientation && args.reason == OFX::eChangeUserEdit )
	{
		const bool                  specificRatio   = _paramSizeSpecificRatio->getValue();
		const EParamSizeOrientation orientation = static_cast<EParamSizeOrientation>(_paramSizeOrientation->getValue());

		_paramSizeWidth       -> setIsSecretAndDisabled( ! specificRatio || orientation != eParamSizeOrientationX );
		_paramSizeHeight      -> setIsSecretAndDisabled( ! specificRatio || orientation != eParamSizeOrientationY );
	}
	else if( paramName == kParamSizeWidth && args.reason == OFX::eChangeUserEdit )
	{
		_paramMode              -> setValue( eParamModeSize );
		_paramSizeSpecificRatio -> setValue( true );
		_paramSizeRatioValue    -> setIsSecretAndDisabled( false );
		_paramSizeOrientation   -> setValue( eParamSizeOrientationX );

		_paramSize              -> setValue( _paramSizeWidth->getValue(), _paramSize->getValue().y );
	}
	else if( paramName == kParamSizeHeight && args.reason == OFX::eChangeUserEdit )
	{
		_paramMode              -> setValue( eParamModeSize );
		_paramSizeSpecificRatio -> setValue( true );
		_paramSizeRatioValue    -> setIsSecretAndDisabled( false );
		_paramSizeOrientation   -> setValue( eParamSizeOrientationY );

		_paramSize              -> setValue( _paramSize->getValue().x, _paramSizeHeight->getValue() );
	}
}

void GeneratorPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	clipPreferences.setOutputFrameVarying( true );

	switch( getExplicitConversion() )
	{
		case eParamGeneratorExplicitConversionByte:
		{
			clipPreferences.setClipBitDepth( *this->_clipDst, OFX::eBitDepthUByte );
			break;
		}
		case eParamGeneratorExplicitConversionShort:
		{
			clipPreferences.setClipBitDepth( *this->_clipDst, OFX::eBitDepthUShort );
			break;
		}
		case eParamGeneratorExplicitConversionFloat:
		{
			clipPreferences.setClipBitDepth( *this->_clipDst, OFX::eBitDepthFloat );
			break;
		}
		case eParamGeneratorExplicitConversionAuto:
		{
			if( _clipSrc->isConnected() )
			{
				switch( _clipSrc->getPixelDepth() )
				{
					case OFX::eBitDepthUByte:
						clipPreferences.setClipBitDepth( *this->_clipDst, OFX::eBitDepthUByte );
						break;
					case OFX::eBitDepthUShort:
						clipPreferences.setClipBitDepth( *this->_clipDst, OFX::eBitDepthUShort );
						break;
					case OFX::eBitDepthFloat:
						clipPreferences.setClipBitDepth( *this->_clipDst, OFX::eBitDepthFloat );
						break;
					case OFX::eBitDepthCustom:
					case OFX::eBitDepthNone:
						break;
				}
			}
			else
			{
				clipPreferences.setClipBitDepth( *this->_clipDst, OFX::eBitDepthFloat );
			}
		}
	}

	if( _clipSrc->isConnected() )
	{
		switch( _clipSrc->getPixelComponents() )
		{
			case OFX::ePixelComponentAlpha:
				clipPreferences.setClipComponents( *this->_clipDst, OFX::ePixelComponentAlpha );
				break;
			case OFX::ePixelComponentRGB:
				clipPreferences.setClipComponents( *this->_clipDst, OFX::ePixelComponentRGB );
				break;
			case OFX::ePixelComponentRGBA:
				clipPreferences.setClipComponents( *this->_clipDst, OFX::ePixelComponentRGBA );
				break;
			default: break;
		}
		clipPreferences.setPixelAspectRatio( *this->_clipDst, _clipSrc->getPixelAspectRatio() );
	}
	else
	{
		switch( _paramComponents->getValue() )
		{
			case eParamGeneratorComponentsAlpha:
			{
				clipPreferences.setClipComponents( *this->_clipDst, OFX::ePixelComponentAlpha );
				break;
			}
			case eParamGeneratorComponentsRGB:
			{
				clipPreferences.setClipComponents( *this->_clipDst, OFX::ePixelComponentRGB );
				break;
			}
			case eParamGeneratorComponentsRGBA:
			{
				clipPreferences.setClipComponents( *this->_clipDst, OFX::ePixelComponentRGBA );
				break;
			}
		}
		clipPreferences.setPixelAspectRatio( *this->_clipDst, 1.0 );
	}
}

bool GeneratorPlugin::getTimeDomain( OfxRangeD& range )
{
	if( _clipSrc->isConnected() )
		return false;
	range.min = kOfxFlagInfiniteMin;
	range.max = kOfxFlagInfiniteMax;
	return true;
}

bool GeneratorPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	using namespace boost::gil;

	if( _clipSrc->isConnected() )
	{
		using namespace boost::gil;
	
		const OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
		
		rod = srcRod;
		return true;
	}
	
	switch(_paramMode->getValue())
	{
		case eParamModeFormat :
		{
			std::size_t width = 0;
			std::size_t height = 0;
			getFormatResolution( static_cast<EParamFormat>(_paramFormat->getValue()), width, height );
			rod.x1 = 0;
			rod.y1 = 0;
			rod.x2 = width > 0 ? width : 0;
			rod.y2 = height > 0 ? height : 0;

			return true;
		}
		case eParamModeSize :
		{
			std::size_t sizex = 0;
			std::size_t sizey = 0;
			if( _paramSizeSpecificRatio->getValue() )
			{
				if( _paramSizeOrientation->getValue() == eParamSizeOrientationX )
				{
					sizex   = _paramSizeWidth->getValue();
					sizey   = sizex / _paramSizeRatioValue->getValue();
				}
				else // direction == eParamSizeY
				{
					sizey   = _paramSizeHeight->getValue();
					sizex   = _paramSizeRatioValue->getValue() * sizey;
				}
			}
			else
			{
				const OfxPointI s = _paramSize->getValue();
				sizex = s.x;
				sizey = s.y;
			}

			rod.x1 = 0;
			rod.y1 = 0;
			rod.x2 = sizex > 0 ? sizex : 0;
			rod.y2 = sizey > 0 ? sizey : 0;

			return true;
		}
	}

	return false;
}

}
}
