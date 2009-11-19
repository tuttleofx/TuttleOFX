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
#include <ofxCore.h>
#include <ofxImageEffect.h>

// ofx host
#include <tuttle/host/ofx/ofxhBinary.h>
#include <tuttle/host/ofx/ofxhPropertySuite.h>
#include <tuttle/host/ofx/ofxhClip.h>
#include <tuttle/host/ofx/ofxhParam.h>
#include <tuttle/host/ofx/ofxhMemory.h>
#include <tuttle/host/ofx/ofxhImageEffect.h>
#include <tuttle/host/ofx/ofxhPluginAPICache.h>
#include <tuttle/host/ofx/ofxhPluginCache.h>
#include <tuttle/host/ofx/ofxhHost.h>
#include <tuttle/host/ofx/ofxhImageEffectAPI.h>

// my host
#include "HostDescriptor.hpp"
#include "EffectInstance.hpp"
#include "ClipInstance.hpp"
#include "ParamInstance.hpp"

namespace tuttle {
namespace host {
namespace core {

// my host support code
    EffectInstance::EffectInstance(tuttle::host::ofx::imageEffect::ImageEffectPlugin* plugin,
                                     tuttle::host::ofx::imageEffect::Descriptor& desc,
                                     const std::string& context)
                                     : tuttle::host::ofx::imageEffect::Instance(plugin,desc,context,false)
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
        for (std::map<std::string, tuttle::host::ofx::attribute::ClipImageInstance*>::const_iterator it = this->_clips.begin(); it != this->_clips.end(); ++it)
        {
            std::cout << "\t\t* " << it->first << std::endl;
        }
        std::cout << "Params:" << std::endl;
        for (std::list<tuttle::host::ofx::attribute::ParamInstance*>::const_iterator it = this->_paramList.begin(); it != this->_paramList.end(); ++it)
        {
            std::cout << "\t\t* " << (*it)->getLabel() << std::endl;
        }

        std::cout << "________________________________________________________________________________" << std::endl;
    }

    // get a new clip instance
    tuttle::host::ofx::attribute::ClipImageInstance* EffectInstance::newClipInstance(tuttle::host::ofx::imageEffect::Instance* plugin,
                                                                          tuttle::host::ofx::attribute::ClipImageDescriptor* descriptor,
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
    tuttle::host::ofx::attribute::ParamInstance* EffectInstance::newParam(const std::string& name, tuttle::host::ofx::attribute::ParamDescriptor& descriptor, tuttle::host::ofx::attribute::ParamInstanceSet * setInstance)
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
          return new tuttle::host::ofx::attribute::ParamGroupInstance(descriptor, *setInstance);
        else if(descriptor.getParamType()==kOfxParamTypePage)
          return new tuttle::host::ofx::attribute::ParamPageInstance(descriptor, *setInstance);
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
}
}
