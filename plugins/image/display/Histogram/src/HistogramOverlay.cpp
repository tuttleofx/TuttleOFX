#include "HistogramOverlay.hpp"
#include "HistogramPlugin.hpp"
#include "HistogramDisplay.hpp"

namespace tuttle
{
namespace plugin
{
namespace histogram
{

HistogramOverlay::HistogramOverlay(OfxInteractHandle handle, OFX::ImageEffect* effect)
    : OFX::OverlayInteract(handle)
    , _plugin(static_cast<HistogramPlugin*>(_effect))
    , _infos(effect)
    , _hslParam(_plugin)
    , _rgbParam(_plugin)
{
    _penDown = false; // Mouse is not clicked down by default
    _keyDown = false; // Ctrl key is not pressed by default
    _plugin->addRefOverlayData(); // add reference to Overlay data
    _isFirstTime = true; // temporary
    getOverlayData()._isDataInvalid = true;
}

HistogramOverlay::~HistogramOverlay()
{
    _plugin->releaseOverlayData(); // release Overlay data
}

bool HistogramOverlay::draw(const OFX::DrawArgs& args)
{
    const OfxPointI fullImgSize = _plugin->_clipSrc->getPixelRodSize(args.time);
    const OfxPointI imgSize = _plugin->_clipSrc->getPixelRodSize(args.time, args.renderScale);
    const OfxRectI pixelRegionOfDefinition = _plugin->_clipSrc->getPixelRod(args.time);

    // Global display option
    if(_plugin->_paramGlobalDisplaySelection->getValue() == false)
        return false;

    if(_isFirstTime || getOverlayData()._isDataInvalid || getOverlayData().isCurrentTimeModified(args.time) ||
       getOverlayData().isImageSizeModified(imgSize) ///< HACK changeClip method doesn't work in nuke when source clip is
                                                     ///changed so we have to check size of imgBool all of the time
       )
    {
        if(getOverlayData().isImageSizeModified(imgSize))
        {
            getOverlayData().clearAll(imgSize);
        }

        if(!_plugin->getIsRendering())
        {
            getOverlayData()._isDataInvalid = false;
            getOverlayData().computeFullData(_plugin->_clipSrc, args.time, args.renderScale);
        }
        else // Data is not updated : draw warning signal
        {
            // initialize model-view and projection matrixes to identity
            glMatrixMode(GL_PROJECTION); // load standard mode
            glLoadIdentity(); // projection to identity
            glMatrixMode(GL_MODELVIEW); // load standard mode
            glLoadIdentity(); // model-view to identity

            // get current viewport size
            GLint viewport[4] = {0, 0, 0, 0}; // define result array
            glGetIntegerv(GL_VIEWPORT, viewport); // get current viewport size
            const double ratio = (viewport[2] - viewport[0]) / (double)(viewport[3] - viewport[1]); // compute ratio
            // define new coordinates
            glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0); // set coordinates to 0-1

            // display warning sign on screen
            Ofx3DPointD warningPoint; // initialize warning drawing point
            warningPoint.x = 0.15; // x == viewport width/7;
            warningPoint.y = 0.2; // y == viewport height/5;
            drawWarning(warningPoint, ratio); // draw warning sign
        }
    }

    // Draw component
    bool displaySomething = false;
    if(_plugin->_clipSrc->isConnected())
    {
        displaySomething = true;

        if(_plugin->_paramDisplaySelection->getValue()) // DisplaySelection
            this->displaySelectedAreas(fullImgSize, imgSize, pixelRegionOfDefinition);

        ///@todo : remove next lines when Nuke curves overlay works
        glPushMatrix();
        glTranslatef(-(fullImgSize.x + kTranslationRGB), 0.0f, 0.0f);

        _rgbParam.draw(args);
        glPopMatrix();

        ///@todo : remove next lines when Nuke curves overlay works
        glPushMatrix();
        glTranslatef((fullImgSize.x + kTranslationHSL), 0.0f, 0.0f);

        _hslParam.draw(args);
        glPopMatrix();

        if(_penDown && !_keyDown) // Display selection zone
        {
            this->displaySelectionZone();
        }
    }

