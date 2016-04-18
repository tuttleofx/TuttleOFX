#include <tuttle/common/system/windows/windows.h>

#include <GL/glew.h>
#include <tuttle/plugin/opengl/gl.h>

#include "ColorCubeViewerOverlay.hpp"

namespace tuttle
{
namespace plugin
{
namespace colorCubeViewer
{

/*
 * Constructor
 * @param handle
 * @param effect
 */
ColorCubeViewerOverlay::ColorCubeViewerOverlay(OfxInteractHandle handle, OFX::ImageEffect* effect)
    : OFX::OverlayInteract(handle)
    , _infos(effect)
{
    _plugin = static_cast<ColorCubeViewerPlugin*>(_effect); // get plugin
    _plugin->addRefCloudPointData();                        // create pointer to overlay data

    _isPenDown = false;                          // mouse is not under control by default
    _isCtrlKeyDown = false;                      // Ctrl key is not pressed by default
    _rotateX = _rotateY = 0.0;                   // initialize rotation to 0
    _rotateXForm = _rotateYForm = 0.0;           // initialize rotation centered to geodesic form to 0
    _origin.x = _origin.y = _end.x = _end.y = 0; // initialize mouse positions to 0

    setToIdentity(_modelViewMatrix); // set model view matrix to identity

    /// HACK : to initialize correctly overlay display data
    OFX::InstanceChangedArgs args;                   // create instance changed arguments
    _plugin->changedParam(args, kPointCloudDisplay); // call changed parameters function to initialize overlay data
}

/*
 * Destructor
 */
ColorCubeViewerOverlay::~ColorCubeViewerOverlay()
{
    _plugin->releaseCloudPointData(); // release Overlay data
}

/*
 * Prepare the OpenGL scene for drawing (projection and frustrum settings)
 */
void ColorCubeViewerOverlay::prepareOpenGLScene(const OFX::DrawArgs& args)
{
    // reset OpenGL scene (remove previous image)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // change background color to gray
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    // get projection matrix
    GLdouble proj_matrix[16];
    glGetDoublev(GL_PROJECTION_MATRIX, proj_matrix);
    // initialize model-view and projection matrixes to identity
    glMatrixMode(GL_PROJECTION); // load standard mode
    glLoadIdentity();            // projection to identity
    glMatrixMode(GL_MODELVIEW);  // load standard mode
    glLoadIdentity();            // model-view to identity

    // get current viewport size
    GLint viewport[4] = {0, 0, 0, 0};                                                       // define result array
    glGetIntegerv(GL_VIEWPORT, viewport);                                                   // get current viewport size
    const double ratio = (viewport[2] - viewport[0]) / (double)(viewport[3] - viewport[1]); // compute ratio
    // define new coordinates
    glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0); // set coordinates to 0-1

    if(args.time != getData()._time || args.time != getData()._averageColor._time) // if current overlay has not been updated
    {
        // display warning sign on screen
        Ofx3DPointD warningPoint;         // initialize warning drawing point
        warningPoint.x = 0.15;            // x == viewport width/7;
        warningPoint.y = 0.2;             // y == viewport height/5;
        drawWarning(warningPoint, ratio); // draw warning sign
    }

    // define openGL scene frustrum
    glMatrixMode(GL_PROJECTION); // load standard mode
    glLoadMatrixd(proj_matrix);  // reload previous projection matrix

    const GLdouble vleft = -0.5;                        // frustrum left
    const GLdouble vright = 1.5;                        // frustrum right
    const GLdouble vbottom = -0.5;                      // frustrum bottom
    const GLdouble vtop = 1.5;                          // frustrum top
    const GLdouble vnear = 10.0;                        // frustrum near
    const GLdouble vfar = -10.0;                        // frustrum far
    glOrtho(vleft, vright, vbottom, vtop, vnear, vfar); // define new frustrum for overlay data

