
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
	
	_isPenDown = false;								//mouse is not under control by default
	_isCtrlKeyDown = false;							//Ctrl key is not pressed by default
	_rotateX = _rotateY = 0.0;						//initialize rotation to 0
	_rotateXForm = _rotateYForm = 0.0;				//initialize rotation centered to geodesic form to 0
	_origin.x = _origin.y = _end.x = _end.y = 0;	//initialize mouse positions to 0
	
	///HACK : to initialize correctly overlay display data
	OFX::InstanceChangedArgs args;						//create instance changed arguments
	_plugin->changedParam(args,kPointCloudDisplay);		//call changed parameters function to initialize overlay data
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
	//reset OpenGL scene (remove previous image)
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	//change background color to gray
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	
	//get projection matrix
	GLdouble proj_matrix[16];
	glGetDoublev(GL_PROJECTION_MATRIX, proj_matrix);
	//initialize model-view and projection matrixes to identity
	glMatrixMode( GL_PROJECTION );	//load standard mode
	glLoadIdentity();				//projection to identity
	glMatrixMode( GL_MODELVIEW );	//load standard mode
	glLoadIdentity();				//model-view to identity 
	
	//get current viewport size	
	GLint viewport[4] = { 0, 0, 0, 0 };															//define result array
	glGetIntegerv(GL_VIEWPORT,viewport);														//get current viewport size
	const double ratio = (viewport[2]-viewport[0]) / (double)(viewport[3]-viewport[1]);			//compute ratio
	//define new coordinates
	glOrtho( 0.0, 1.0, 0.0, 1.0, -1.0, 1.0 );													//set coordinates to 0-1
	
	if(args.time != getData()._time || args.time != getData()._averageColor._time)		//if current overlay has not been updated
	{
		//display warning sign on screen
		Ofx3DPointD warningPoint;														//initialize warning drawing point
		warningPoint.x = 0.15;															//x == viewport width/7;
		warningPoint.y = 0.2;															//y == viewport height/5;
		drawWarning(warningPoint, ratio);												//draw warning sign
	}
	
	//define openGL scene frustrum
	glMatrixMode( GL_PROJECTION );	//load standard mode
	glLoadMatrixd( proj_matrix );   //reload previous projection matrix
	glMatrixMode( GL_MODELVIEW );	//load standard mode
	glLoadIdentity();				//projection to identity
	
	const GLdouble left = -0.5;								//frustrum left
	const GLdouble right = 1.5;								//frustrum right
	const GLdouble bottom = -0.5;							//frustrum bottom
	const GLdouble top = 1.5;								//frustrum top
	const GLdouble near = 10.;								//frustrum near
	const GLdouble far = -10.;								//frustrum far
	glOrtho( left, right, bottom, top, near, far );			//define new frustrum for overlay data
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
		glPushMatrix();								//new transformation
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
		
		//rotate management (cube center)
		glTranslatef(.5f,.5f,.5f);				//center rotation to the middle of cube
		glRotated(_rotateY,1.0f,0.0f,0.0);		//rotation on Y axis (piloted with mouse)
		glRotated(_rotateX,0.0f,1.0f,0.0);		//rotation on X axis (piloted with mouse)
		glTranslatef(-.5f,-.5f,-.5f);			//un-active center translation
				
		//rotate management (geodesic form center)
		glTranslated(_coordAverageDisplay.x,_coordAverageDisplay.y,_coordAverageDisplay.z);	//center rotation to the average
		glRotated(_rotateYForm,1.0f,0.0f,0.0);	//rotation on Y axis (piloted with mouse)
		glRotated(_rotateXForm,0.0f,1.0f,0.0);	//rotation on X axis (piloted with mouse)
		glTranslated(-_coordAverageDisplay.x,_coordAverageDisplay.y, -_coordAverageDisplay.z);	//un-active center rotation
		
		//drawing Axes
		drawAxes();								//draw the X,Y and Z axes
		//drawing VBO
		if(getData()._isVBOBuilt)				//if VBO has been built
			getData()._imgVBO.draw();			//draw VBO
		
		//drawing selection VBO
		if(getData()._isSelectionVBOBuilt && _plugin->_paramBoolSeeSelection->getValue()) //selection VBO data is built
		{
			getData()._selectionVBO.draw();		//draw selection VBO
		}		
		
		//drawing average
		getData()._averageColor.draw();							//draw average (cross)
		//drawing geodesic form
		if(_plugin->_paramBoolDisplayGeodesicForm->getValue())		//if geodesic form has been built
			getData()._geodesicForm.draw();							//draw geodesic form on screen
		
		//OpenGL end of parameters
		glDisable(GL_DEPTH_TEST);	//disable deep
		glPopMatrix();				//pop matrix
		displaySomethings = true;	//something has been drown on screen
	}
	updateCoordAverageRotation();	//test update coordinates
	return displaySomethings;		//return if overlay has displayed something (y or n)
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

/*
 * Matrix product with a 3D vector and a 4*4 matrix 
 */
