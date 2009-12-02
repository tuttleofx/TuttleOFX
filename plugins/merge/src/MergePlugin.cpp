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