    if(_isFirstTime)
    {
        _isFirstTime = false;
    }
    return displaySomething;
}

/**
 * When the pen/mouse is under motion
 * @param args current arg
 * @return event capted (y or n)
 */
bool HistogramOverlay::penMotion(const OFX::PenArgs& args)
{
    if(_penDown && !_keyDown) // the mouse is moving but there is not Ctrl key pressed
    {
        _squareEnd.x = args.penPosition.x; // needed to draw the selection square
        _squareEnd.y = args.penPosition.y; // needed to draw the selection square
        return true; // event captured
    }
    if(_penDown && _keyDown) // the mouse is moving and there is Ctrl key pressed
    {
        const OfxRectI pixelRegionOfDefinition =
            _plugin->_clipSrc->getPixelRod(args.time, args.renderScale); // pixel region of definition
        int y = args.penPosition.y * args.renderScale.y;
        int x = args.penPosition.x * args.renderScale.x;

        if(y > static_cast<int>(getOverlayData()._imgBool.shape()[0]) || y < 0 ||
           x > static_cast<int>(getOverlayData()._imgBool.shape()[1]) || x < 0)
        {
            return false;
        }
        y -= pixelRegionOfDefinition.y1; // repere change (reformat)
        x -= pixelRegionOfDefinition.x1; // repere change (reformat)

        if(_plugin->_paramSelectionMode->getValue() == 2) // selection mode is subtractive mode
            getOverlayData()._imgBool[y][x] = 0; // current pixel is no more marked as selected
        else
            getOverlayData()._imgBool[y][x] = 255; // current pixel is marked as selected
        return true; // event captured
    }
    return false; // event is not captured
}

/**
 * When the pen/mouse is clicking down
 * @param args current arg
 * @return event capted (y or n)
 */
bool HistogramOverlay::penDown(const OFX::PenArgs& args)
{
    const OfxPointI fullsize = _plugin->_clipSrc->getPixelRodSize(args.time); // full size
    const OfxPointI imgSize = _plugin->_clipSrc->getPixelRodSize(args.time, args.renderScale); // size with renderscale

    if(!_penDown && !_keyDown &&
       _plugin->_paramDisplaySelection->getValue()) // mouse is already used and there is not Ctrl key pressed
    {
        _penDown = true;
        if(args.penPosition.y < fullsize.y && args.penPosition.y > 0) // mouse Y is into the image
            _origin.y = args.penPosition.y * args.renderScale.y;
        else
        {
            if(args.penPosition.y > fullsize.y) // clamp the selected Y pixel to the image borders
                _origin.y = imgSize.y; // click is on the top of the image
            else
                _origin.y = 0; // click is on the bottom of the image
        }
        if(args.penPosition.x < fullsize.x && args.penPosition.x > 0) // mouse X is on the image
            _origin.x = args.penPosition.x * args.renderScale.x;
        else
        {
            if(args.penPosition.x > fullsize.x) // clamp the selected X pixel to the image borders
                _origin.x = imgSize.x; // click is on the right of the image
            else
                _origin.x = 0; // click is on the left of the image
        }
        _end.x = args.penPosition.x * args.renderScale.x; // set X end of the selection square at the origin (initialization)
        _end.y = args.penPosition.y * args.renderScale.y; // set Y end of the selection square at the origin (initialization)

        _squareBegin.x = _squareEnd.x = args.penPosition.x; // copy x value to square position
        _squareBegin.y = _squareEnd.y = args.penPosition.y; // copy v value to square position
    }
    else // there is Ctrl key pressed
    {
        if(!_penDown && _plugin->_paramDisplaySelection->getValue())
            _penDown = true;
    }
    if(_plugin->_paramSelectionMode->getValue() == 1) // Selection mode is unique
    {
        getOverlayData().clearSelection(); // reset past selection
    }
    return true;
}

/**
 * When the pen/mouse is clicking up
 * @param args current arg
 * @return event capted (y or n)
 */
bool HistogramOverlay::penUp(const OFX::PenArgs& args)
{
    const OfxPointI fullSize = _plugin->_clipSrc->getPixelRodSize(args.time); // full image size
    const OfxPointI imgSize = _plugin->_clipSrc->getPixelRodSize(args.time, args.renderScale); // size with renderscale
    const OfxRectI pixelRegionOfDefinition =
        _plugin->_clipSrc->getPixelRod(args.time, args.renderScale); // pixel region of definition

    // clamp selection
    _end.x = args.penPosition.x * args.renderScale.x; // attach the end of the selection square to the current pixel X
    _end.y = args.penPosition.y * args.renderScale.y; // attach the end of the selection square to the current pixel Y
    if(_end.x == _origin.x && _end.y == _origin.y) // it's just one click!
    {
        _penDown = false; // change penDown
        return false; // event is not capured
    }

    if(!(args.penPosition.x < fullSize.x && args.penPosition.x > 0 && args.penPosition.y < fullSize.y &&
         args.penPosition.y > 0)) // if click is not on the image
    {
        if(args.penPosition.x < 0.0 || args.penPosition.x > fullSize.x) // problem with X axis
        {
            if(args.penPosition.x < 0.0) // click is on the left of the image
                _end.x = 0.0; // clamp
            else
                _end.x = imgSize.x; // click is on the right of the image
        }
        if(args.penPosition.y < 0.0 || args.penPosition.y > fullSize.y) // problem with Y axis
        {
            if(args.penPosition.y < 0.0) // click is on the bottom of the image
                _end.y = 0.0; // clamp
            else
                _end.y = imgSize.y; // click is on the top of the image
        }
    }

    if(_penDown && !_keyDown) // if there is not Ctrl key pressed
    {
        int startX, endX, startY, endY;
        if(_origin.x > _end.x) // transform selection zone to be OK (X axis)
        {
            startX = _origin.x;
            endX = _end.x;
        }
        else
        {
            endX = _origin.x;
            startX = _end.x;
        }

        if(_origin.y > _end.y) // transform selection zone to be OK (Y axis)
        {
            startY = _origin.y;
            endY = _end.y;
        }
        else
        {
            endY = _origin.y;
            startY = _end.y;
        }
        const int step_x = startX - endX; // determinate width of the selected zone
        const int step_y = startY - endY; // determinate height of the selected zone

        BOOST_ASSERT(endY >= 0);
        BOOST_ASSERT(endX >= 0);
        BOOST_ASSERT(getOverlayData()._imgBool.shape()[0] >= std::size_t(endY + step_y));
        BOOST_ASSERT(getOverlayData()._imgBool.shape()[1] >= std::size_t(endX + step_x));

        unsigned char fillValue;
        if(_plugin->_paramSelectionMode->getValue() == 2) // selection mode is subtractive
            fillValue = 0; // remove all of the selected pixel
        else
            fillValue = 255; // mark all of the selected pixel

        for(int val_y = 0; val_y < step_y; ++val_y)
        {
            for(int val_x = 0; val_x < step_x; ++val_x)
            {
                const int y = endY + val_y - pixelRegionOfDefinition.y1; // y in img bool size (reformat)
                const int x = endX + val_x - pixelRegionOfDefinition.x1; // x in img bool size (reformat)

                getOverlayData()._imgBool[y][x] = fillValue;
            }
        }
        // recompute full data
        // getOverlayData().computeFullData(_plugin->_clipSrc,args.time,args.renderScale);
        getOverlayData()._isDataInvalid = true;
        _plugin->redrawOverlays();
    }
    _penDown = false; // treatment is finished
    return true;
}

/**
 * Ctrl key is pressed down
 * @param args current arg
 * @return event capted (y or n)
 */
bool HistogramOverlay::keyDown(const OFX::KeyArgs& args)
{
    if(args.keySymbol == kOfxKey_Control_L ||
       args.keySymbol == kOfxKey_Control_R) // if the pressed key is Ctrl key (left or right)
    {
        _keyDown = true; // treatment begins
        return true; // event captured
    }
    return false; // event is not captured (other key)
}

/**
 * Ctrl key is released
 * @param args current arg
 * @return event capted (y or n)
 */
bool HistogramOverlay::keyUp(const OFX::KeyArgs& args)
{
    if((args.keySymbol == kOfxKey_Control_L || args.keySymbol == kOfxKey_Control_R) &&
       _keyDown) // if the release key is Ctrl (and it has been pressed before)
    {
        _keyDown = false; // treatment ends
        _penDown = false; // pen down

        //		getOverlayData().computeFullData( _plugin->_clipSrc, args.time, args.renderScale );
        getOverlayData()._isDataInvalid = true;
        _plugin->redrawOverlays();

        return true; // event captured
    }
    return false; // event is not captured (wrong key)
}

/**
 * Display the selected areas on the clip (color : gray)
 */
void HistogramOverlay::displaySelectedAreas(const OfxPointI& fullImgSize, const OfxPointI& imgSize, const OfxRectI& pixelRoD)
{
    glEnable(GL_TEXTURE_2D); // Activate texturing
    GLuint Name; // Texture name
    glGenTextures(1, &Name); // generate a texture number
    glBindTexture(GL_TEXTURE_2D, Name);
    BOOST_ASSERT(getOverlayData()._imgBool.shape()[0] == std::size_t(imgSize.y));
    BOOST_ASSERT(getOverlayData()._imgBool.shape()[1] == std::size_t(imgSize.x));
    glTexImage2D(GL_TEXTURE_2D, // Type : texture 2D
                 0, // Mipmap : none
                 GL_ALPHA8, // Colors : 4
                 imgSize.x, // width
                 imgSize.y, // height
                 0, // border size
                 GL_ALPHA, // Format : RGBA
                 GL_UNSIGNED_BYTE, // color kind
                 getOverlayData()._imgBool.data() // data buffer
                 );
    glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // Draw Texture on screen
    glBegin(GL_QUADS);
    glTexCoord2d(0, 1);
    glVertex2i(pixelRoD.x1, pixelRoD.y2); // glVertex2d(0,fullImgSize.y);				//Top Left
    glTexCoord2d(1, 1);
    glVertex2i(pixelRoD.x2, pixelRoD.y2); // glVertex2d(fullImgSize.x,fullImgSize.y);	//Top Right
    glTexCoord2d(1, 0);
    glVertex2i(pixelRoD.x2, pixelRoD.y1); // glVertex2d(fullImgSize.x,0);				//Bottom Right
    glTexCoord2d(0, 0);
    glVertex2i(pixelRoD.x1, pixelRoD.y1); // glVertex2d(0,0);							//Bottom Left
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 0.0f, 0.0f);
}

/**
 *Display the selection zone on the clip (color : border gray)
 */
void HistogramOverlay::displaySelectionZone()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, (short)0x0101); // to draw -------
    glBegin(GL_LINE_LOOP);
    glColor3f(.6f, 0.6f, 0.6f); // white
    glVertex2f(_squareBegin.x, _squareBegin.y); // draw selection square
    glVertex2f(_squareBegin.x, _squareEnd.y);
    glVertex2f(_squareEnd.x, _squareEnd.y);
    glVertex2f(_squareEnd.x, _squareBegin.y);
    glEnd();
    glDisable(GL_LINE_STIPPLE);
    glDisable(GL_BLEND);
    glEnd();
}