void productVectorMatrix(double* v, double* m, double* result)
{
	result[0] = m[0]*v[0]+m[4]*v[1]+m[8]*v[2]+m[12]*1;	//compute X value
	result[1] = m[1]*v[0]+m[5]*v[1]+m[9]*v[2]+m[13]*1;	//compute Y value
	result[2] = m[2]*v[0]+m[6]*v[1]+m[10]*v[2]+m[14]*1;	//compute Z value
	result[3] = m[3]*v[0]+m[7]*v[1]+m[11]*v[2]+m[15]*1; //compute w
	
	//normalize result vector
	/*if(result[3] != 1.0)
	{
		std::cout <<"normalize result vector"<< std::endl;
		for(unsigned int i=0; i<4; ++i)
			result[i] /= result[3];
	}*/
}


/*
 * Does the multiplication A=A*B : all the matrices are described column-major
 */
void multMatrixBtoMatrixA( Matrix A, Matrix B)
{
    unsigned int i=0; // row index
    unsigned int j=0; // column index
    Matrix temp;

    for (unsigned int iValue=0 ; iValue<16 ; ++iValue)
    {
        temp[iValue]=0;
        i=iValue%4;		// if column-major
        j=iValue/4;		// if column-major
        for (unsigned int k=0 ; k<4 ; ++k)
        {
            int indexik=k*4+i;
            int indexkj=j*4+k;
            temp[iValue]+=A[indexik]*B[indexkj];
        }
    }
	//recopy temporary matrix into result
    A = temp;
}


/*
 *	Sets the provided matrix to identity
 */
void setToIdentity(GLfloat * matrix)
{
    GLfloat I[]={1.0, 0.0, 0.0, 0.0,
                 0.0, 1.0, 0.0, 0.0,
                 0.0, 0.0, 1.0, 0.0,
                 0.0, 0.0, 0.0, 1.0};
    for (int iMatrixCoord=0 ; iMatrixCoord<16 ; iMatrixCoord++)
        matrix[iMatrixCoord]=I[iMatrixCoord];
}


/*
 * Sets the provided matrix to a rotate matrix of angle "angle", around axis "axis"
 * 
 * @param[out] matrix
 * @param[in] angle
 * @param[in] axis
 */
void setToRotate( GLfloat * matrix, const GLfloat angle, const GLfloat * const axis )
{
    const GLfloat c=cos(angle);
    const GLfloat s=sin(angle);
    const GLfloat x=axis[0];
    const GLfloat y=axis[1];
    const GLfloat z=axis[2];

    if ((x==1.0) && (y==0.0) && (z==0.0))
    {
        GLfloat R[]={1.0, 0.0, 0.0, 0.0,
                     0.0, c,   s,   0.0,
                     0.0, -s,  c,   0.0,
                     0.0, 0.0, 0.0, 1.0};
        for (int iMatrixCoord=0 ; iMatrixCoord<16 ; iMatrixCoord++)
            matrix[iMatrixCoord]=R[iMatrixCoord];
    }
    else
    {
        if ((x==0.0) && (y==1.0) && (z==0.0))
        {
            GLfloat R[]={c,   0.0, -s,  0.0,
                         0.0, 1.0, 0.0, 0.0,
                         s,   0.0, c,   0.0,
                         0.0, 0.0, 0.0, 1.0};
            for (int iMatrixCoord=0 ; iMatrixCoord<16 ; iMatrixCoord++)
                matrix[iMatrixCoord]=R[iMatrixCoord];
        }
        else
        {

            if ((x==0.0) && (y==0.0) && (z==1.0))
            {
                GLfloat R[]={c,   s,   0.0, 0.0,
                             -s,  c,   0.0, 0.0,
                             0.0, 0.0, 1.0, 0.0,
                             0.0, 0.0, 0.0, 1.0};
                for (int iMatrixCoord=0 ; iMatrixCoord<16 ; iMatrixCoord++)
                    matrix[iMatrixCoord]=R[iMatrixCoord];
            }
            else
            {
                GLfloat R[]={ (1.0-c)*(x*x-1.0) + 1.0, (1.0-c)*x*y + (z*s),     (1.0-c)*x*z - (y*s),      0.0,
                              (1.0-c)*x*y - (z*s),     (1.0-c)*(y*y-1.0) + 1.0, (1.0-c)*y*z + (x*s),      0.0,
                              (1.0-c)*x*z + (y*s),     (1.0-c)*y*z - (x*s),     (1.0-c)*(z*z-1.0) + 1.0,  0.0,
                              0.0,                     0.0,                     0.0,                      1.0};
                for (int iMatrixCoord=0 ; iMatrixCoord<16 ; iMatrixCoord++)
                    matrix[iMatrixCoord]=R[iMatrixCoord];
                std::cout<<"Rotation on non standard axis."<<std::endl;
            }
        }
    }
}

/*
 *Sets the provided matrix to a translate matrix on vector t
 */
void setToTranslate(GLfloat * matrix, GLfloat * t)
{
    GLfloat T[]={1.0,   0.0,   0.0,   0.0,
                 0.0,   1.0,   0.0,   0.0,
                 0.0,   0.0,   1.0,   0.0,
                 t[0],  t[1],  t[2],  1.0};
    for (int iMatrixCoord=0 ; iMatrixCoord<16 ; iMatrixCoord++)
        matrix[iMatrixCoord]=T[iMatrixCoord];
}

