#include "CTLAlgorithm.hpp"
#include "CTLProcess.hpp"
#include "CTLPlugin.hpp"

#include <half.h>
#include <Iex.h>
#include <CtlMessage.h>

namespace tuttle
{
namespace plugin
{
namespace ctl
{

namespace
{

CTLPlugin* ctlPlugin;

void ctlMessageOutput(const std::string& message)
{
    if(ctlPlugin)
    {
        ctlPlugin->sendMessage(OFX::Message::eMessageMessage, "CTL message", message);
    }
}

template <class Type>
void fillInputArg(Ctl::FunctionArgPtr& arg, const std::string& argStr, const Type& v, const std::size_t n)
{
    if(!arg ||
       //		!arg->type().cast<half>() ||
       !arg->isVarying())
    {
        // The CTL function has no argument argStr, the argument
        // is not of type half, or the argument is not varying
        BOOST_THROW_EXCEPTION(Iex::ArgExc(std::string("Cannot set value of argument ") + argStr));
    }

    memcpy(arg->data(), &v, n * sizeof(Type));
}

template <class Type>
void retrieveOutputArg(const Ctl::FunctionArgPtr& arg, const std::string& argStr, Type& v, const std::size_t n)
{
    if(!arg ||
       //		!arg->type( ).cast<half>() ||
       !arg->isVarying())
    {
        // The CTL function has no argument argStr, the argument
        // is not of type half, or the argument is not varying
        BOOST_THROW_EXCEPTION(Iex::ArgExc(std::string("Cannot set value of argument ") + argStr));
    }

    memcpy(&v, arg->data(), n * sizeof(Type));
}

template <class Type>
void callCtlChunk(Ctl::FunctionCallPtr call, const std::size_t n, Type& rOut, Type& gOut, Type& bOut, Type& aOut,
                  const Type& r, const Type& g, const Type& b, const Type& a)
{
    // First set the input arguments for the function call:
    Ctl::FunctionArgPtr rArg = call->findInputArg("rIn");
    fillInputArg(rArg, "rIn", r, n);
    Ctl::FunctionArgPtr gArg = call->findInputArg("gIn");
    fillInputArg(gArg, "gIn", g, n);
    Ctl::FunctionArgPtr bArg = call->findInputArg("bIn");
    fillInputArg(bArg, "bIn", b, n);
    Ctl::FunctionArgPtr aArg = call->findInputArg("aIn");
    fillInputArg(aArg, "aIn", a, n);

    // Now we can call the CTL function for
    // pixels 0, through n-1
    call->callFunction(n);

    // Retrieve the results
    Ctl::FunctionArgPtr rOutArg = call->findOutputArg("rOut");
    retrieveOutputArg(rOutArg, "rOut", rOut, n);
    Ctl::FunctionArgPtr gOutArg = call->findOutputArg("gOut");
    retrieveOutputArg(gOutArg, "gOut", gOut, n);
    Ctl::FunctionArgPtr bOutArg = call->findOutputArg("bOut");
    retrieveOutputArg(bOutArg, "bOut", bOut, n);
    Ctl::FunctionArgPtr aOutArg = call->findOutputArg("aOut");
    retrieveOutputArg(aOutArg, "aOut", aOut, n);
}

template <class Type>
void callCtl(Ctl::Interpreter& interp, Ctl::FunctionCallPtr call, const std::size_t size, Type* rOut, Type* gOut, Type* bOut,
             Type* aOut, const Type* r, const Type* g, const Type* b, const Type* a)
{
    std::size_t n = size;
    while(n > 0)
    {
        const std::size_t m = std::min(n, interp.maxSamples());
        callCtlChunk(call, m, *rOut, *gOut, *bOut, *aOut, *r, *g, *b, *a);

        n -= m;
        rOut += m;
        gOut += m;
        bOut += m;
        aOut += m;
        r += m;
        g += m;
        b += m;
        a += m;
    }
}
}

template <class View>
CTLProcess<View>::CTLProcess(CTLPlugin& effect)
    : ImageGilFilterProcessor<View>(effect, eImageOrientationIndependant)
    , _plugin(effect)
{
    ctlPlugin = &_plugin;
}

template <class View>
void CTLProcess<View>::setup(const OFX::RenderArguments& args)
{
    ctlPlugin = &_plugin;
    ImageGilFilterProcessor<View>::setup(args);
    _params = _plugin.getProcessParams(args.renderScale);

    switch(_params._inputType)
    {
        case eParamChooseInputCode:
        {
            TUTTLE_LOG_TRACE("CTL -- Load code: " << _params._code);
            _interpreter.loadModule("", "", _params._code);
            break;
        }
        case eParamChooseInputFile:
        {
            _interpreter.setModulePaths(_params._paths);
            TUTTLE_LOG_TRACE("CTL -- Load module: " << _params._filename << " " << _params._module);
            _interpreter.loadFile(_params._filename, _params._module);
            break;
        }
    }
    Ctl::setMessageOutputFunction(ctlMessageOutput);
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template <class View>
void CTLProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace boost::gil;

    Ctl::FunctionCallPtr call = _interpreter.newFunctionCall("main");

    const OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};

    const std::size_t alignment = 2;
    rgba32f_planar_image_t srcWorkLine(procWindowSize.x, 1, alignment);
    rgba32f_planar_view_t srcWorkLineV = view(srcWorkLine);
    rgba32f_planar_image_t dstWorkLine(procWindowSize.x, 1, alignment);
    rgba32f_planar_view_t dstWorkLineV = view(dstWorkLine);

    for(int y = procWindowRoW.y1; y < procWindowRoW.y2; ++y)
    {
        View srcLineV = subimage_view(this->_srcView, procWindowRoW.x1, y, procWindowSize.x, 1);
        View dstLineV = subimage_view(this->_dstView, procWindowRoW.x1, y, procWindowSize.x, 1);

        copy_pixels(srcLineV, srcWorkLineV);

        float* rOut = reinterpret_cast<float*>(&dstWorkLineV(0, 0)[0]);
        float* gOut = reinterpret_cast<float*>(&dstWorkLineV(0, 0)[1]);
        float* bOut = reinterpret_cast<float*>(&dstWorkLineV(0, 0)[2]);
        float* aOut = reinterpret_cast<float*>(&dstWorkLineV(0, 0)[3]);
        const float* r = reinterpret_cast<float*>(&srcWorkLineV(0, 0)[0]);
        const float* g = reinterpret_cast<float*>(&srcWorkLineV(0, 0)[1]);
        const float* b = reinterpret_cast<float*>(&srcWorkLineV(0, 0)[2]);
        const float* a = reinterpret_cast<float*>(&srcWorkLineV(0, 0)[3]);

        callCtl<float>(_interpreter, call, procWindowSize.x, rOut, gOut, bOut, aOut, r, g, b, a);

        copy_pixels(dstWorkLineV, dstLineV);

        if(this->progressForward(procWindowSize.x))
            return;
    }
}
}
}
}
