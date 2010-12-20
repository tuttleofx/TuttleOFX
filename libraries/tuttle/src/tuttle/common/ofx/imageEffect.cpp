#include "imageEffect.hpp"

#include <tuttle/common/exceptions.hpp>

#include <ofxCore.h>

#include <boost/lexical_cast.hpp>

namespace tuttle {
namespace ofx {
namespace imageEffect {

/** @brief map a std::string to a context */
EContext mapContextStringToEnum( const std::string& s )
{
	if( s == kOfxImageEffectContextGenerator )
		return eContextGenerator;
	if( s == kOfxImageEffectContextFilter )
		return eContextFilter;
	if( s == kOfxImageEffectContextTransition )
		return eContextTransition;
	if( s == kOfxImageEffectContextPaint )
		return eContextPaint;
	if( s == kOfxImageEffectContextGeneral )
		return eContextGeneral;
	if( s == kOfxImageEffectContextRetimer )
		return eContextRetimer;
	if( s == kOfxImageEffectContextReader )
		return eContextReader;
	if( s == kOfxImageEffectContextWriter )
		return eContextWriter;
	BOOST_THROW_EXCEPTION( exception::Value()
	    << exception::dev() + s );
	return eContextGeneral;
}

/** @brief map a std::string to a context */
const std::string mapContextEnumToString( const EContext s )
{
	switch( s )
	{
		case eContextGenerator:
			return kOfxImageEffectContextGenerator;
		case eContextFilter:
			return kOfxImageEffectContextFilter;
		case eContextTransition:
			return kOfxImageEffectContextTransition;
		case eContextPaint:
			return kOfxImageEffectContextPaint;
		case eContextGeneral:
			return kOfxImageEffectContextGeneral;
		case eContextRetimer:
			return kOfxImageEffectContextRetimer;
		case eContextReader:
			return kOfxImageEffectContextReader;
		case eContextWriter:
			return kOfxImageEffectContextWriter;
		case eContextNone:
			return "ContextNone...";
	}
	BOOST_THROW_EXCEPTION( exception::Value()
	    << exception::dev() + "Unknown image effect context enum." );
	return "";
}

//const std::string mapMessageTypeEnumToString( const OFX::Message::EMessageType type )
//{
//	if( type == OFX::Message::eMessageFatal )
//		return kOfxMessageFatal;
//	else if( type == OFX::Message::eMessageError )
//		return kOfxMessageError;
//	else if( type == OFX::Message::eMessageMessage )
//		return kOfxMessageMessage;
//	else if( type == OFX::Message::eMessageLog )
//		return kOfxMessageLog;
//	else if( type == OFX::Message::eMessageQuestion )
//		return kOfxMessageQuestion;
//	BOOST_THROW_EXCEPTION( exception::Value()
//		<< exception::dev() + type );
//	return kOfxMessageError;
//}
//
//OFX::Message::EMessageReply mapMessageReplyStatusToEnum( const OfxStatus stat )
//{
//	if( stat == kOfxStatOK )
//		return OFX::Message::eMessageReplyOK;
//	else if( stat == kOfxStatReplyYes )
//		return OFX::Message::eMessageReplyYes;
//	else if( stat == kOfxStatReplyNo )
//		return OFX::Message::eMessageReplyNo;
//	else if( stat == kOfxStatFailed )
//		return OFX::Message::eMessageReplyFailed;
//	BOOST_THROW_EXCEPTION( exception::Value()
//		<< exception::dev() + stat );
//	return OFX::Message::eMessageReplyFailed;
//}
//
//OfxStatus mapMessageReplyEnumToStatus( const OFX::Message::EMessageReply stat )
//{
//	switch( stat )
//	{
//		case OFX::Message::eMessageReplyOK:
//			return kOfxStatOK;
//		case OFX::Message::eMessageReplyYes:
//			return kOfxStatReplyYes;
//		case OFX::Message::eMessageReplyNo:
//			return kOfxStatReplyNo;
//		case OFX::Message::eMessageReplyFailed:
//				return kOfxStatFailed;
//	}
//	BOOST_THROW_EXCEPTION( exception::Value()
//		<< exception::dev() + stat );
//	return kOfxStatFailed;
//}
//
///** @brief map a std::string to a context */
//InstanceChangeReason mapInstanceChangedReasonStringToEnum( const std::string& s )
//{
//	if( s == kOfxChangePluginEdited )
//		return eChangePluginEdit;
//	if( s == kOfxChangeUserEdited )
//		return eChangeUserEdit;
//	if( s == kOfxChangeTime )
//		return eChangeTime;
//	BOOST_THROW_EXCEPTION( exception::Value()
//		<< exception::dev() + s );
//	return eChangePluginEdit;
//}

/** @brief turns a bit depth string into and enum */
EBitDepth mapBitDepthStringToEnum( const std::string& str )
{
	if( str == kOfxBitDepthByte )
	{
		return eBitDepthUByte;
	}
	else if( str == kOfxBitDepthShort )
	{
		return eBitDepthUShort;
	}
	else if( str == kOfxBitDepthFloat )
	{
		return eBitDepthFloat;
	}
	else if( str == kOfxBitDepthNone )
	{
		return eBitDepthNone;
	}
	else
	{
		return eBitDepthCustom;
	}
}

const std::string mapBitDepthEnumToString( const EBitDepth e )
{
	switch( e )
	{
		case eBitDepthUByte:
			return kOfxBitDepthByte;
		case eBitDepthUShort:
			return kOfxBitDepthShort;
		case eBitDepthFloat:
			return kOfxBitDepthFloat;
		case eBitDepthNone:
			return kOfxBitDepthNone;
		case eBitDepthCustom:
			return "eBitDepthCustom";
	}
	BOOST_THROW_EXCEPTION( exception::Value()
	    << exception::dev() + "BitDepth enum: " + e );
	return kOfxBitDepthNone;
}

/** @brief turns a pixel component string into and enum */
EPixelComponent mapPixelComponentStringToEnum( const std::string& str )
{
	if( str == kOfxImageComponentRGBA )
	{
		return ePixelComponentRGBA;
	}
	else if( str == kOfxImageComponentRGB )
	{
		return ePixelComponentRGBA;
	}
	else if( str == kOfxImageComponentAlpha )
	{
		return ePixelComponentAlpha;
	}
	else if( str == kOfxImageComponentNone )
	{
		return ePixelComponentNone;
	}
	else
	{
		return ePixelComponentCustom;
	}
}

std::string mapPixelComponentEnumToString( const EPixelComponent e )
{
	switch( e )
	{
		case ePixelComponentRGBA:
			return kOfxImageComponentRGBA;
		case ePixelComponentRGB:
			return kOfxImageComponentRGB;
		case ePixelComponentAlpha:
			return kOfxImageComponentAlpha;
		case ePixelComponentNone:
			return kOfxImageComponentNone;
		case ePixelComponentCustom:
			return "ePixelComponentCustom";
	}
	BOOST_THROW_EXCEPTION( exception::Value()
	    << exception::user() + "PixelComponent enum: " + e );
}

///** @brief turns a premultiplication string into and enum */
//EPreMultiplication mapPreMultiplicationStringToEnum( const std::string& str )
//{
//	if( str == kOfxImageOpaque )
//	{
//		return eImageOpaque;
//	}
//	else if( str == kOfxImagePreMultiplied )
//	{
//		return eImagePreMultiplied;
//	}
//	else if( str == kOfxImageUnPreMultiplied )
//	{
//		return eImageUnPreMultiplied;
//	}
//	BOOST_THROW_EXCEPTION( exception::Value()
//		<< exception::user() + str );
//	return eImageOpaque;
//}
//
//std::string mapPreMultiplicationEnumToString( const EPreMultiplication e )
//{
//	switch( e )
//	{
//		case eImageOpaque:
//			return kOfxImageOpaque;
//		case eImagePreMultiplied:
//			return kOfxImagePreMultiplied;
//		case eImageUnPreMultiplied:
//			return kOfxImageUnPreMultiplied;
//	}
//	BOOST_THROW_EXCEPTION( exception::Value()
//		<< exception::user() + "PreMultiplication enum: " + e );
//	return "";
//}
//
///** @brief turns a field string into and enum */
//EField mapFieldStringToEnum( const std::string& str )
//{
//	if( str == kOfxImageFieldNone )
//	{
//		return eFieldNone;
//	}
//	else if( str == kOfxImageFieldBoth )
//	{
//		return eFieldBoth;
//	}
//	else if( str == kOfxImageFieldLower )
//	{
//		return eFieldLower;
//	}
//	else if( str == kOfxImageFieldUpper )
//	{
//		return eFieldUpper;
//	}
//	BOOST_THROW_EXCEPTION( exception::Value()
//		<< exception::user() + str );
//	return eFieldNone;
//}
//
//std::string mapFieldEnumToString( const EField e )
//{
//	switch( e )
//	{
//		case eFieldNone:
//			return kOfxImageFieldNone;
//		case eFieldBoth:
//			return kOfxImageFieldBoth;
//		case eFieldLower:
//			return kOfxImageFieldLower;
//		case eFieldUpper:
//			return kOfxImageFieldUpper;
//	}
//	BOOST_THROW_EXCEPTION( exception::Value()
//		<< exception::user() + e );
//	return "";
//}

/// get me deepest bit depth
std::string findDeepestBitDepth( const std::string& s1, const std::string& s2 )
{
	if( s1 == kOfxBitDepthNone )
	{
		return s2;
	}
	else if( s1 == kOfxBitDepthByte )
	{
		if( s2 == kOfxBitDepthShort || s2 == kOfxBitDepthFloat )
			return s2;
		return s1;
	}
	else if( s1 == kOfxBitDepthShort )
	{
		if( s2 == kOfxBitDepthFloat )
			return s2;
		return s1;
	}
	else if( s1 == kOfxBitDepthFloat )
	{
		return s1;
	}
	else
	{
		BOOST_THROW_EXCEPTION( exception::Value()
		    << exception::user() + "Unrecognized bitdepth " + quotes( s1 ) + "."
		    << exception::dev() + "We can't find the deepest bit depth between " + quotes( s1 ) + " and " + quotes( s2 ) );
		return s2;
	}
}

}
}
}

