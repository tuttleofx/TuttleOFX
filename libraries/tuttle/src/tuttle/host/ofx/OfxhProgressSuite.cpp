#include "OfxhProgressSuite.hpp"
#include "OfxhIObject.hpp"
#include "OfxhProgress.hpp"
#include "OfxhException.hpp"

namespace tuttle
{
namespace host
{
namespace ofx
{

namespace
{

OfxStatus ProgressStart(void* effectInstance, const char* label)
{
    try
    {
        OfxhIProgress* me = dynamic_cast<OfxhIProgress*>(reinterpret_cast<OfxhIObject*>(effectInstance));
        if(!me)
            return kOfxStatErrBadHandle;

        me->progressStart(label);

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

/**
 * finish progressing
 */
OfxStatus ProgressEnd(void* effectInstance)
{
    try
    {
        OfxhIProgress* me = dynamic_cast<OfxhIProgress*>(reinterpret_cast<OfxhIObject*>(effectInstance));
        if(!me)
            return kOfxStatErrBadHandle;

        me->progressEnd();

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

/**
 * update progressing
 */
OfxStatus ProgressUpdate(void* effectInstance, double progress)
{
    try
    {
        OfxhIProgress* me = dynamic_cast<OfxhIProgress*>(reinterpret_cast<OfxhIObject*>(effectInstance));
        if(!me)
            return kOfxStatErrBadHandle;

        const bool v = me->progressUpdate(progress);

        // if v abort the process
        return v ? kOfxStatReplyNo : kOfxStatOK;
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

/**
 * our progress suite
 */
struct OfxProgressSuiteV1 gProgressSuite = {ProgressStart, ProgressUpdate, ProgressEnd};
}

void* getProgressSuite(const int version)
{
    if(version == 1)
        return &gProgressSuite;
    return NULL;
}
}
}
}
