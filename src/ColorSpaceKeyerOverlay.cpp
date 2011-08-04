
#include "ColorSpaceKeyerOverlay.hpp"
#include <tuttle/plugin/opengl/gl.h>


namespace tuttle {
namespace plugin {
namespace colorSpaceKeyer {
	
/**
 * Constructor
 * @param handle
 * @param effect
 */
ColorSpaceKeyerOverlay::ColorSpaceKeyerOverlay(OfxInteractHandle handle,OFX::ImageEffect* effect)
: OFX::OverlayInteract(handle)
, _infos(effect)
{
	_plugin = static_cast<ColorSpaceKeyerPlugin*>(_effect); //get plugin
	_plugin->addRefCloudPointData(); //create pointer to overlay data
	
	_isPenDown = false;			//mouse is not under control by default
	_rotateX = _rotateY = 0.0;	//initialize rotation to 0
	_origin.x = _origin.y = _end.x = _end.y = 0;	//initialize mouse positions to 0
	_isFirst = true;			//draws haven't been done yet (first time)
}

/**
 * Destructor
 */
ColorSpaceKeyerOverlay::~ColorSpaceKeyerOverlay() 
{
	_plugin->releaseCloudPointData();	//release Overlay data
}

/**
 * Prepare the OpenGL scene for drawing (projection and frustrum settings)
 */
void ColorSpaceKeyerOverlay::prepareOpenGLScene(const OFX::DrawArgs& args)
{
	//reset OpenGL scene (remove previous img)
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	//change background color to gray
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	
	//define openGL scene frustrum
	glMatrixMode( GL_MODELVIEW );	//load standard mode
	glLoadIdentity();				//projection to idendity
	
	const GLdouble left = -0.5;
	const GLdouble right = 1.5;
	const GLdouble bottom = -0.5;
	const GLdouble top = 1.5;
	const GLdouble near = 10.;
	const GLdouble far = -10.;
	glOrtho( left, right, bottom, top, near, far );
	//glFrustum( left, right, bottom, top, near, far );

	if( _isFirst )
	{
		//construct VBO
//		const bool useVBODiscretization = _plugin->_paramBoolDiscretizationActive->getValue();		//Should it discretize the VBO
//		const int discretizationStep = _plugin->_paramIntDiscretization->getValue();				//discretization step (if discretize)
//		const bool errcodeVBO = getData().generateVBO(_plugin->_clipSrc,args.renderScale, useVBODiscretization,discretizationStep);	//generate VBO
//		if( !errcodeVBO )
//		{
//			TUTTLE_COUT_WARNING( "Error building VBO" );
//		}
//		std::cout<< "AVERAGE" << std::endl;
//		const bool errcodeAVG = getData().generateAverageColorSelection(_plugin->_clipColor,args.renderScale);
//		if( !errcodeAVG )
//		{
//			TUTTLE_COUT_WARNING("Error computing selection average");
//		}
		_isFirst = false;	//it is not the first draw anymore
	}
}

/**
 * Draw the x,Y and Z axes into the OpenGL scene
 */
void ColorSpaceKeyerOverlay::drawAxes()
{
	glBegin(GL_LINES);
	//X axis (black => red)
	glColor3f(0.0f,0.0f,0.0f); glVertex2i(0,0);		//color and direction (0,0,0)
    glColor3f(1.0f,0.0f,0.0f); glVertex2i(1,0);		//color and direction (1,0,0)
	//Y axis (black => green)
	glColor3f(0.0f,0.0f,0.0f); glVertex2i(0,0);		//color and direction (0,0,0)
    glColor3f(0.0f,1.0f,0.0f); glVertex2i(0,1);		//color and direction (0,1,0)
    //Z axis (black => blue)
	glColor3f(0.0f,0.0f,0.0f); glVertex2i(0,0);		//color and direction (0,0,0)
	glColor3f(0.0f,0.0f,1.0f); glVertex3i(0,0,1);	//color and direction (0,0,1)
	//axis (red => yellow)
	glColor3f(1.0f,0.0f,0.0f); glVertex2i(1,0);		//red
	glColor3f(1.0f,1.0f,0.0f); glVertex2i(1,1);		//yellow
	//axis (red => yellow)
	glColor3f(0.0f,1.0f,0.0f); glVertex2i(0,1);		//green
	glColor3f(1.0f,1.0f,0.0f); glVertex2i(1,1);		//yellow
	//axis (green => cyan)
	glColor3f(0.0f,1.0f,0.0f); glVertex2i(0,1);		//green
	glColor3f(0.0f,1.0f,1.0f); glVertex3i(0,1,1);	//cyan
	//axis (blue => cyan)
	glColor3f(0.0f,0.0f,1.0f); glVertex3i(0,0,1);	//blue
	glColor3f(0.0f,1.0f,1.0f); glVertex3i(0,1,1);	//cyan
	//axis (cyan => white)
	glColor3f(1.0f,1.0f,1.0f); glVertex3i(1,1,1);	//white
	glColor3f(0.0f,1.0f,1.0f); glVertex3i(0,1,1);	//cyan
	//axis (yellow => white)
	glColor3f(1.0f,1.0f,1.0f); glVertex3i(1,1,1);	//white
	glColor3f(1.0f,1.0f,0.0f); glVertex2i(1,1);		//yellow
	//axis (magenta => white)
	glColor3f(1.0f,1.0f,1.0f); glVertex3i(1,1,1);	//white
	glColor3f(1.0f,0.0f,1.0f); glVertex3i(1,0,1);	//magenta
	//axis (magenta => blue)
	glColor3f(0.0f,0.0f,1.0f); glVertex3i(0,0,1);	//blue
	glColor3f(1.0f,0.0f,1.0f); glVertex3i(1,0,1);	//magenta
	//axis (magenta => red)
	glColor3f(1.0f,0.0f,0.0f); glVertex2i(1,0);		//red
	glColor3f(1.0f,0.0f,1.0f); glVertex3i(1,0,1);	//magenta
	//axis (white => black)
	glColor3f(1.0f,1.0f,1.0f); glVertex3i(1,1,1);	//white
	glColor3f(0.0f,0.0f,0.0f); glVertex2i(0,0);		//black
	glEnd();
	
	glPointSize(6.0f);
	glBegin(GL_POINTS);
	glColor3f(0.0f,0.0f,0.0f); glVertex2i(0,0);		//black	
	glColor3f(1.0f,0.0f,0.0f); glVertex2i(1,0);		//red
	glColor3f(0.0f,1.0f,0.0f); glVertex2i(0,1);		//green
	glColor3f(0.0f,0.0f,1.0f); glVertex3i(0,0,1);	//blue
	glColor3f(1.0f,1.0f,1.0f); glVertex3i(1,1,1);	//white
	glColor3f(1.0f,0.0f,1.0f); glVertex3i(1,0,1);	//magenta
	glColor3f(0.0f,1.0f,1.0f); glVertex3i(0,1,1);	//cyan
	glColor3f(1.0f,1.0f,0.0f); glVertex2i(1,1);		//yellow
	glPointSize(1.0f);
	glEnd();
}

/**
 * Main display of the ColorSpaceKeyerOverlay class
 * @param args current arg (time/renderScale/pixelScale....
 * @return if there is something drawed by the function (y or n)
 */
bool ColorSpaceKeyerOverlay::draw( const OFX::DrawArgs& args )
{
	bool displaySomethings = false;
	if( _plugin->_paramBoolPointCloudDisplay->getValue() ) //Is CloudPointData displayed ? (GUI)
	{
		if(_plugin->_updateVBO )
		{
			//update VBO
			getData().updateVBO(); //update VBO from VBO data (already computed)
			_plugin->_updateVBO = false; //VBO has been recomputed
		}
		
		glPushMatrix();
		prepareOpenGLScene(args); //prepare frustum and projection settings

		//rotate management
		glTranslatef(.5f,.5f,.5f);		//center rotation to the middle of cube
		glRotated(_rotateY,1.0f,0.0f,0.0);	//rotation on Y axis (piloted with mouse)
		glRotated(_rotateX,0.0f,1.0f,0.0);	//rotation on X axis (piloted with mouse)
		glTranslatef(-.5f,-.5f,-.5f);	//un-active center translation

		//drawing
		drawAxes();					//draw the X,Y and Z axes
		getData()._imgVBO.draw();	//draw VBO
		getData().drawAverage();	//draw the average on screen
		getData()._geodesicForm.draw();	//draw geodesic form
		
		glTranslatef(.5f,.5f,.5f);
		
		glPopMatrix();
		displaySomethings = true;	//something has been drown on screen
	}
	return displaySomethings;
}

/**
 * Capture and treat click down event
 * @param args current argument (time/renderScale/pixelScale....
 * @return if the event has been treated (y or n)
 */
bool ColorSpaceKeyerOverlay::penDown( const OFX::PenArgs& args )
{
	if(!_isPenDown) //is mouse is not already selected
	{
		_isPenDown = true;
		_origin.x = _end.x = args.penPosition.x; //capture x position of current click
		_origin.y = _end.y = args.penPosition.y; //capture y position of current click
		return true; //event has been captured
	}
	return false;
}

/**
 * Capture and treat click up event (release mouse)
 * @param args current argument (time/renderScale/pixelScale....
 * @return if the event has been treated (y or n)
 */
bool ColorSpaceKeyerOverlay::penUp( const OFX::PenArgs& args )
{
	if(_isPenDown) //is mouse is already selected
	{
		_isPenDown = false; //mouse is no more selected
		_end.x = args.penPosition.x; //capture mouse current position x
		_end.y = args.penPosition.y; //capture mouse current position y
		
		if(_origin.x == _end.x && _origin.y == _end.y)
			return false;	//basic click (there is nothing to do in this case)
		return true; //event has been captured
	}
	return false;
}

/**
 * Treat if mouse/pen is under motion (drawing on screen for example)
 * @param args current arg (time/renderScale/pixelScale....
 * @return if the event has been treated (y or n)
 */
bool ColorSpaceKeyerOverlay::penMotion( const OFX::PenArgs& args )
{
	if(_isPenDown) //is mouse is already selected
	{
		_end.x = args.penPosition.x; //capture mouse current position x
		_end.y = args.penPosition.y; //capture mouse current position y
		
		int deltaX = _end.x - _origin.x; //compute delta for rotation on Y axis (horizontal)
		int deltaY = _end.y - _origin.y; //compute delta for rotation on X axis (vertical)

		_rotateX += (deltaX/args.pixelScale.x)/kRotationSpeed; //add delta to current rotation (X axis)
		_rotateY += (deltaY/args.pixelScale.y)/kRotationSpeed; //add delta to current rotation (Y axis)

		_origin.x = args.penPosition.x;
		_origin.y = args.penPosition.y;
		
		return true; //event has been captured
	}
	return false;
}

/**
 * Get cloud point data from ColorSpaceKeyerPlugin
 * @return 
 */
CloudPointData& ColorSpaceKeyerOverlay::getData()
{
	return _plugin->getCloudPointData();	//return CloudPointData initialized at constructor (by scoped pointer)
}

}
}
}

