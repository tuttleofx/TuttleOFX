#include "InputBufferPlugin.hpp"
#include "InputBufferDefinitions.hpp"

#include <pointerParam.hpp>

#include <tuttle/common/ofx/imageEffect.hpp>

#include <boost/assert.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/cstdint.hpp>

namespace tuttle {
namespace plugin {
namespace inputBuffer {

using namespace memoryBuffer;

namespace {
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
	_paramInputCallbackCustomData = fetchStringParam( kParamInputCustomData );
	_paramInputCallbackDestroyCustomData = fetchStringParam( kParamInputCallbackDestroyCustomData );
	
	_paramSize = fetchInt2DParam( kParamSize );
	_paramRowByteSize = fetchIntParam( kParamRowBytesSize );
	_paramPixelAspectRatio = fetchDoubleParam( kParamPixelAspectRatio );
	_paramFramerate = fetchDoubleParam( kParamFramerate );
	_paramPixelComponents = fetchChoiceParam( kParamPixelComponents );
	_paramBitDepth = fetchChoiceParam( kParamBitDepth );
	_paramField = fetchChoiceParam( kParamField );
	
	_paramTimeDomain = fetchDouble2DParam( kParamTimeDomain );
	
	// init temporary values
	_callbackMode_time = 0;
	_callbackMode_imgSize.x = 0;
	_callbackMode_imgSize.y = 0;
	_callbackMode_rowSizeBytes = 0;
	_callbackMode_imgPointer = NULL;
	
	changedParam( OFX::InstanceChangedArgs(), kParamInputMode );
}

InputBufferPlugin::~InputBufferPlugin()
{
}

InputBufferProcessParams InputBufferPlugin::getProcessParams( const OfxTime time ) const
{
	InputBufferProcessParams params;
	params._mode = static_cast<EParamInputMode>( _paramInputMode->getValue() );
	
	params._inputBuffer = static_cast<unsigned char*>( stringToPointer( _paramInputBufferPointer->getValueAtTime( time ) ) );
	params._callbackPtr = reinterpret_cast<CallbackInputImagePtr>( stringToPointer( _paramInputCallbackPointer->getValue() ) );
	params._customDataPtr = static_cast<CustomDataPtr>( stringToPointer( _paramInputCallbackCustomData->getValue() ) );
	params._callbackDestroyPtr = reinterpret_cast<CallbackDestroyCustomDataPtr>( stringToPointer( _paramInputCallbackDestroyCustomData->getValue() ) );
	
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
		     ( paramName == kParamInputCustomData ) )
	{
		_paramInputMode->setValue( eParamInputModeCallbackPointer );
	}
	else if( paramName == kParamInputCustomData )
	{
		InputBufferProcessParams params = getProcessParams(args.time);
		if( params._callbackDestroyPtr != NULL && _tempStoreCustomDataPtr != NULL )
			params._callbackDestroyPtr( _tempStoreCustomDataPtr );
		_tempStoreCustomDataPtr = static_cast<CustomDataPtr>( stringToPointer( _paramInputCallbackCustomData->getValue() ) );
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
	InputBufferProcessParams params = getProcessParams( args.time );
	switch( params._mode )
	{
		case eParamInputModeBufferPointer:
		{
			rod.x1 = 0;
			rod.y1 = 0;
			OfxPointI imgSize = _paramSize->getValueAtTime( args.time );
			rod.x2 = imgSize.x;
			rod.y2 = imgSize.y;
			break;
		}
		case eParamInputModeCallbackPointer:
		{
			callbackMode_updateImage( args.time, params );
			rod.x1 = 0;
			rod.y1 = 0;
			rod.x2 = _callbackMode_imgSize.x;
			rod.y2 = _callbackMode_imgSize.y;
			break;
		}
	}
	return true;
}

void InputBufferPlugin::callbackMode_updateImage( const OfxTime time, const InputBufferProcessParams& params )
{
	if( _callbackMode_time == time && _callbackMode_imgPointer != NULL )
		return;
	
	if( ! params._callbackPtr )
		return;
	
	_callbackMode_time = time;
	params._callbackPtr(
			time, params._customDataPtr,
			(void**)&(_callbackMode_imgPointer),
			&_callbackMode_imgSize.x,
			&_callbackMode_imgSize.y,
			&_callbackMode_rowSizeBytes
		);
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void InputBufferPlugin::render( const OFX::RenderArguments &args )
{
	// @todo: add an OpenFX extension to setup the image buffer pointer
	//        from the plugin.
	// If the host supports this extension, we only need to set the pointer,
	// else we need to do a buffer copy.

	// User parameters
	InputBufferProcessParams params = getProcessParams( args.time );
	

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

		unsigned char* inputImageBufferPtr = NULL;
		int rowBytesDistanceSize = 0;
		switch( params._mode )
		{
			case eParamInputModeBufferPointer:
			{
				inputImageBufferPtr = params._inputBuffer;
				rowBytesDistanceSize = params._rowByteSize;
				break;
			}
			case eParamInputModeCallbackPointer:
			{
				callbackMode_updateImage( args.time, params );
				inputImageBufferPtr = _callbackMode_imgPointer;
				rowBytesDistanceSize = _callbackMode_rowSizeBytes;
				break;
			}
		}
//		TUTTLE_TCOUT_VAR( (void*)inputImageBufferPtr );

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
		
		switch( params._mode )
		{
			case eParamInputModeCallbackPointer:
			{
				// We duplicated the image buffer to a buffer allocated by the host.
				// Now we can destroy the customData.
				if( params._callbackDestroyPtr != NULL )
					params._callbackDestroyPtr( params._customDataPtr );
				_callbackMode_imgPointer = NULL;
				break;
			}
			case eParamInputModeBufferPointer:
				break;
		}
	}
}


}
}
}
