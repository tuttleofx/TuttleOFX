/*
 * Software License :
 *
 * Copyright (c) 2007-2009, The Open Effects Association Ltd.  All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * Neither the name The Open Effects Association Ltd, nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _TUTTLE_HOST_OFX_INTERACT_HPP_
#define _TUTTLE_HOST_OFX_INTERACT_HPP_

#include "property/OfxhSet.hpp"
#include "property/OfxhGetHook.hpp"
#include "property/OfxhNotifyHook.hpp"
#include "OfxhException.hpp"

#include <ofxInteract.h>

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace interact
{

class OfxhInteractBase
{
public:
    OfxhInteractBase()
        : _magic(kMagic)
    {
    }
    virtual ~OfxhInteractBase() = 0;

private:
    static const int kMagic = 0x09012013; ///< magic number for Interact, and current day :-)
    const int _magic;                     ///< to check for handles being nice

public:
    bool verifyMagic() const { return this != NULL && _magic == kMagic; }

    /// grab a handle on the parameter for passing to the C API
    OfxInteractHandle getHandle() { return (OfxInteractHandle) this; }

#ifndef SWIG
    /// get the property handle for this instance/descriptor
    virtual OfxPropertySetHandle getPropHandle() = 0;
#endif
};

/// state the interact can be in
enum State
{
    eUninitialised,
    eDescribed,
    eCreated,
    eFailed
};

/// Descriptor for an interact. Interacts all share a single description
class OfxhInteractDescriptor : public OfxhInteractBase
{
protected:
    property::OfxhSet _properties;    ///< its props
    State _state;                     ///< how is it feeling today
    OfxPluginEntryPoint* _entryPoint; ///< the entry point for this overlay

public:
    /// CTOR
    OfxhInteractDescriptor();

    /// dtor
    virtual ~OfxhInteractDescriptor();

    /// set the main entry points
    void setEntryPoint(OfxPluginEntryPoint* entryPoint) { _entryPoint = entryPoint; }

    /// call describe on this descriptor, returns true if all went well
    bool describe(int bitDepthPerComponent, bool hasAlpha);

    /// grab a handle on the properties of this parameter for the C api
    OfxPropertySetHandle getPropHandle() { return _properties.getHandle(); }

    /// get prop set
    const property::OfxhSet& getProperties() const { return _properties; }

    /// get a non const prop set
    property::OfxhSet& getProperties() { return _properties; }

    /// call the entry point with action and the given args
    OfxStatus callEntry(const char* action, void* handle, OfxPropertySetHandle inArgs, OfxPropertySetHandle outArgs) const;

    /// what is it's state?
    State getState() const { return _state; }
};

/// a generic interact, it doesn't belong to anything in particular
/// we need to generify this slighty more and remove the renderscale args
/// into a derived class, as they only belong to image effect plugins
class OfxhInteract : public OfxhInteractBase, protected property::OfxhGetHook
{
protected:
    const OfxhInteractDescriptor& _descriptor; ///< who we are
    property::OfxhSet _properties;             ///< its props
    State _state;                              ///< how is it feeling today
    void* _effectInstance;                     ///< this is ugly, we need a base class to all plugin instances at some point.
    property::OfxhSet _argProperties;

    /// initialise the argument properties
    void initArgProp(OfxTime time, const OfxPointD& renderScale);

    /// set pen props in the args
    void setPenArgProps(const OfxPointD& penPos, const OfxPointI& penPosViewport, double pressure);

    /// set key args in the props
    void setKeyArgProps(int key, char* keyString);

public:
    OfxhInteract(const OfxhInteractDescriptor& desc, void* effectInstance);

    virtual ~OfxhInteract();

    /// what is it's state?
    State getState() const { return _state; }

    /// grab a handle on the properties of this parameter for the C api
    OfxPropertySetHandle getPropHandle() { return _properties.getHandle(); }

    /// get prop set
    const property::OfxhSet& getProperties() const { return _properties; }

    /// call the entry point in the descriptor with action and the given args
    OfxStatus callEntry(const char* action, property::OfxhSet* inArgs);

    /// hooks to kOfxInteractPropViewportSize in the property set
    /// this is actually redundant and is to be deprecated
    virtual void getViewportSize(int& width, int& height) const = 0;

    // hooks to live kOfxInteractPropPixelScale in the property set
    virtual void getPixelScale(double& xScale, double& yScale) const = 0;

    // hooks to kOfxInteractPropBackgroundColour in the property set
    virtual void getBackgroundColour(double& r, double& g, double& b) const = 0;

    /// implement
    virtual void swapBuffers() OFX_EXCEPTION_SPEC = 0;

    /// implement this
    virtual void redraw() OFX_EXCEPTION_SPEC = 0;

    /// returns the params the interact uses
    virtual void getSlaveToParam(std::vector<std::string>& params) const;

    // don't know what to do
    virtual void reset(const std::string& name) OFX_EXCEPTION_SPEC;

    /// call create instance
    virtual void createInstanceAction() OFX_EXCEPTION_SPEC;

    // interact action - kOfxInteractActionDraw
    //
    // Params -
    //
    //    time              - the effect time at which changed occured
    //    renderScale       - the render scale
    virtual void drawAction(OfxTime time, const OfxPointD& renderScale) OFX_EXCEPTION_SPEC;

    // interact action - kOfxInteractActionPenMotion
    //
    // Params  -
    //
    //    time              - the effect time at which changed occured
    //    renderScale       - the render scale
    //    penX              - the X position
    //    penY              - the Y position
    //    pressure          - the pen pressue 0 to 1
    virtual void penMotionAction(OfxTime time, const OfxPointD& renderScale, const OfxPointD& penPos,
                                 const OfxPointI& penPosViewport, double pressure) OFX_EXCEPTION_SPEC;

    // interact action - kOfxInteractActionPenUp
    //
    // Params  -
    //
    //    time              - the effect time at which changed occured
    //    renderScale       - the render scale
    //    penX              - the X position
    //    penY              - the Y position
    //    pressure          - the pen pressue 0 to 1
    virtual void penUpAction(OfxTime time, const OfxPointD& renderScale, const OfxPointD& penPos,
                             const OfxPointI& penPosViewport, double pressure) OFX_EXCEPTION_SPEC;

    // interact action - kOfxInteractActionPenDown
    //
    // Params  -
    //
    //    time              - the effect time at which changed occured
    //    renderScale       - the render scale
    //    penX              - the X position
    //    penY              - the Y position
    //    pressure          - the pen pressue 0 to 1
    virtual void penDownAction(OfxTime time, const OfxPointD& renderScale, const OfxPointD& penPos,
                               const OfxPointI& penPosViewport, double pressure) OFX_EXCEPTION_SPEC;

    // interact action - kOfxInteractActionkeyDown
    //
    // Params  -
    //
    //    time              - the effect time at which changed occured
    //    renderScale       - the render scale
    //    key               - the pressed key
    //    keyString         - the pressed key string
    virtual void keyDownAction(OfxTime time, const OfxPointD& renderScale, int key, char* keyString) OFX_EXCEPTION_SPEC;

    // interact action - kOfxInteractActionkeyUp
    //
    // Params  -
    //
    //    time              - the effect time at which changed occured
    //    renderScale       - the render scale
    //    key               - the pressed key
    //    keyString         - the pressed key string
    virtual void keyUpAction(OfxTime time, const OfxPointD& renderScale, int key, char* keyString) OFX_EXCEPTION_SPEC;

    // interact action - kOfxInteractActionkeyRepeat
    //
    // Params  -
    //
    //    time              - the effect time at which changed occured
    //    renderScale       - the render scale
    //    key               - the pressed key
    //    keyString         - the pressed key string
    virtual void keyRepeatAction(OfxTime time, const OfxPointD& renderScale, int key, char* keyString) OFX_EXCEPTION_SPEC;

    // interact action - kOfxInteractActionLoseFocus
    //
    // Params -
    //
    //    time              - the effect time at which changed occured
    //    renderScale       - the render scale
    virtual void gainFocusAction(OfxTime time, const OfxPointD& renderScale) OFX_EXCEPTION_SPEC;

    // interact action - kOfxInteractActionLoseFocus
    //
    // Params -
    //
    //    time              - the effect time at which changed occured
    //    renderScale       - the render scale
    virtual void loseFocusAction(OfxTime time, const OfxPointD& renderScale) OFX_EXCEPTION_SPEC;
};
}
}
}
}

#endif