/*
 * Draw a warning sign on the openGL scene
 */
void HistogramOverlay::drawWarning(const Ofx3DPointD& centerPoint, const double ratio)
{
    float size = 5.0f; // define size
    glColor3f(1.0f, .7f, 0); // color orange
    glLineWidth(size); // change line width (bigger)
    // draw triangle
    glBegin(GL_LINE_STRIP); // draw exterior triangle
    glVertex2d((centerPoint.x - 0.025 * ratio), centerPoint.y); // first point of triangle
    glVertex2d((centerPoint.x + 0.025 * ratio), centerPoint.y); // second point of triangle
    glVertex2d(centerPoint.x, (centerPoint.y + 0.085 * ratio)); // third point of triangle
    glVertex2d((centerPoint.x - 0.025 * ratio), centerPoint.y); // first point of triangle (boucle)
    glEnd(); // end of drawing
    // draw !
    glBegin(GL_LINES); // draw ! sign
    glVertex2d(centerPoint.x, (centerPoint.y + 0.07 * ratio)); // first point
    glVertex2d(centerPoint.x, (centerPoint.y + 0.03 * ratio)); // second point
    glEnd(); // end of drawing
    glBegin(GL_POINTS); // draw ! point
    glPointSize(size); // change point size (bigger)
    glVertex2d(centerPoint.x, (centerPoint.y + 0.02 * ratio)); // point of !
    glEnd(); // end of drawing
    // reset basic parameters
    glLineWidth(1.0f); // reset line width (normal)
    glPointSize(1.0f); // reset point size (normal)
}

/**
 * Get overlay data from plugin
 * @return
 */
OverlayData& HistogramOverlay::getOverlayData()
{
    return _plugin->getOverlayData();
}
}
}
}
