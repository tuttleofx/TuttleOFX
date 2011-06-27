#include "HistogramKeyerOverlay.hpp"
#include "HistogramKeyerPlugin.hpp"
#include "HistogramKeyerHistogramDisplay.hpp"

#include <tuttle/plugin/opengl/gl.h>
#include <tuttle/plugin/interact/interact.hpp>
#include <tuttle/plugin/interact/overlay.hpp>
#include <tuttle/plugin/image/ofxToGil.hpp>

#include <ofxsImageEffect.h>
#include <ofxsInteract.h>
#include <boost/gil/gil_all.hpp>

#include <vector>

namespace tuttle {
namespace plugin {
namespace histogramKeyer {

HistogramKeyerOverlay::HistogramKeyerOverlay( OfxInteractHandle handle, OFX::ImageEffect* effect )
	: OFX::OverlayInteract( handle ), _infos( effect )
{    
	_plugin = static_cast<HistogramKeyerPlugin*>( _effect );
	_histogramDisplay = HistogramKeyerHistogramDisplay(_plugin);
	_penDown = false;
	
	//initialize bool img
	_size = _plugin->_clipSrc->getPixelRodSize(NULL);
	_imgBool = new  bool*[_size.y];
	for(unsigned int i=0; i<_size.y; ++i)
	{
		_imgBool[i] = new bool[_size.x];
		for(unsigned int j=0; j<_size.x; ++j)
			_imgBool[i][j] = false;
	}
}

bool HistogramKeyerOverlay::draw( const OFX::DrawArgs& args )
{
	typedef boost::gil::point2<Scalar> Point2;
	bool displaySomething = false;

	if(_plugin->_clipSrc->isConnected())
	{
		displaySomething = true;
        //is the display adapted to each channel or global (IHM)        
        if(this->_plugin->_paramDisplayTypeSelection->getValue() == 0)
			this->_histogramDisplay._isAdaptedToHeight = true;
        else
            this->_histogramDisplay._isAdaptedToHeight = false;
        
		this->_histogramDisplay.displayHistogramOnScreenRGB(_plugin->_data, args.time);
		this->_histogramDisplay.displayHistogramOnScreenHSL(_plugin->_data, args.time);
		this->displaySelectedAreas();

	}
	return displaySomething;
}

bool HistogramKeyerOverlay::penMotion( const OFX::PenArgs& args )
{
//	if( _penDown && _pickMode == ePickModePixel )
//	{
//	}
	return false;
}

bool HistogramKeyerOverlay::penDown( const OFX::PenArgs& args )
{
	if(!_penDown)
	{
		_origin.x = args.penPosition.x;
		_origin.y = args.penPosition.y;
		_penDown = true;
	}
	return true;
}

bool HistogramKeyerOverlay::penUp( const OFX::PenArgs& args )
{
//	DOESN'T WORK :(
	if(_penDown)
	{
		_end.x = args.penPosition.x;
		_end.y = args.penPosition.y;
		
		//pixel traitement
		int startX,endX,startY,endY;
		if(_origin.x > _end.x)
		{
			startX = _origin.x;
			endX = _end.x;
		}
		else
		{
			endX =_origin.x;
			startX = _end.x;
		}
		
		if(_origin.y > _end.y)
		{
			startY = _origin.y;
			endY = _end.y;
		}
		else
		{ 
			endY= _origin.y;
			startY = _end.y;
		}
		int step_x = startX-endX;
		int step_y = startY-endY;
		for(unsigned int val_y=0; val_y<step_y; ++val_y)
		{
			for(unsigned int val_x=0; val_x<step_x; ++val_x )
			{
				_imgBool[endY+val_y][endX+val_x] = true;
			}
		}
		_penDown = false;
	}
	return true;
}

void HistogramKeyerOverlay::displaySelectedAreas()
{
	glDisable(GL_BLEND);
	glBegin( GL_POINTS );
	glColor3f(1.0f,0.0f,0.0f);
	for(unsigned int i=0; i<_size.y; ++i)
	{
		for(unsigned int j=0; j<_size.x; ++j)
		{	
			if(_imgBool[i][j])
				glVertex2f((float)j,(float)i);
		}
	}
	glEnd();
}

}
}
}
