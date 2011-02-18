#include "ImageStatisticsOverlayInteract.hpp"
#include "ImageStatisticsDefinitions.hpp"
#include "ImageStatisticsPlugin.hpp"
#include "tuttle/plugin/interact/Frame.hpp"
#include "tuttle/plugin/interact/ParamRectangleFromCenterSize.hpp"
#include <tuttle/plugin/interact/ParamRectangleFromTwoCorners.hpp>
#include <tuttle/plugin/opengl/gl.h>
#include <tuttle/plugin/interact/interact.hpp>
#include <tuttle/plugin/interact/overlay.hpp>

#include <ofxsImageEffect.h>
#include <ofxsInteract.h>
#include <boost/gil/gil_all.hpp>

#include <vector>

namespace tuttle {
namespace plugin {
namespace imageStatistics {

ImageStatisticsOverlayInteract::ImageStatisticsOverlayInteract( OfxInteractHandle handle, OFX::ImageEffect* effect )
	: OFX::OverlayInteract( handle )
	, _infos( effect )
	, _interactScene( *effect, _infos )
{
	_effect = effect;
	_plugin = dynamic_cast<ImageStatisticsPlugin*>( _effect );

	interact::InteractObject* objCanonical  = new interact::ParamRectangleFromCenterSize<interact::FrameClip, eCoordinateSystemXY>( _infos, _plugin->_paramRectCenter, _plugin->_paramRectSize, interact::FrameClip( _plugin->_clipSrc ) );
	interact::InteractObject* objNormalized = new interact::ParamRectangleFromCenterSize<interact::FrameClip, eCoordinateSystemXYn>( _infos, _plugin->_paramRectCenter, _plugin->_paramRectSize, interact::FrameClip( _plugin->_clipSrc ) );
	_interactScene.push_back( objCanonical, new interact::IsActiveChoiceParamFunctor<>( _plugin->_paramCoordinateSystem ) );
	_interactScene.push_back( objNormalized, new interact::IsActiveChoiceParamFunctor<true>( _plugin->_paramCoordinateSystem ) );
}

bool ImageStatisticsOverlayInteract::draw( const OFX::DrawArgs& args )
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
