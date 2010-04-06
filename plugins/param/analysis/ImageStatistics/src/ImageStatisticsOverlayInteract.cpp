#include "ImageStatisticsOverlayInteract.hpp"
#include "ImageStatisticsDefinitions.hpp"
#include "ImageStatisticsPlugin.hpp"
#include <tuttle/plugin/interact/ParamRectangleInClip.hpp>
#include <tuttle/common/opengl/gl.h>
#include <tuttle/plugin/interact/interact.hpp>
#include <tuttle/plugin/interact/overlay.hpp>

#include <ofxsImageEffect.h>
#include <ofxsInteract.h>
#include <boost/gil/gil_all.hpp>

#include <vector>

namespace tuttle {
namespace plugin {
namespace imageStatistics {

ImageStatisticsOverlayInteract::ImageStatisticsOverlayInteract( OfxInteractHandle handle, OFX::ImageEffect *effect )
: OFX::OverlayInteract( handle )
, _infos( effect )
, _interactScene( *effect , _infos )
{
	_effect = effect;
	_plugin = static_cast<ImageStatisticsPlugin*>( _effect );

    _srcClip = _plugin->fetchClip( kOfxImageEffectSimpleSourceClipName );

	_cornerA = _plugin->fetchDouble2DParam( kCornerA );
	_cornerB = _plugin->fetchDouble2DParam( kCornerB );
	_chooseOutput = _plugin->fetchChoiceParam( kChooseOutput );
	_outputAverage = _plugin->fetchRGBAParam( kOutputAverage );
	_outputChannelMin = _plugin->fetchRGBAParam( kOutputChannelMin );
	_outputChannelMax = _plugin->fetchRGBAParam( kOutputChannelMax );
	_outputLuminosityMin = _plugin->fetchRGBAParam( kOutputLuminosityMin );
	_outputLuminosityMax = _plugin->fetchRGBAParam( kOutputLuminosityMax );
	
	_interactScene.push_back( new interact::ParamRectangleInClip<interact::eCoordonateSystemXXcn>( _infos, _cornerA, _cornerB, _srcClip ), new interact::ActiveFunctor() );
}

bool ImageStatisticsOverlayInteract::draw( const OFX::DrawArgs &args )
{
	return _interactScene.draw( args );
}

bool ImageStatisticsOverlayInteract::penMotion( const OFX::PenArgs& args )
{
	return _interactScene.penMotion( args );
}

bool ImageStatisticsOverlayInteract::penDown( const OFX::PenArgs& args )
{
	return _interactScene.penDown( args );
}

bool ImageStatisticsOverlayInteract::penUp( const OFX::PenArgs& args )
{
	return _interactScene.penUp( args );
}

}
}
}
