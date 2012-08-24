#include "InputBufferPlugin.hpp"
#include "InputBufferDefinitions.hpp"

#include <tuttle/common/ofx/imageEffect.hpp>

#include <boost/assert.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/cstdint.hpp>

namespace tuttle {
namespace plugin {
namespace inputBuffer {

namespace {
void* stringToPointer( const std::string& value )
{
	if( value.empty() )
		return NULL;
	std::ptrdiff_t p = boost::lexical_cast<std::ptrdiff_t>( value );
	return reinterpret_cast<void*>( p );
}

inline std::size_t bitDepthMemorySize( const OFX::EBitDepth e )
{
	switch( e )
	{
		case OFX::eBitDepthUByte:
			return sizeof( boost::uint8_t );
		case OFX::eBitDepthUShort:
			return sizeof( boost::uint16_t );
		case OFX::eBitDepthFloat:
			return sizeof( float );
		case OFX::eBitDepthNone:
		case OFX::eBitDepthCustom:
			return 0;
	}
	BOOST_ASSERT( false );
	return 0;
}

inline std::size_t numberOfComponents( const OFX::EPixelComponent c )
{
	switch( c )
	{
		case OFX::ePixelComponentRGBA:
			return 4;
		case OFX::ePixelComponentRGB:
			return 3;
		case OFX::ePixelComponentAlpha:
			return 1;
		case OFX::ePixelComponentNone:
			return 0;
		case OFX::ePixelComponentCustom:
			BOOST_THROW_EXCEPTION( exception::Value()
			    << exception::user() + "Can't retrieve the number of values inside a custom pixel component." );
	}
	BOOST_ASSERT( false );
	return 0;
}

}


InputBufferPlugin::InputBufferPlugin( OfxImageEffectHandle handle )
: OFX::ImageEffect( handle )
{
	_clipDst = fetchClip( kOfxImageEffectOutputClipName );

	
	_paramInputMode = fetchChoiceParam( kParamInputMode );
	_paramInputBufferPointer = fetchStringParam( kParamInputBufferPointer );
	_paramInputCallbackPointer = fetchStringParam( kParamInputCallbackPointer );
	_paramInputCallbackCustomData = fetchStringParam( kParamInputCallbackCustomData );
	
	_paramSize = fetchInt2DParam( kParamSize );
	_paramRowByteSize = fetchIntParam( kParamRowBytesSize );
	_paramPixelAspectRatio = fetchDoubleParam( kParamPixelAspectRatio );
	_paramFramerate = fetchDoubleParam( kParamFramerate );
	_paramPixelComponents = fetchChoiceParam( kParamPixelComponents );
	_paramBitDepth = fetchChoiceParam( kParamBitDepth );
	_paramField = fetchChoiceParam( kParamField );
	
	_paramTimeDomain = fetchDouble2DParam( kParamTimeDomain );
	
	changedParam( OFX::InstanceChangedArgs(), kParamInputMode );
}

InputBufferProcessParams InputBufferPlugin::getProcessParams( const OfxTime time ) const
{
	InputBufferProcessParams params;
	params._mode = static_cast<EParamInputMode>( _paramInputMode->getValue() );
	
	params._inputBuffer = static_cast<unsigned char*>( stringToPointer( _paramInputBufferPointer->getValueAtTime( time ) ) );
	params._callbackPtr = reinterpret_cast<CallbackPtr>( stringToPointer( _paramInputCallbackPointer->getValue() ) );
	params._callbackCustomDataPtr = static_cast<CallbackCustomDataPtr>( stringToPointer( _paramInputCallbackCustomData->getValue() ) );
	
	const OfxPointI imgSize = _paramSize->getValueAtTime( time );
	params._width = imgSize.x;
	params._height = imgSize.y;
	params._rowByteSize = _paramRowByteSize->getValueAtTime( time );
	
	params._pixelAspectRatio = _paramPixelAspectRatio->getValue();
	params._framerate = _paramFramerate->getValue();
	
	switch( static_cast<EParamPixelComponent>(_paramPixelComponents->getValue()) )
	{
	case eParamPixelComponentAlpha:
		params._pixelComponents = OFX::ePixelComponentAlpha;
		break;
	case eParamPixelComponentRGB:
		params._pixelComponents = OFX::ePixelComponentRGB;
		break;
	case eParamPixelComponentRGBA:
		params._pixelComponents = OFX::ePixelComponentRGBA;
		break;
	}
	switch( static_cast<EParamBitDepth>(_paramBitDepth->getValue()) )
	{
	case eParamBitDepthUByte:
		params._bitDepth = OFX::eBitDepthUByte;
		break;
	case eParamBitDepthUShort:
		params._bitDepth = OFX::eBitDepthUShort;
		break;
	case eParamBitDepthFloat:
		params._bitDepth = OFX::eBitDepthFloat;
		break;
	}
	switch( static_cast<EParamField>(_paramField->getValue()) )
	{
	case eParamFieldNone:
		params._field = OFX::eFieldNone;
		break;
	case eParamFieldBoth:
		params._field = OFX::eFieldBoth;
		break;
	case eParamFieldLower:
		params._field = OFX::eFieldLower;
		break;
	case eParamFieldUpper:
		params._field = OFX::eFieldUpper;
		break;
	}
	
	return params;
}

void InputBufferPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kParamInputMode )
	{
		const EParamInputMode mode = static_cast<EParamInputMode>( _paramInputMode->getValue() );
		
		_paramInputBufferPointer->setIsSecretAndDisabled( (mode != eParamInputModeBufferPointer) );
		_paramInputCallbackPointer->setIsSecretAndDisabled( (mode != eParamInputModeCallbackPointer) );
		_paramInputCallbackCustomData->setIsSecretAndDisabled( (mode != eParamInputModeCallbackPointer) );
	}
	else if( paramName == kParamInputBufferPointer )
	{
		_paramInputMode->setValue( eParamInputModeBufferPointer );
	}
	else if( ( paramName == kParamInputCallbackPointer ) ||
		     ( paramName == kParamInputCallbackCustomData ) )
	{
		_paramInputMode->setValue( eParamInputModeCallbackPointer );
	}
}

bool InputBufferPlugin::getTimeDomain( OfxRangeD& range )
{
	_paramTimeDomain->getValue( range.min, range.max );
	return true;
}

void InputBufferPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	OfxRangeD range;
	_paramTimeDomain->getValue( range.min, range.max );
	InputBufferProcessParams params = getProcessParams( range.min );
	