/*
 * Update the average coordinates with center rotation
 */
void ColorSpaceKeyerOverlay::updateCoordAverageRotation()
{
	//initialize center values
	_coordCenterReferenceDisplay.x = _coordCenterReferenceDisplay.y = _coordCenterReferenceDisplay.z = 0.5;	//initialize center reference coordinates
	_coordAverageDisplay = getData()._averageColor._averageValue;											//initialize average center	
	
	//define axes
	float axisX[3] = {1,0,0};		//define axe X
	float axisY[3] = {0,1,0};		//define axe Y
	
	//Create rotation + translation matrix
	float rotationTranslationMatrixCenter[16];			//initialize
	setToIdentity(rotationTranslationMatrixCenter);		//set matrix center to identity
	//Define rotation matrices
	float rotationReferenceCenterX[16];						//initialize rotation matrix X
	setToRotate(rotationReferenceCenterX,_rotateX, axisX);	//construct rotation matrix X
	float rotationReferenceCenterY[16];						//initialize rotation matrix Y
	setToRotate(rotationReferenceCenterY,_rotateY, axisY);	//construct rotation matrix Y
	//Define translate matrices
	float translateCenter[16];								//initialize translation matrix
	float translationVector[3] = {							//translation vector
		_coordCenterReferenceDisplay.x,						//x 
		_coordCenterReferenceDisplay.y,						//y
		_coordCenterReferenceDisplay.z};					//z
	setToTranslate(translateCenter,translationVector);		//construct translation matrix
	float translateInverseCenter[16];						//initialize inverse translation matrix
	float inverseTranslationVector[3] = {					//invert translation vector
		-_coordCenterReferenceDisplay.x,					//x
		-_coordCenterReferenceDisplay.y,					//y
		-_coordCenterReferenceDisplay.z};					//z
	setToTranslate(translateInverseCenter,inverseTranslationVector);	//construct inverse translation matrix
	
	//Construct final reference center matrix (inverse order of application)
	multMatrixBtoMatrixA(rotationTranslationMatrixCenter, translateInverseCenter);		//inverse translation matrix
	multMatrixBtoMatrixA(rotationTranslationMatrixCenter, rotationReferenceCenterY);	//rotation matrix Y
	multMatrixBtoMatrixA(rotationTranslationMatrixCenter, rotationReferenceCenterX);	//rotation matrix X
	multMatrixBtoMatrixA(rotationTranslationMatrixCenter, translateCenter);				//translation matrix
	
	/*
	std::cout << "display transform matrix" << std::endl;
	for(unsigned int i=0; i<16; ++i)
	{
		std::cout <<rotationTranslationMatrixCenter[i]<<"-"<<std::ends;
		if(i%4 ==0)
			std::cout <<""<<std::endl;
	}
	*/
	//compute average coordinates updated
	double result[3];								//initialize result vector
	double vectorCoordinatesAverageRotation[4] = {	//construct vector with average coordinates
		_coordAverageDisplay.x,						//x value
		_coordAverageDisplay.y,						//y value
		_coordAverageDisplay.z};					//z value
	productVectorMatrix(vectorCoordinatesAverageRotation,(double*)rotationTranslationMatrixCenter,result);//compute new average coordinates
	//get average coordinates updated
	_coordAverageDisplay.x = result[0];				//get x value
	_coordAverageDisplay.y = result[1];				//get y value
	_coordAverageDisplay.z = result[2];				//get z value
}

/*
 * Draw a warning sign on the openGL scene
 */
void ColorSpaceKeyerOverlay::drawWarning(const Ofx3DPointD& centerPoint, const double ratio)
{
	float size = 5.0f;												//define size
	glColor3f(1.0f,.7f,0);											//color orange
	glLineWidth(size);												//change line width (bigger)
	//draw triangle
	glBegin(GL_LINE_STRIP);											//draw exterior triangle
	glVertex2d((centerPoint.x - 0.025*ratio),centerPoint.y);			//first point of triangle
	glVertex2d((centerPoint.x + 0.025*ratio),centerPoint.y);			//second point of triangle
	glVertex2d(centerPoint.x,(centerPoint.y+0.085*ratio));			//third point of triangle
	glVertex2d((centerPoint.x - 0.025*ratio),centerPoint.y);			//first point of triangle (boucle)
	glEnd();														//end of drawing
	//draw !
	glBegin(GL_LINES);												//draw ! sign
	glVertex2d(centerPoint.x, (centerPoint.y+0.07*ratio));			//first point
	glVertex2d(centerPoint.x, (centerPoint.y+0.03*ratio));			//second point
	glEnd();														//end of drawing
	glBegin(GL_POINTS);												//draw ! point
	glPointSize(size);												//change point size (bigger)
	glVertex2d(centerPoint.x, (centerPoint.y + 0.02*ratio));
	glEnd();														//end of drawing
	//reset basic parameters
	glLineWidth(1.0f);												//reset line width (normal)
	glPointSize(1.0f);												//reset point size (normal)
}


}
}
}

