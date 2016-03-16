#ifndef _TUTTLE_PLUGIN_COLOR_CUBE_VIEWER_OVERLAY_HPP
#define _TUTTLE_PLUGIN_COLOR_CUBE_VIEWER_OVERLAY_HPP

#include "ColorCubeViewerPlugin.hpp"
#include "SelectionAverage.hpp"
#include "GeodesicForm.hpp"
#include "MatrixManipulation.hpp"

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/interact/interact.hpp>
#include <tuttle/plugin/interact/InteractInfos.hpp>

#include <ofxsImageEffect.h>
#include <ofxsInteract.h>

namespace tuttle
{
namespace plugin
{
namespace colorCubeViewer
{

class ColorCubeViewerOverlay : public OFX::OverlayInteract
{
public:
    /*Class arguments*/
    ColorCubeViewerPlugin* _plugin; // plugin reference
    interact::InteractInfos _infos;

    OfxPointI _origin; // origin (point of pen down)
    OfxPointI _end; // end (point of pen up)

    bool _isPenDown; // mouse management (is mouse clicked)
    bool _isCtrlKeyDown; // keyboard management (s Ctrl key pressed)

    double _rotateX; // rotation on X axis (mouse)
    double _rotateY; // rotation on Y axis (mouse)
    double _rotateXForm; // rotation on X (center is geodesic form)
    double _rotateYForm; // rotation on Y (center is geodesic form)

    Matrix4 _modelViewMatrix; // Model matrix used to do transformations

public:
    /*Constructor/Destructor*/
    ColorCubeViewerOverlay(OfxInteractHandle handle, OFX::ImageEffect* effect);
    virtual ~ColorCubeViewerOverlay();

    bool draw(const OFX::DrawArgs& args); // draw all of the component (main display)

    /*Mouse management*/
    bool penDown(const OFX::PenArgs& args); // mouse/pen is clicking down
    bool penUp(const OFX::PenArgs& args); // mouse/pen is releasing
    bool penMotion(const OFX::PenArgs& args); // mouse/pen is under motion

    /*Keyboard management*/
    bool keyDown(const OFX::KeyArgs& args); // Ctrl key is pressing down
    bool keyUp(const OFX::KeyArgs& args); // Ctrl key is releasing

private:
    /*OpenGL scene*/
    void
    prepareOpenGLScene(const OFX::DrawArgs& args); // prepare the frustrum and projection settings and initialize first VBO
    void drawAxes(); // draw X, Y and Z axes on screen
    void updateModelView(const Ofx3DPointD& rotationCenter); // update modelView with (by adding a new transformation)
    void drawWarning(const Ofx3DPointD& centerPoint, const double ratio); // draw a warning sign on the openGL scene

    /*Get overlay data*/
    CloudPointData& getData();
};

class ColorCubeViewerOverlayDescriptor : public OFX::EffectOverlayDescriptor
{
public:
    OFX::Interact* createInstance(OfxInteractHandle handle, OFX::ImageEffect* effect)
    {
        return new ColorCubeViewerOverlay(handle, effect);
    }
};
}
}
}

#endif
