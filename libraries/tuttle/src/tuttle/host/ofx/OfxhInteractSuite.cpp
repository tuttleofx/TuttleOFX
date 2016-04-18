#include "OfxhInteractSuite.hpp"
#include "OfxhInteract.hpp"

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace interact
{

namespace
{

OfxStatus interactSwapBuffers(OfxInteractHandle handle)
{
    try
    {
        interact::OfxhInteract* interactInstance = reinterpret_cast<interact::OfxhInteract*>(handle);
        if(!interactInstance)
            return kOfxStatErrBadHandle;

        interactInstance->swapBuffers();

        return kOfxStatOK;
    }
    catch(OfxhException& e)
    {
        return e.getStatus();
    }
    catch(...)
    {
        return kOfxStatErrUnknown;
    }
}

OfxStatus interactRedraw(OfxInteractHandle handle)
{
    try
    {
        interact::OfxhInteract* interactInstance = reinterpret_cast<interact::OfxhInteract*>(handle);
        if(!interactInstance)
            return kOfxStatErrBadHandle;

        interactInstance->redraw();

        return kOfxStatOK;
    }
    catch(OfxhException& e)
    {
        return e.getStatus();
    }
    catch(...)
    {
        return kOfxStatErrUnknown;
    }
}

OfxStatus interactGetPropertySet(OfxInteractHandle handle, OfxPropertySetHandle* property)
{
    try
    {
        interact::OfxhInteractBase* interact = reinterpret_cast<interact::OfxhInteractBase*>(handle);
        if(!interact)
            return kOfxStatErrBadHandle;
        if(!interact->verifyMagic())
            return kOfxStatErrBadHandle;

        *property = interact->getPropHandle();

        return kOfxStatOK;
    }
    catch(OfxhException& e)
    {
        return e.getStatus();
    }
    catch(...)
    {
        return kOfxStatErrUnknown;
    }
}

/// the interact suite
OfxInteractSuiteV1 gSuite = {interactSwapBuffers, interactRedraw, interactGetPropertySet};
}

/// function to get the interact suite
void* getInteractSuite(const int version)
{
    if(version == 1)
        return static_cast<void*>(&gSuite);
    return NULL;
}
}
}
}
}
