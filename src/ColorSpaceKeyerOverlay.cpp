
#include "ColorSpaceKeyerOverlay.hpp"
#include <tuttle/plugin/opengl/gl.h>


namespace tuttle {
namespace plugin {
namespace colorSpaceKeyer {
	
/*
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
	_isCtrlKeyDown = false;		//Ctrl key is not pressed by default
	_rotateX = _rotateY = 0.0;	//initialize rotation to 0
	_rotateXForm = _rotateYForm = 0.0; //initialize rotation centered to geodesic form to 0
	_origin.x = _origin.y = _end.x = _end.y = 0;	//initialize mouse positions to 0
}

/*
 * Destructor
 */
ColorSpaceKeyerOverlay::~ColorSpaceKeyerOverlay() 
{
	_plugin->releaseCloudPointData();	//release Overlay data
}

/*
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
	
	if(args.time != getData()._time || args.time != getData()._averageColor._time)
	{		
		std::cout << "Warning !!" << std::endl;
	}
	
	
	glLoadIdentity();				//projection to idendity
	///@todo: put overlay warning here
	
	const GLdouble left = -0.5;
	const GLdouble right = 1.5;
	const GLdouble bottom = -0.5;
	const GLdouble top = 1.5;
	const GLdouble near = 10.;
	const GLdouble far = -10.;
	glOrtho( left, right, bottom, top, near, far );
	//glFrustum( left, right, bottom, top, near, far );

}

/*
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
	
	//test average coord
	glColor3f(1.0f,0,0); glVertex3d(_coordAverageRotation.x,_coordAverageRotation.y, _coordAverageRotation.z); //draw coord average
	//std::cout<<"coordAverage : "<<_coordAverageRotation.x <<","<<_coordAverageRotation.y<<","<<_coordAverageRotation.z<<std::endl;
	
	glPointSize(1.0f);
	glEnd();
}

/*
 * Main display of the ColorSpaceKeyerOverlay class
 * @param args current arg (time/renderScale/pixelScale....
 * @return if there is something drawed by the function (y or n)
 */
bool ColorSpaceKeyerOverlay::draw( const OFX::DrawArgs& args )
{
	bool displaySomethings = false;
	if( _plugin->_paramBoolPointCloudDisplay->getValue() ) //Is CloudPointData displayed ? (GUI)
	{
		prepareOpenGLScene(args);					//prepare frustum and projection settings
		if(_plugin->_updateVBO )					//VBO need to be updated
		{
			//update VBO
			getData().updateVBO();					//update VBO from VBO data (already computed)
			_plugin->_updateVBO = false;			//VBO has been recomputed
		}
		if(_plugin->_resetViewParameters)			//View parameters need to be reseted
		{
			_rotateX = _rotateY = 0;				//reset parameters
			_rotateXForm = _rotateYForm = 0;		//reset geodesic form center rotation parameters
			_plugin->_resetViewParameters = false;	//view parameters has been changed
		}
		
		//OpenGL parameters
		glEnable(GL_DEPTH_TEST);	//active depth (better for understand results)
		glPushMatrix();				//new transformation

		//rotate management (geodesic form center)
		glTranslated(getData()._averageColor._averageValue.x, getData()._averageColor._averageValue.y, getData()._averageColor._averageValue.z);	//center rotation to the average
		glRotated(_rotateYForm,1.0f,0.0f,0.0);	//rotation on Y axis (piloted with mouse)
		glRotated(_rotateXForm,0.0f,1.0f,0.0);	//rotation on X axis (piloted with mouse)
		glTranslated(-getData()._averageColor._averageValue.x, -getData()._averageColor._averageValue.y, -getData()._averageColor._averageValue.z);	//un-active center rotation
		
		//rotate management (cube center)
		glTranslatef(.5f,.5f,.5f);				//center rotation to the middle of cube
		glRotated(_rotateY,1.0f,0.0f,0.0);		//rotation on Y axis (piloted with mouse)
		glRotated(_rotateX,0.0f,1.0f,0.0);		//rotation on X axis (piloted with mouse)
		glTranslatef(-.5f,-.5f,-.5f);			//un-active center translation
		
		//drawing Axes
		drawAxes();								//draw the X,Y and Z axes
		//drawing VBO
		if(getData()._isVBOBuilt)				//if VBO has been built
			getData()._imgVBO.draw();			//draw VBO

		
		
		//drawing selection VBO

		//if(getData()._isSelectionVBOBuilt)	//if selection VBO has been built
		//	getData()._selectionVBO.draw();		//draw selection VBO
		if(getData()._isSelectionVBOBuilt && _plugin->_paramBoolSeeSelection->getValue()) //selection VBO data is built
		{
			glBegin(GL_POINTS); 
			glColor3f(1.0f,1.0f,1.0f);
			for(unsigned int i=0; i<getData()._selectionCopy.size(); i+= 3)
				glVertex3d(getData()._selectionCopy[i],getData()._selectionCopy[i+1],getData()._selectionCopy[i+2]);
			glEnd();
		}		
		
		//drawing average
		getData()._averageColor.draw();							//draw average (cross)
		//drawing geodesic form
		if(_plugin->_paramBoolDisplayGeodesicForm->getValue())	//if geodesic form has been built
			getData()._geodesicForm.draw();						//draw geodesic form on screen
		
		//OpenGL end of parameters
		glPopMatrix();				//pop matrix
		glDisable(GL_DEPTH_TEST);	//disable deep
		displaySomethings = true;	//something has been drown on screen
	}
	return displaySomethings; //return if overlay has displayed something (y or n)
}

/*
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

/*
 * Capture and treat click up event (release mouse)
 * @param args current argument (time/renderScale/pixelScale....
 * @return if the event has been treated (y or n)
 */
bool ColorSpaceKeyerOverlay::penUp( const OFX::PenArgs& args )
{
	if(_isPenDown) //is mouse is already selected
	{
		_isPenDown = false;				//mouse is no more selected
		_end.x = args.penPosition.x;	//capture mouse current position x
		_end.y = args.penPosition.y;	//capture mouse current position y
		
		if(_origin.x == _end.x && _origin.y == _end.y)
			return false;	//basic click (there is nothing to do in this case)
				
		return true; //event has been captured
	}
	return false;
}

/*
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

		if(_isCtrlKeyDown)
		{
			_rotateXForm += (deltaX/args.pixelScale.x)/kRotationSpeed; //add delta to geodesic center rotation (X axis)
			_rotateYForm += (deltaY/args.pixelScale.y)/kRotationSpeed; //add delta to geodesic center rotation (Y axis)
		}
		else
		{
			_rotateX += (deltaX/args.pixelScale.x)/kRotationSpeed; //add delta to cube rotation (X axis)
			_rotateY += (deltaY/args.pixelScale.y)/kRotationSpeed; //add delta to cube rotation (Y axis)
		}
		_origin.x = args.penPosition.x;
		_origin.y = args.penPosition.y;
		
		return true; //event has been captured
	}
	return false;
}

/**
 * Treat the Ctrl key pressed (down)
 * @param args current arg (time/rendeerScale/...)
 * @return  if the event has been treated (y or n)
 */
bool ColorSpaceKeyerOverlay::keyDown( const OFX::KeyArgs& args )
{
	if(args.keySymbol==kOfxKey_Control_L||args.keySymbol==kOfxKey_Control_R) //if the pressed key is Ctrl key (left or right)
	{
		_isCtrlKeyDown = true; //Ctrl key is pressed
		return true; //event has been treated
	}
	return false; //event has not been treated (key pressed is not Ctrl)
}

/**
 * Treat the Ctrl key pressed (up)
 * @param args current arg (time/rendeerScale/...)
 * @return  if the event has been treated (y or n)
 */
bool ColorSpaceKeyerOverlay::keyUp( const OFX::KeyArgs& args )
{
	if(args.keySymbol==kOfxKey_Control_L||args.keySymbol==kOfxKey_Control_R) //if the pressed key is Ctrl key (left or right)
	{
		if(_isCtrlKeyDown)
		{
			_isCtrlKeyDown = false; //Ctrl key is not pressed anymore
		}
		return true; //event has been treated
	}
	return false; //event has not been treated (key pressed is not Ctrl)
}

/*
 * Get cloud point data from ColorSpaceKeyerPlugin
 * @return 
 */
CloudPointData& ColorSpaceKeyerOverlay::getData()
{
	return _plugin->getCloudPointData();	//return CloudPointData initialized at constructor (by scoped pointer)
}

/**
 * Matrix product with a 3D vector and a 4*4 matrix 
 */
void productVectorMatrix(double* v, double* m, double* result)
{
	result[0] = m[0]*v[0]+m[4]*v[1]+m[8]*v[2]+m[12]*1;	//compute X value
	result[1] = m[1]*v[0]+m[5]*v[1]+m[9]*v[2]+m[13]*1;	//compute Y value
	result[2] = m[2]*v[0]+m[6]*v[1]+m[10]*v[2]+m[14]*1;	//compute Z value
	result[3] = m[3]*v[0]+m[7]*v[1]+m[11]*v[2]+m[15]*1; //compute w
	
	//normalize result vector
	for(unsigned int i=0; i<4; ++i)
		result[i] /= result[3];
}

}
}
}

