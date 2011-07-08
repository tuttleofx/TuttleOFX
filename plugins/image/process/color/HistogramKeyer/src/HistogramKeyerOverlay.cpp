#include "HistogramKeyerOverlay.hpp"
#include "HistogramKeyerPlugin.hpp"
#include "HistogramKeyerHistogramDisplay.hpp"

namespace tuttle {
namespace plugin {
namespace histogramKeyer {

HistogramKeyerOverlay::HistogramKeyerOverlay(OfxInteractHandle handle,OFX::ImageEffect* effect): OFX::OverlayInteract(handle), _infos(effect)
{    
	_plugin = static_cast<HistogramKeyerPlugin*>(_effect);
	_hslParam = HSLOverlay(_plugin);
	_rgbParam = RGBOverlay(_plugin);
	
	_size = _plugin->_clipSrc->getPixelRodSize(NULL);			//size of the current source clip
	_penDown = false;											//Mouse is not clicked down by default
	_keyDown = false;											//Ctrl key is not pressed by default 	
	_plugin->addRefOverlayData();								//add reference to Overlay data
	
	_isFirstTime = true; //temporary
}	

HistogramKeyerOverlay::~HistogramKeyerOverlay()
{
	_plugin->releaseOverlayData();	//release Overlay data
}

bool HistogramKeyerOverlay::draw( const OFX::DrawArgs& args )
{	
	/*Gloabl display option*/
	if(_plugin->_paramGlobalDisplaySelection->getValue() == false)
		return false;
	if(_isFirstTime)
	{
		getData().computeFullData(_plugin->_clipSrc,args.time,args.renderScale);
		_isFirstTime = false;
	}
	
	/*Draw component*/
	bool displaySomething = false;
	if(_plugin->_clipSrc->isConnected())
	{
		displaySomething = true;  
		
		if(_plugin->_paramDisplaySelection->getValue())	//DisplaySelection
			this->displaySelectedAreas();
		
		///@todo : remove next lines when Nuke curves overlay works
		glPushMatrix();
		glTranslatef(-(_size.x+kTranslationRGB),0.0f,0.0f);
		
		_rgbParam.draw(args);  
		glPopMatrix();
		
		///@todo : remove next lines when Nuke curves overlay works
		glPushMatrix();
		glTranslatef((_size.x+kTranslationHSL),0.0f,0.0f);	
		
		_hslParam.draw(args);
		glPopMatrix();
		
		if(_penDown && !_keyDown)//Display selection zone
			this->displaySelectionZone();
	}
	return displaySomething;
}

/**
 * When the pen/mouse is under motion
 * @param args current arg
 * @return event capted (y or n)
 */
bool HistogramKeyerOverlay::penMotion( const OFX::PenArgs& args )
{	
	if(_penDown && !_keyDown)//the mouse is moving but there is not Ctrl key pressed
	{
		_end.x = args.penPosition.x;	//needed to draw the selection square
		_end.y = args.penPosition.y;	//needed to draw the selection square
		return true;					//event captured
	}
	if(_penDown && _keyDown)//the mouse is moving and there is Ctrl key pressed
	{
		getData()._imgBool[args.penPosition.y][args.penPosition.x] = 255;	//current pixel is marked as selected
		return true;														//event captured
	}
	return false; //event is not captured
}

/**
 * When the pen/mouse is clicking down
 * @param args current arg
 * @return event capted (y or n)
 */
bool HistogramKeyerOverlay::penDown( const OFX::PenArgs& args )
{
	if(!_penDown && !_keyDown)	//mouse is already used and there is not Ctrl key pressed
	{
		_penDown = true;	
		if(args.penPosition.y < _size.y && args.penPosition.y > 0)	//mouse Y is into the image
			_origin.y = args.penPosition.y;
		else
		{
			if(args.penPosition.y > _size.y)	//clamp the selected Y pixel to the image borders
				_origin.y = _size.y;			//click is on the top of the image
			else
				_origin.y = 0;					//click is on the bottom of the image
		}
		if(args.penPosition.x < _size.x && args.penPosition.x > 0) //mouse X is on the image
			_origin.x = args.penPosition.x;
		else
		{
			if(args.penPosition.x > _size.x)	//clamp the selected X pixel to the image borders
				_origin.x = _size.x;			//click is on the right of the image
			else
				_origin.x = 0;					//click is on the left of the image
		}
		_end.x = args.penPosition.x;	//set X end of the selection square at the origin (initialization)
		_end.y = args.penPosition.y;	//set Y end of the selection square at the origin (initialization)
	}
	else	//there is Ctrl key pressed
	{
		if(!_penDown)
			_penDown = true;
	}
	return true;
}

/**
 * When the pen/mouse is clicking up
 * @param args current arg
 * @return event capted (y or n)
 */
bool HistogramKeyerOverlay::penUp( const OFX::PenArgs& args )
{
	//clamp selection
	_end.x = args.penPosition.x;		//attach the end of the selection square to the current pixel X
	_end.y = args.penPosition.y;		//attach the end of the selection square to the current pixel Y
	if(_end.x == _origin.x && _end.y == _origin.y)	//it's just one click!
	{
		_penDown = false;	//change penDown
		return false;		//event is not capured
	}
	if(!(args.penPosition.x < _size.x && args.penPosition.x > 0 && args.penPosition.y < _size.y && args.penPosition.y > 0)) // if click is not on the image
	{
		if(args.penPosition.x < 0.0 || args.penPosition.x > _size.x) //problem with X axis
		{
			if(args.penPosition.x < 0.0)	//click is on the left of the image
				_end.x = 0.0;				//clamp
			else
				_end.x = _size.x;			//click is on the right of the image
		}
		if(args.penPosition.y < 0.0 || args.penPosition.y > _size.y) //problem with Y axis
		{
			if(args.penPosition.y < 0.0)	//click is on the bottom of the image
				_end.y = 0.0;				//clamp
			else
				_end.y = _size.y;			//click is on the top of the image
		}
	}
	if(_penDown && !_keyDown)	//if there is not Ctrl key pressed
	{
		int startX,endX,startY,endY; 
		if(_origin.x > _end.x)  //transform selection zone to be OK (X axis)
		{
			startX = _origin.x;
			endX = _end.x;
		}
		else
		{
			endX =_origin.x;
			startX = _end.x;
		}
		
		if(_origin.y > _end.y)	//transform selection zone to be OK (Y axis)
		{
			startY = _origin.y;
			endY = _end.y;
		}
		else
		{ 
			endY= _origin.y;
			startY = _end.y;
		}
		int step_x = startX-endX;	//determinate width of the selected zone
		int step_y = startY-endY;	//determinate height of the selected zone
		for(unsigned int val_y=0; val_y<step_y; ++val_y)
		{
			for(unsigned int val_x=0; val_x<step_x; ++val_x )
			{
				getData()._imgBool[endY+val_y][endX+val_x] = 255;	//mark all of the selected pixel
			}
		}
		getData().computeHistogramBufferData(getData()._selectionData,_plugin->_clipSrc,args.time,args.renderScale,true); //update selection histograms buffer datas
		getData().correctHistogramBufferData(getData()._selectionData);		    //correct selection histograms buffer datas
		getData().computeAverages();	//update average data
	}
	_penDown = false; //treatment is finished
	return true;
}

/**
 * Ctrl key is pressed down
 * @param args current arg
 * @return event capted (y or n)
 */
bool HistogramKeyerOverlay::keyDown( const OFX::KeyArgs& args )
{
	if(args.keySymbol==kOfxKey_Control_L||args.keySymbol==kOfxKey_Control_R) //if the pressed key is Ctrl key (left or right)
	{
		_keyDown = true;	//treatment begins
		return true;		//event captured 
	}
	return false;			//event is not captured (other key)
}

/**
 * Ctrl key is released
 * @param args current arg
 * @return event capted (y or n)
 */
bool HistogramKeyerOverlay::keyUp( const OFX::KeyArgs& args )
{
	if((args.keySymbol == kOfxKey_Control_L||args.keySymbol==kOfxKey_Control_R)&&_keyDown) //if the release key is Ctrl (and it has been pressed before)
	{
		_keyDown = false;	//treatment ends
		getData().computeHistogramBufferData(getData()._selectionData,_plugin->_clipSrc,args.time,args.renderScale,true);	//update selection histogram
		getData().correctHistogramBufferData(getData()._selectionData);								//correct selection histogram
		return true;		//event captured

	}
	return false;			//event is not captured (wrong key)
}

/**
 * Display the selected areas on the clip (color : gray)
 */
void HistogramKeyerOverlay::displaySelectedAreas()
{
	glEnable(GL_TEXTURE_2D);					//Activate texturing
	GLuint Name;								//Texture name
	glGenTextures(1,&Name);						//generate a texture number
	glBindTexture(GL_TEXTURE_2D,Name);
	glTexImage2D(
		GL_TEXTURE_2D,		//Type : texture 2D
		0,					//Mipmap : none
		GL_ALPHA8,			//Colors : 4
		_size.x,			//width
		_size.y,			//height
		0,					//border size
		GL_ALPHA,			//Format : RGBA
		GL_UNSIGNED_BYTE, 	//color kind
		getData()._imgBool.data()		// data buffer
	); 	
	glColor4f(1.0f,1.0f,1.0f,0.2f);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//Draw Texture on screen	
	glBegin(GL_QUADS);
    glTexCoord2d(0,1);  glVertex2d(0,_size.y);			//Top Left
	glTexCoord2d(1,1);  glVertex2d(_size.x,_size.y);	//Top Right
	glTexCoord2d(1,0);  glVertex2d(_size.x,0);			//Bottom Right
	glTexCoord2d(0,0);  glVertex2d(0,0);				//Bottom Left
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

/**
 *Display the selection zone on the clip (color : border gray)
 */
void HistogramKeyerOverlay::displaySelectionZone()
{
	glEnable(GL_BLEND);
	glBlendFunc (GL_ONE, GL_ONE);
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(1, (short) 0x0101);	//to draw -------
	glBegin( GL_LINE_LOOP );
	glColor3f(.5f,0.5f,0.5f);			//white
	glVertex2f(_origin.x,_origin.y);	//draw selection square
	glVertex2f(_origin.x,_end.y);
	glVertex2f(_end.x,_end.y);
	glVertex2f(_end.x,_origin.y);
	glEnd();
	glDisable(GL_LINE_STIPPLE);
	glDisable(GL_BLEND);
	glEnd();
}

/**
 * Get overlay data from plugin
 * @return 
 */
OverlayData& HistogramKeyerOverlay::getData()
{
	return _plugin->getOverlayData();
}
	
}
}
}