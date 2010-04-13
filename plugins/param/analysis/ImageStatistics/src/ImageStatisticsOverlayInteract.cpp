#include "ImageStatisticsOverlayInteract.hpp"
#include "ImageStatisticsDefinitions.hpp"
#include "ImageStatisticsPlugin.hpp"
#include "tuttle/plugin/interact/Frame.hpp"
#include "tuttle/plugin/interact/ParamRectangleFromCenterSize.hpp"
#include <tuttle/plugin/interact/ParamRectangleFromTwoCorners.hpp>
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
	
	_interactScene.push_back( new interact::ParamRectangleFromCenterSize<interact::FrameClip, eCoordonateSystemXY>( _infos, _plugin->_rectCenter, _plugin->_rectSize, interact::FrameClip(_plugin->_srcClip) ), new interact::AlwaysActiveFunctor() );
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
