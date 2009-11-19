/*
Software License :

Copyright (c) 2007, The Open Effects Association Ltd. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name The Open Effects Association Ltd, nor the names of its 
      contributors may be used to endorse or promote products derived from this
      software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <iostream>
#include <fstream>

// ofx
#include "ofxCore.h"
#include "ofxImageEffect.h"

// ofx host
#include "ofx/ofxhBinary.h"
#include "ofx/ofxhPropertySuite.h"
#include "ofx/ofxhClip.h"
#include "ofx/ofxhParam.h"
#include "ofx/ofxhMemory.h"
#include "ofx/ofxhImageEffect.h"
#include "ofx/ofxhPluginAPICache.h"
#include "ofx/ofxhPluginCache.h"
#include "ofx/ofxhHost.h"
#include "ofx/ofxhImageEffectAPI.h"

// my host
#include "HostDescriptor.hpp"
#include "EffectInstance.hpp"
#include "ClipInstance.hpp"
#include "ParamInstance.hpp"

using namespace OFX;
// my host support code
namespace tuttle {
    EffectInstance::EffectInstance(OFX::Host::ImageEffect::ImageEffectPlugin* plugin,
                                     OFX::Host::ImageEffect::Descriptor& desc,
                                     const std::string& context)
                                     : OFX::Host::ImageEffect::Instance(plugin,desc,context,false)
    {
    }

    // class member function implementation


    void EffectInstance::dumpToStdOut()
    {
        std::cout << "________________________________________________________________________________" << std::endl;
        std::cout << "Plug-in:" << this->getLabel() << std::endl;
        std::cout << "Description:" << this->getLongLabel() << std::endl;
        std::cout << "Context:" << this->_context << std::endl;
        std::cout << "Clips:" << std::endl;
        for (std::map<std::string, OFX::Host::Attribute::ClipImageInstance*>::const_iterator it = this->_clips.begin(); it != this->_clips.end(); ++it)
        {
            std::cout << "\t\t* " << it->first << std::endl;
        }
        std::cout << "Params:" << std::endl;
        for (std::list<OFX::Host::Attribute::ParamInstance*>::const_iterator it = this->_paramList.begin(); it != this->_paramList.end(); ++it)
        {
            std::cout << "\t\t* " << (*it)->getLabel() << std::endl;
        }

        std::cout << "________________________________________________________________________________" << std::endl;
    }

    // get a new clip instance
    OFX::Host::Attribute::ClipImageInstance* EffectInstance::newClipInstance(OFX::Host::ImageEffect::Instance* plugin,
                                                                          OFX::Host::Attribute::ClipImageDescriptor* descriptor,
                                                                          int index)
    {
        return new ClipImgInstance(this,descriptor);
    }


    /// get default output fielding. This is passed into the clip prefs action
    /// and  might be mapped (if the host allows such a thing)
    const std::string &EffectInstance::getDefaultOutputFielding() const
    {
        /// our clip is pretending to be progressive PAL SD, so return kOfxImageFieldNone
        static const std::string v(kOfxImageFieldNone);
        return v;
    }

    // vmessage
    OfxStatus EffectInstance::vmessage(const char* type,
                                       const char* id,
                                       const char* format,
                                       va_list args)
    {
        vprintf(format,args);
        return kOfxStatOK;
    }

    // get the project size in CANONICAL pixels, so PAL SD return 768, 576
    void EffectInstance::getProjectSize(double& xSize, double& ySize) const
    {
        xSize = 720;
        ySize = 576;
    }

    // get the project offset in CANONICAL pixels, we are at 0,0
    void EffectInstance::getProjectOffset(double& xOffset, double& yOffset) const
    {
        xOffset = 0;
        yOffset = 0;
    }

    // get the project extent in CANONICAL pixels, so PAL SD return 768, 576
    void EffectInstance::getProjectExtent(double& xSize, double& ySize) const
    {
        xSize = 720.0;
        ySize = 576.0;
    }

    // get the PAR, SD PAL is 768/720=1.0666
    double EffectInstance::getProjectPixelAspectRatio() const
    {
        return 720.0 / 720.0;
    }

    // we are only 25 frames
    double EffectInstance::getEffectDuration() const
    {
        return 25.0;
    }

    // get frame rate, so progressive PAL SD return 25
    double EffectInstance::getFrameRate() const
    {
        return 25.0;
    }

    /// This is called whenever a param is changed by the plugin so that
    /// the recursive instanceChangedAction will be fed the correct frame
    double EffectInstance::getFrameRecursive() const
    {
        return 0.0;
    }

    /// This is called whenever a param is changed by the plugin so that
    /// the recursive instanceChangedAction will be fed the correct
    /// renderScale
    void EffectInstance::getRenderScaleRecursive(double &x, double &y) const
    {
        x = y = 1.0;
    }
    // The pixel components type of the current project
    // @OFX_TODO : to remove in the future.... size, pixelType, BitDepth, etc... must be locally defined
    const std::string EffectInstance::getProjectPixelComponentsType() const {
        return kOfxImageComponentRGBA;
    }

    // The pixel bit depth of the current project (host work in float)
    // @OFX_TODO : to remove in the future.... size, pixelType, BitDepth, etc... must be locally defined
    const std::string EffectInstance::getProjectBitDepth() const {
        return kOfxBitDepthFloat;
    }

    // make a parameter instance
    OFX::Host::Attribute::ParamInstance* EffectInstance::newParam(const std::string& name, OFX::Host::Attribute::ParamDescriptor& descriptor, OFX::Host::Attribute::ParamInstanceSet * setInstance)
    {
        if(descriptor.getParamType()==kOfxParamTypeString)
          return new StringInstance(this, name, descriptor, *setInstance);
        else if(descriptor.getParamType()==kOfxParamTypeInteger)
          return new IntegerInstance(this, name, descriptor, *setInstance);
        else if(descriptor.getParamType()==kOfxParamTypeDouble)
          return new DoubleInstance(this, name, descriptor, *setInstance);
        else if(descriptor.getParamType()==kOfxParamTypeBoolean)
          return new BooleanInstance(this, name, descriptor, *setInstance);
        else if(descriptor.getParamType()==kOfxParamTypeChoice)
          return new ChoiceInstance(this, name, descriptor, *setInstance);
        else if(descriptor.getParamType()==kOfxParamTypeRGBA)
          return new RGBAInstance(this, name, descriptor, *setInstance);
        else if(descriptor.getParamType()==kOfxParamTypeRGB)
          return new RGBInstance(this, name, descriptor, *setInstance);
        else if(descriptor.getParamType()==kOfxParamTypeDouble2D)
          return new Double2DInstance(this, name, descriptor, *setInstance);
        else if(descriptor.getParamType()==kOfxParamTypeInteger2D)
          return new Integer2DInstance(this, name, descriptor, *setInstance);
        else if(descriptor.getParamType()==kOfxParamTypePushButton)
          return new PushbuttonInstance(this, name, descriptor, *setInstance);
        else if(descriptor.getParamType()==kOfxParamTypeGroup)
          return new OFX::Host::Attribute::ParamGroupInstance(descriptor, *setInstance);
        else if(descriptor.getParamType()==kOfxParamTypePage)
          return new OFX::Host::Attribute::ParamPageInstance(descriptor, *setInstance);
        else
          return 0;
    }

    OfxStatus EffectInstance::editBegin(const std::string& name)
    {
        return kOfxStatOK; // OFX_TODO
        return kOfxStatErrMissingHostFeature;
    }

    OfxStatus EffectInstance::editEnd()
    {
        return kOfxStatOK; // OFX_TODO
        return kOfxStatErrMissingHostFeature;
    }

    /// Start doing progress.
    void  EffectInstance::progressStart(const std::string &message)
    {
    }

    /// finish yer progress
    void  EffectInstance::progressEnd()
    {
    }

    /// set the progress to some level of completion, returns
    /// false if you should abandon processing, true to continue
    bool  EffectInstance::progressUpdate(double t)
    {
        return true;
    }


    /// get the current time on the timeline. This is not necessarily the same
    /// time as being passed to an action (eg render)
    double  EffectInstance::timeLineGetTime()
    {
    return 0;
    }

    /// set the timeline to a specific time
    void  EffectInstance::timeLineGotoTime(double t)
    {
    }

    /// get the first and last times available on the effect's timeline
    void  EffectInstance::timeLineGetBounds(double &t1, double &t2)
    {
        t1 = 0;
        t2 = 25;
    }

    /// override to get frame range of the effect
    OfxStatus EffectInstance::beginRenderAction(OfxTime  startFrame,
                                                OfxTime  endFrame,
                                                OfxTime  step,
                                                bool     interactive,
                                                OfxPointD   renderScale) {
        _frameRange.x = startFrame;
        _frameRange.y = endFrame;
        return Instance::beginRenderAction(startFrame, endFrame, step, interactive, renderScale);
    }
}
