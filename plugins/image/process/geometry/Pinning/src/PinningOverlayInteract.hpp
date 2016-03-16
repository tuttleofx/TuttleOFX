#ifndef _PINNINGOVERLAYINTERACT_HPP_
#define _PINNINGOVERLAYINTERACT_HPP_

#include "PinningPlugin.hpp"

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/interact/interact.hpp>
#include <tuttle/plugin/interact/InteractInfos.hpp>
#include <tuttle/plugin/interact/InteractScene.hpp>
//#include <tuttle/plugin/interact/InteractObject.hpp>

#include <ofxsImageEffect.h>
#include <ofxsInteract.h>

namespace tuttle
{
namespace plugin
{
namespace pinning
{

using namespace boost::numeric::ublas;

class PinningOverlayInteract : public OFX::OverlayInteract
{
    typedef double Scalar;

    PinningPlugin* _plugin;

    interact::InteractInfos _infos;
    interact::InteractScene _interactScene;

    bool _beginSelection;
    OfxRectD _multiSelectionRec;
    bool _multiSelection;
    /*
           bool _keyPressed_ctrl;
           bool _keyPressed_shift;
    */

public:
    PinningOverlayInteract(OfxInteractHandle handle, OFX::ImageEffect* effect);

    bool draw(const OFX::DrawArgs& args);

    bool penDown(const OFX::PenArgs& args);
    bool penUp(const OFX::PenArgs& args);
    bool penMotion(const OFX::PenArgs& args);

    // void calculCentre( const std::vector< bounded_vector<double, 2> > pSelect);
    // void rotatePts( std::vector< bounded_vector<double, 2> > pSelect, double angle);
    // void scalePts( std::vector< bounded_vector<double, 2> > pSelect, double coef);
    /*
            bool keyDown( const KeyArgs& args );
            bool keyUp( const KeyArgs& args );
            bool keyRepeat( const KeyArgs& args );
    */
};

class PinningEffectOverlayDescriptor : public OFX::EffectOverlayDescriptor
{
public:
    OFX::Interact* createInstance(OfxInteractHandle handle, OFX::ImageEffect* effect)
    {
        return new PinningOverlayInteract(handle, effect);
    }
};
}
}
}

#endif