    glMatrixMode(GL_MODELVIEW); // load standard mode
    // initialize double* modelViewMatrix
    double modelViewMatrix[16]; // initialize
    for(unsigned int i = 0; i < 16; ++i)
        modelViewMatrix[i] = _modelViewMatrix[i]; // recopy Matrix4 into double*
    glLoadMatrixd(modelViewMatrix);               // load modelView matrix (first time is equal to identity)
}

/*
 * Draw the x,Y and Z axes into the OpenGL scene
 */
void ColorCubeViewerOverlay::drawAxes()
{
    glBegin(GL_LINES);
    // X axis (black => red)
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2i(0, 0); // color and direction (0,0,0)
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2i(1, 0); // color and direction (1,0,0)
    // Y axis (black => green)
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2i(0, 0); // color and direction (0,0,0)
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex2i(0, 1); // color and direction (0,1,0)
    // Z axis (black => blue)
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2i(0, 0); // color and direction (0,0,0)
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3i(0, 0, 1); // color and direction (0,0,1)
    // axis (red => yellow)
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2i(1, 0); // red
    glColor3f(1.0f, 1.0f, 0.0f);
    glVertex2i(1, 1); // yellow
    // axis (red => yellow)
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex2i(0, 1); // green
    glColor3f(1.0f, 1.0f, 0.0f);
    glVertex2i(1, 1); // yellow
    // axis (green => cyan)
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex2i(0, 1); // green
    glColor3f(0.0f, 1.0f, 1.0f);
    glVertex3i(0, 1, 1); // cyan
    // axis (blue => cyan)
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3i(0, 0, 1); // blue
    glColor3f(0.0f, 1.0f, 1.0f);
    glVertex3i(0, 1, 1); // cyan
    // axis (cyan => white)
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex3i(1, 1, 1); // white
    glColor3f(0.0f, 1.0f, 1.0f);
    glVertex3i(0, 1, 1); // cyan
    // axis (yellow => white)
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex3i(1, 1, 1); // white
    glColor3f(1.0f, 1.0f, 0.0f);
    glVertex2i(1, 1); // yellow
    // axis (magenta => white)
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex3i(1, 1, 1); // white
    glColor3f(1.0f, 0.0f, 1.0f);
    glVertex3i(1, 0, 1); // magenta
    // axis (magenta => blue)
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3i(0, 0, 1); // blue
    glColor3f(1.0f, 0.0f, 1.0f);
    glVertex3i(1, 0, 1); // magenta
    // axis (magenta => red)
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2i(1, 0); // red
    glColor3f(1.0f, 0.0f, 1.0f);
    glVertex3i(1, 0, 1); // magenta
    // axis (white => black)
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex3i(1, 1, 1); // white
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2i(0, 0); // black
    glEnd();

    glPointSize(6.0f);
    glBegin(GL_POINTS);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2i(0, 0); // black
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2i(1, 0); // red
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex2i(0, 1); // green
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3i(0, 0, 1); // blue
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex3i(1, 1, 1); // white
    glColor3f(1.0f, 0.0f, 1.0f);
    glVertex3i(1, 0, 1); // magenta
    glColor3f(0.0f, 1.0f, 1.0f);
    glVertex3i(0, 1, 1); // cyan
    glColor3f(1.0f, 1.0f, 0.0f);
    glVertex2i(1, 1); // yellow

    glPointSize(1.0f);
    glEnd();
}

/*
 * Main display of the ColorCubeViewerOverlay class
 * @param args current arg (time/renderScale/pixelScale....
 * @return if there is something drawed by the function (y or n)
 */
bool ColorCubeViewerOverlay::draw(const OFX::DrawArgs& args)
{
    bool displaySomethings = false;
    if(_plugin->_paramBoolPointCloudDisplay->getValue()) // Is CloudPointData displayed ? (GUI)
    {
        glPushMatrix();           // new transformation
        prepareOpenGLScene(args); // prepare frustum and projection settings
        if(_plugin->_updateVBO)   // VBO need to be updated
        {
            // update VBO
            getData().updateVBO();       // update VBO from VBO data (already computed)
            _plugin->_updateVBO = false; // VBO has been recomputed
        }
        if(_plugin->_resetViewParameters) // View parameters need to be reseted
        {
            _rotateX = _rotateY = 0;               // reset parameters
            _rotateXForm = _rotateYForm = 0;       // reset geodesic form center rotation parameters
            _plugin->_resetViewParameters = false; // view parameters has been changed
            setToIdentity(_modelViewMatrix);       // reset model-view matrix to identity
        }
        // OpenGL parameters
        glEnable(GL_DEPTH_TEST); // active depth (better for understand results)

        // drawing Axes
        drawAxes(); // draw the X, Y and Z axes
        // drawing VBO
        if(getData()._isVBOBuilt)     // if VBO has already been built
            getData()._imgVBO.draw(); // draw VBO

        // drawing color selection VBO
        if(getData()._isSelectionVBOBuilt && _plugin->_paramBoolSeeSelection->getValue()) // color selection VBO data is
                                                                                          // built
        {
            glColor3f(1.0f, 1.0f, 1.0f);         // color is white
            getData()._selectionColorVBO.draw(); // draw selection VBO
        }
        // drawing spill selection VBO
        if(getData()._isSpillSelectionVBOBuilt &&
           _plugin->_paramBoolSeeSpillSelection->getValue()) // spill selection VBO data is built
        {
            glColor3f(.3f, .3f, .3f);            // color is white
            getData()._selectionSpillVBO.draw(); // draw selection VBO
        }

        // drawing average
        getData()._averageColor.draw(); // draw average (cross)
        // drawing geodesic form
        if(_plugin->_paramBoolDisplayGeodesicForm->getValue()) // does user want to display color geodesic form
            getData()._geodesicFormColor.draw(false);          // draw geodesic form on screen without alpha
        if(_plugin->_paramBoolDisplaySpillGF->getValue())      // does user want to display spill geodesic form
            getData()._geodesicFormSpill.draw(true);           // draw spill geodesic form on screen with alpha

        // OpenGL end of parameters
        glDisable(GL_DEPTH_TEST); // disable deep
        glPopMatrix();            // pop matrix
        displaySomethings = true; // something has been drown on screen
    }
    return displaySomethings; // return if overlay has displayed something (y or n)
}

/*
 * Capture and treat click down event
 * @param args current argument (time/renderScale/pixelScale....
 * @return if the event has been treated (y or n)
 */
bool ColorCubeViewerOverlay::penDown(const OFX::PenArgs& args)
{
    if(!_isPenDown) // is mouse is not already selected
    {
        _isPenDown = true;                       // active mouse operation (for penMotion)
        _origin.x = _end.x = args.penPosition.x; // capture x position of current click
        _origin.y = _end.y = args.penPosition.y; // capture y position of current click
        return true;                             // event has been captured
    }
    return false; // event has not been captured
}

/*
 * Capture and treat click up event (release mouse)
 * @param args current argument (time/renderScale/pixelScale....
 * @return if the event has been treated (y or n)
 */
bool ColorCubeViewerOverlay::penUp(const OFX::PenArgs& args)
{
    if(_isPenDown) // is mouse is already selected
    {
        _isPenDown = false;          // mouse is no more selected
        _end.x = args.penPosition.x; // capture mouse current position x
        _end.y = args.penPosition.y; // capture mouse current position y

        if(_origin.x == _end.x && _origin.y == _end.y)
            return false; // basic click (there is nothing to do in this case)
        return true;      // event has been captured
    }
    return false; // event has not been captured
}

/*
 * Treat if mouse/pen is under motion (drawing on screen for example)
 * @param args current arg (time/renderScale/pixelScale....
 * @return if the event has been treated (y or n)
 */
bool ColorCubeViewerOverlay::penMotion(const OFX::PenArgs& args)
{
    if(_isPenDown) // if mouse is already selected
    {
        _end.x = args.penPosition.x; // capture mouse current position x
        _end.y = args.penPosition.y; // capture mouse current position y

        int deltaX = _end.x - _origin.x; // compute delta for rotation on Y axis (horizontal)
        int deltaY = _end.y - _origin.y; // compute delta for rotation on X axis (vertical)
        if(_isCtrlKeyDown)               // rotation center is current color selection average
        {
            _rotateXForm = (deltaX / args.pixelScale.x) / kRotationSpeed; // add delta to geodesic center rotation (X axis)
            _rotateYForm = (deltaY / args.pixelScale.y) / kRotationSpeed; // add delta to geodesic center rotation (Y axis)

            // update model-View matrix
            Ofx3DPointD rotationCenter = getData()._geodesicFormColor._center; // get current rotation center
            updateModelView(rotationCenter);                                   // update model-view
        }
        else // rotation center is reference center (0.5,0.5,0.5 in cube reference)
        {
            _rotateX = (deltaX / args.pixelScale.x) / kRotationSpeed; // add delta to cube rotation (X axis)
            _rotateY = (deltaY / args.pixelScale.y) / kRotationSpeed; // add delta to cube rotation (Y axis)

            // update model-View matrix
            Ofx3DPointD rotationCenter;      // create rotation center
            rotationCenter.x = 0.5;          // set rotation center X value 0 by default
            rotationCenter.y = 0.5;          // set rotation center Y value
            rotationCenter.z = 0.5;          // set rotation center Z value
            updateModelView(rotationCenter); // update model-view
        }
        _origin.x = args.penPosition.x; // change origin X (prepare next penMotion)
        _origin.y = args.penPosition.y; // change origin Y (prepare next penMotion)

        return true; // event has been captured
    }
    return false;
}

/*
 * Treat the Ctrl key pressed (down)
 * @param args current arg (time/rendeerScale/...)
 * @return  if the event has been treated (y or n)
 */
bool ColorCubeViewerOverlay::keyDown(const OFX::KeyArgs& args)
{
    if(args.keySymbol == kOfxKey_Control_L ||
       args.keySymbol == kOfxKey_Control_R) // if the pressed key is Ctrl key (left or right)
    {
        _isCtrlKeyDown = true; // Ctrl key is pressed
        return true;           // event has been treated
    }
    return false; // event has not been treated (key pressed is not Ctrl)
}

/*
 * Treat the Ctrl key pressed (up)
 * @param args current arg (time/rendeerScale/...)
 * @return  if the event has been treated (y or n)
 */
bool ColorCubeViewerOverlay::keyUp(const OFX::KeyArgs& args)
{
    if(args.keySymbol == kOfxKey_Control_L ||
       args.keySymbol == kOfxKey_Control_R) // if the pressed key is Ctrl key (left or right)
    {
        if(_isCtrlKeyDown)
            _isCtrlKeyDown = false; // Ctrl key is not pressed anymore
        return true;                // event has been treated
    }
    return false; // event has not been treated (key pressed is not Ctrl)
}

/*
 * Get cloud point data from ColorCubeViewerPlugin
 * @return
 */
CloudPointData& ColorCubeViewerOverlay::getData()
{
    return _plugin->getCloudPointData(); // return CloudPointData initialized at constructor (by scoped pointer)
}

/*
 * Update the average coordinates with center rotation
 */
void ColorCubeViewerOverlay::updateModelView(const Ofx3DPointD& rotationCenter)
{
    // Define and create new transformation matrix
    Matrix4 newTransformationMatrix; // initialize new transformation matrix
    if(rotationCenter.x == 0.5 && rotationCenter.y == 0.5 &&
       rotationCenter.z == 0.5) // if current rotation center is reference center
        newTransformationMatrix =
            constructRotationMatrix(rotationCenter, _rotateY, _rotateX, 0); // construct new transformation matrix
    else // current rotation center is geodesic form center
        newTransformationMatrix =
            constructRotationMatrix(rotationCenter, _rotateYForm, _rotateXForm, 0); // construct new transformation matrix
    // X and Y angles are inverted (better to control view)
    // Add current transformation to model-view matrix
    multMatrixBtoMatrixA(_modelViewMatrix, newTransformationMatrix); // add current transformation to model-view matrix
}

/*
 * Draw a warning sign on the openGL scene
 */
void ColorCubeViewerOverlay::drawWarning(const Ofx3DPointD& centerPoint, const double ratio)
{
    float size = 5.0f;       // define size
    glColor3f(1.0f, .7f, 0); // color orange
    glLineWidth(size);       // change line width (bigger)
    // draw triangle
    glBegin(GL_LINE_STRIP);                                     // draw exterior triangle
    glVertex2d((centerPoint.x - 0.025 * ratio), centerPoint.y); // first point of triangle
    glVertex2d((centerPoint.x + 0.025 * ratio), centerPoint.y); // second point of triangle
    glVertex2d(centerPoint.x, (centerPoint.y + 0.085 * ratio)); // third point of triangle
    glVertex2d((centerPoint.x - 0.025 * ratio), centerPoint.y); // first point of triangle (boucle)
    glEnd();                                                    // end of drawing
    // draw !
    glBegin(GL_LINES);                                         // draw ! sign
    glVertex2d(centerPoint.x, (centerPoint.y + 0.07 * ratio)); // first point
    glVertex2d(centerPoint.x, (centerPoint.y + 0.03 * ratio)); // second point
    glEnd();                                                   // end of drawing
    glBegin(GL_POINTS);                                        // draw ! point
    glPointSize(size);                                         // change point size (bigger)
    glVertex2d(centerPoint.x, (centerPoint.y + 0.02 * ratio)); // point of !
    glEnd();                                                   // end of drawing
    // reset basic parameters
    glLineWidth(1.0f); // reset line width (normal)
    glPointSize(1.0f); // reset point size (normal)
}
}
}
}
