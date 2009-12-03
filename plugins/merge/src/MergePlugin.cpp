/**
 * @file MergePlugin.cpp
 * @brief Merging ofx plugin
 * @author Eloi Du Bois
 * @date    27/11/09 13:42
 *
 */

#include "MergePlugin.hpp"
#include "MergeProcess.hpp"
#include "MergeDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace merge {

using namespace boost::gil;

MergePlugin::MergePlugin( OfxImageEffectHandle handle )
	: ImageEffect( handle )
{
	_mergeFunction = fetchChoiceParam( kMergeFunction );
	_srcClipA = fetchClip( kMergeSourceA );
	_srcClipB = fetchClip( kMergeSourceB );
	_dstClip  = fetchClip( kOfxImageEffectOutputClipName );
	assert( _mergeFunction && _srcClipA && _srcClipB && _dstClip );
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void MergePlugin::render( const OFX::RenderArguments& args )
{
	bool isGray = _dstClip->getPixelComponents() == OFX::ePixelComponentAlpha;
	if (isGray) {
		switch( (EMergeFunction)_mergeFunction->getValue() )
		{
			// Functions that doesn't need alpha
			case eMergeFunctionAverage:
			{
				renderGray<FunctorAverage>( args );
				break;
			}
			case eMergeFunctionCopy:
			{
				break;
			}
			case eMergeFunctionDifference:
			{
				break;
			}
			case eMergeFunctionDivide:
			{
				break;
			}
			case eMergeFunctionExclusion:
			{
				break;
			}
			case eMergeFunctionFrom:
			{
				break;
			}
			case eMergeFunctionGeometric:
			{
				break;
			}
			case eMergeFunctionHardLight:
			{
				break;
			}
			case eMergeFunctionHypot:
			{
				break;
			}
			case eMergeFunctionMax:
			{
				break;
			}
			case eMergeFunctionMin:
			{
				break;
			}
			case eMergeFunctionMinus:
			{
				break;
			}
			case eMergeFunctionMultiply:
			{
				break;
			}
			case eMergeFunctionOverlay:
			{
				break;
			}
			case eMergeFunctionPlus:
			{
				renderGray<FunctorPlus>( args );
				break;
			}
			case eMergeFunctionScreen:
			{
				break;
			}
			default:
				COUT_ERROR("Unsupported operation !");
				break;
		}
	} else {
		switch( (EMergeFunction)_mergeFunction->getValue() )
		{
			// Functions that need alpha
			case eMergeFunctionATop:
			{
				renderRGBA<FunctorATop>( args );
				break;
			}
			case eMergeFunctionConjointOver:
			{
				break;
			}
			case eMergeFunctionColorBurn:
			{
				break;
			}
			case eMergeFunctionColorDodge:
			{
				break;
			}
			case eMergeFunctionDisjointOver:
			{
				break;
			}
			case eMergeFunctionIn:
			{
				break;
			}
			case eMergeFunctionMask:
			{
				break;
			}
			case eMergeFunctionMatte:
			{
				break;
			}
			case eMergeFunctionOut:
			{
				break;
			}
			case eMergeFunctionOver:
			{
				break;
			}
			case eMergeFunctionStencil:
			{
				break;
			}
			case eMergeFunctionUnder:
			{
				break;
			}
			case eMergeFunctionXOR:
			{
				break;
			}
			// Function that doesn't needs alpha
			case eMergeFunctionAverage:
			{
				renderRGBA<FunctorAverage>( args );
				break;
			}
			case eMergeFunctionCopy:
			{
				break;
			}
			case eMergeFunctionDifference:
			{
				break;
			}
			case eMergeFunctionDivide:
			{
				break;
			}
			case eMergeFunctionExclusion:
			{
				break;
			}
			case eMergeFunctionFrom:
			{
				break;
			}
			case eMergeFunctionGeometric:
			{
				break;
			}
			case eMergeFunctionHardLight:
			{
				break;
			}
			case eMergeFunctionHypot:
			{
				break;
			}
			case eMergeFunctionMax:
			{
				break;
			}
			case eMergeFunctionMin:
			{
				break;
			}
			case eMergeFunctionMinus:
			{
				break;
			}
			case eMergeFunctionMultiply:
			{
				break;
			}
			case eMergeFunctionOverlay:
			{
				break;
			}
			case eMergeFunctionPlus:
			{
				renderRGBA<FunctorPlus>( args );
				break;
			}
			case eMergeFunctionScreen:
			{
				break;
			}
			default:
				COUT_ERROR("Unsupported operation !");
				break;
		}

	}
}

template<class Functor>
void MergePlugin::renderGray( const OFX::RenderArguments& args )
{
	assert( _dstClip );
	/*
	// instantiate the render code based on the pixel depth of the dst clip
	OFX::BitDepthEnum dstBitDepth = _dstClip->getPixelDepth();
	switch( dstBitDepth )
	{
		case OFX::eBitDepthUByte:
		{
			MergeProcess<gray8_view_t, Functor> fred( *this );
			fred.setupAndProcess( args );
			break;
		}
		case OFX::eBitDepthUShort:
		{
			MergeProcess<gray16_view_t, Functor> fred( *this );
			fred.setupAndProcess( args );
			break;
		}
		case OFX::eBitDepthFloat:
		{
			MergeProcess<gray32f_view_t, Functor> fred( *this );
			fred.setupAndProcess( args );
			break;
		}
		case OFX::eBitDepthCustom:
		case OFX::eBitDepthNone:
			COUT_FATALERROR( "BitDepthNone not recognize." );
			return;
	}
	*/
}

template<class Functor>
void MergePlugin::renderRGBA( const OFX::RenderArguments& args )
{
	assert( _dstClip );
	OFX::BitDepthEnum dstBitDepth = _dstClip->getPixelDepth();

	// do the rendering
	switch( dstBitDepth )
	{
		case OFX::eBitDepthUByte:
		{
			MergeProcess<rgba8_view_t, Functor> fred( *this );
			fred.setupAndProcess( args );
			break;
		}
		case OFX::eBitDepthUShort:
		{
			MergeProcess<rgba16_view_t, Functor> fred( *this );
			fred.setupAndProcess( args );
			break;
		}
		case OFX::eBitDepthFloat:
		{
			MergeProcess<rgba32f_view_t, Functor> fred( *this );
			fred.setupAndProcess( args );
			break;
		}
		case OFX::eBitDepthCustom:
		case OFX::eBitDepthNone:
			COUT_FATALERROR( "BitDepthNone not recognize." );
			return;
	}
}


void MergePlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kMergeHelpButton )
	{
		sendMessage( OFX::Message::eMessageMessage,
		             "", // No XML resources
		             kMergeHelpString );
	}
}

}
}
}