	clipPreferences.setOutputFrameVarying( params._mode == eParamInputModeCallbackPointer );
	clipPreferences.setClipComponents( *_clipDst, params._pixelComponents );
	clipPreferences.setClipBitDepth( *_clipDst, params._bitDepth );
	clipPreferences.setPixelAspectRatio( *_clipDst, params._pixelAspectRatio );
	clipPreferences.setOutputFrameRate( params._framerate );
	clipPreferences.setOutputFielding( params._field );
}

bool InputBufferPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	rod.x1 = 0;
	rod.y1 = 0;
	OfxPointI imgSize = _paramSize->getValueAtTime( args.time );
	rod.x2 = imgSize.x;
	rod.y2 = imgSize.y;
	return true;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void InputBufferPlugin::render( const OFX::RenderArguments &args )
{
	
	// @todo: add an OpenFX extension to setup the image buffer pointer
	//        from the plugin.
	// If the host support this extension, we only need to set the pointer,
	// else we need to do a buffer copy.
	
//	if()
//	{
//		// Set the buffer pointer
//	}
//	else
	{
		// Buffer Copy
		
		// fetch the destination image
		boost::scoped_ptr<OFX::Image> dst( _clipDst->fetchImage( args.time ) );
		if( !dst.get() )
			BOOST_THROW_EXCEPTION( exception::ImageNotReady()
				<< exception::dev() + "Error on clip " + quotes(_clipDst->name()) );
		if( dst->getRowDistanceBytes() == 0 )
			BOOST_THROW_EXCEPTION( exception::WrongRowBytes()
				<< exception::dev() + "Error on clip " + quotes(_clipDst->name()) );

		// dstPixelRod = dst->getRegionOfDefinition(); // bug in nuke, returns bounds
		OfxRectI dstPixelRod = _clipDst->getPixelRod( args.time, args.renderScale );
		OfxPointI dstPixelRodSize;
		dstPixelRodSize.x = ( dstPixelRod.x2 - dstPixelRod.x1 );
		dstPixelRodSize.y = ( dstPixelRod.y2 - dstPixelRod.y1 );

		// User parameters
		InputBufferProcessParams params = getProcessParams( args.time );
		
		unsigned char* inputImageBufferPtr = NULL;
		TUTTLE_TCOUT_INFOS;
		switch( params._mode )
		{
			case eParamInputModeBufferPointer:
			{
				inputImageBufferPtr = params._inputBuffer;
				TUTTLE_TCOUT_INFOS;
				break;
			}
			case eParamInputModeCallbackPointer:
			{
				TUTTLE_TCOUT_INFOS;
				if( params._callbackPtr )
				{
					TUTTLE_TCOUT_INFOS;
					TUTTLE_TCOUT_VAR( (void*)params._callbackPtr );
					TUTTLE_TCOUT_VAR( (void*)params._callbackCustomDataPtr );
					inputImageBufferPtr = static_cast<unsigned char*>( params._callbackPtr( args.time, params._callbackCustomDataPtr ) );
				}
				break;
			}
		}
		TUTTLE_TCOUT_INFOS;
		TUTTLE_TCOUT_VAR( (void*)inputImageBufferPtr );
		
		int rowBytesDistanceSize = params._rowByteSize;
		const std::size_t nbComponents = numberOfComponents( params._pixelComponents );
		const std::size_t bitDepthMemSize = bitDepthMemorySize( params._bitDepth );
		int widthBytesSize = dstPixelRodSize.x * nbComponents * bitDepthMemSize;
		if( rowBytesDistanceSize == 0 )
			rowBytesDistanceSize = widthBytesSize;
		
		// Copy the image
		for( int y = 0; y < dstPixelRodSize.y; ++y )
		{
			memcpy( dst->getPixelAddress( 0, y ), inputImageBufferPtr + y * rowBytesDistanceSize, widthBytesSize );
		}
	}
}


}
}
}
