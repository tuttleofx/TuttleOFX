#include "OfxhImageEffectSuite.hpp"
#include "OfxhImageEffectNode.hpp"

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace imageEffect
{

namespace
{

OfxStatus getPropertySet(OfxImageEffectHandle h1, OfxPropertySetHandle* h2)
{
    OfxhImageEffectNodeBase* effectBase = reinterpret_cast<OfxhImageEffectNodeBase*>(h1);

    if(!effectBase || !effectBase->verifyMagic())
    {
        return kOfxStatErrBadHandle;
    }

    *h2 = effectBase->getProperties().getHandle();
    return kOfxStatOK;
}

OfxStatus getParamSet(OfxImageEffectHandle h1, OfxParamSetHandle* h2)
{
    imageEffect::OfxhImageEffectNodeBase* effectBase = reinterpret_cast<imageEffect::OfxhImageEffectNodeBase*>(h1);

    if(!effectBase || !effectBase->verifyMagic())
    {
        return kOfxStatErrBadHandle;
    }

    imageEffect::OfxhImageEffectNodeDescriptor* effectDescriptor =
        dynamic_cast<imageEffect::OfxhImageEffectNodeDescriptor*>(effectBase);

    if(effectDescriptor)
    {
        *h2 = effectDescriptor->getParamSetHandle();
        return kOfxStatOK;
    }

    imageEffect::OfxhImageEffectNode* effectInstance = dynamic_cast<imageEffect::OfxhImageEffectNode*>(effectBase);

    if(effectInstance)
    {
        *h2 = effectInstance->getParamSetHandle();
        return kOfxStatOK;
    }

    return kOfxStatErrBadHandle;
}

OfxStatus clipDefine(OfxImageEffectHandle h1, const char* name, OfxPropertySetHandle* h2)
{
    imageEffect::OfxhImageEffectNodeBase* effectBase = reinterpret_cast<imageEffect::OfxhImageEffectNodeBase*>(h1);

    if(!effectBase || !effectBase->verifyMagic())
    {
        return kOfxStatErrBadHandle;
    }

    imageEffect::OfxhImageEffectNodeDescriptor* effectDescriptor =
        dynamic_cast<imageEffect::OfxhImageEffectNodeDescriptor*>(effectBase);

    if(!effectDescriptor)
        return kOfxStatErrBadHandle;

    attribute::OfxhClipImageDescriptor* clip = effectDescriptor->defineClip(name);
    *h2 = clip->getPropHandle();
    return kOfxStatOK;
}

OfxStatus clipGetPropertySet(OfxImageClipHandle clip, OfxPropertySetHandle* propHandle)
{
    attribute::OfxhClipImage* clipInstance = reinterpret_cast<attribute::OfxhClipImage*>(clip);

    if(!clipInstance || !clipInstance->verifyMagic())
    {
        return kOfxStatErrBadHandle;
    }

    *propHandle = clipInstance->getPropHandle();
    return kOfxStatOK;
}

OfxStatus clipGetImage(OfxImageClipHandle h1, OfxTime time, OfxRectD* h2, OfxPropertySetHandle* h3)
{
    attribute::OfxhClipImage* clipInstance = reinterpret_cast<attribute::OfxhClipImage*>(h1);

    if(!clipInstance || !clipInstance->verifyMagic())
    {
        return kOfxStatErrBadHandle;
    }

    OfxhImage* image = clipInstance->getImage(time, h2);
    if(!image)
    {
        h3 = NULL;
        return kOfxStatFailed;
    }

    image->addReference(OfxhImage::eReferenceOwnerPlugin);
    *h3 = image->getPropHandle(); // a pointer to the base class cast into OfxPropertySetHandle

    return kOfxStatOK;
}

OfxStatus clipReleaseImage(OfxPropertySetHandle h1)
{
    property::OfxhSet* pset = reinterpret_cast<property::OfxhSet*>(h1);

    if(!pset || !pset->verifyMagic())
    {
        return kOfxStatErrBadHandle;
    }

    OfxhImage* image = dynamic_cast<OfxhImage*>(pset);
    if(!image)
        return kOfxStatErrBadHandle;

    // clip::image has a virtual destructor for derived classes
    image->releaseReference(OfxhImage::eReferenceOwnerPlugin);
    return kOfxStatOK;
}

OfxStatus clipGetHandle(OfxImageEffectHandle imageEffect, const char* name, OfxImageClipHandle* clip,
                        OfxPropertySetHandle* propertySet)
{
    imageEffect::OfxhImageEffectNodeBase* effectBase = reinterpret_cast<imageEffect::OfxhImageEffectNodeBase*>(imageEffect);

    if(!effectBase || !effectBase->verifyMagic())
        return kOfxStatErrBadHandle;

    imageEffect::OfxhImageEffectNode* effectInstance = dynamic_cast<imageEffect::OfxhImageEffectNode*>(effectBase);

    if(!effectInstance)
        return kOfxStatErrBadHandle;

    attribute::OfxhClipImage& instance = effectInstance->getClip(name);
    *clip = instance.getOfxImageClipHandle();
    if(propertySet)
        *propertySet = instance.getPropHandle();

    return kOfxStatOK;
}

OfxStatus clipGetRegionOfDefinition(OfxImageClipHandle clip, OfxTime time, OfxRectD* bounds)
{
    attribute::OfxhClipImage* clipInstance = reinterpret_cast<attribute::OfxhClipImage*>(clip);

    if(!clipInstance || !clipInstance->verifyMagic())
        return kOfxStatErrBadHandle;

    *bounds = clipInstance->fetchRegionOfDefinition(time);
    return kOfxStatOK;
}

// should processing be aborted?

int abort(OfxImageEffectHandle imageEffect)
{
    imageEffect::OfxhImageEffectNodeBase* effectBase = reinterpret_cast<imageEffect::OfxhImageEffectNodeBase*>(imageEffect);

    if(!effectBase || !effectBase->verifyMagic())
        return kOfxStatErrBadHandle;

    imageEffect::OfxhImageEffectNode* effectInstance = dynamic_cast<imageEffect::OfxhImageEffectNode*>(effectBase);

    if(!effectInstance)
        return kOfxStatErrBadHandle;

    return effectInstance->abort();
}

OfxStatus imageMemoryAlloc(OfxImageEffectHandle instanceHandle, size_t nBytes, OfxImageMemoryHandle* memoryHandle)
{
    imageEffect::OfxhImageEffectNodeBase* effectBase =
        reinterpret_cast<imageEffect::OfxhImageEffectNodeBase*>(instanceHandle);
    if(!effectBase || !effectBase->verifyMagic())
        return kOfxStatErrBadHandle;

    imageEffect::OfxhImageEffectNode* effectInstance = dynamic_cast<imageEffect::OfxhImageEffectNode*>(effectBase);
    OfxhMemory* memory;

    if(effectInstance)
    {
        if(!effectInstance->verifyMagic())
            return kOfxStatErrBadHandle;

        memory = effectInstance->imageMemoryAlloc(nBytes);
    }
    else
    {
        memory = new OfxhMemory;
        memory->alloc(nBytes);
    }

    *memoryHandle = memory->getHandle();
    return kOfxStatOK;
}

OfxStatus imageMemoryFree(OfxImageMemoryHandle memoryHandle)
{
    OfxhMemory* memoryInstance = reinterpret_cast<OfxhMemory*>(memoryHandle);

    if(!memoryInstance || !memoryInstance->verifyMagic())
        return kOfxStatErrBadHandle;

    memoryInstance->freeMem();
    delete memoryInstance;
    return kOfxStatOK;
}

OfxStatus imageMemoryLock(OfxImageMemoryHandle memoryHandle, void** returnedPtr)
{
    OfxhMemory* memoryInstance = reinterpret_cast<OfxhMemory*>(memoryHandle);

    if(!memoryInstance || !memoryInstance->verifyMagic())
        return kOfxStatErrBadHandle;

    memoryInstance->lock();
    *returnedPtr = memoryInstance->getPtr();
    return kOfxStatOK;
}

OfxStatus imageMemoryUnlock(OfxImageMemoryHandle memoryHandle)
{
    OfxhMemory* memoryInstance = reinterpret_cast<OfxhMemory*>(memoryHandle);

    if(!memoryInstance || !memoryInstance->verifyMagic())
        return kOfxStatErrBadHandle;

    memoryInstance->unlock();
    return kOfxStatOK;
}

struct OfxImageEffectSuiteV1 gImageEffectSuite = {getPropertySet,
                                                  getParamSet,
                                                  clipDefine,
                                                  clipGetHandle,
                                                  clipGetPropertySet,
                                                  clipGetImage,
                                                  clipReleaseImage,
                                                  clipGetRegionOfDefinition,
                                                  abort,
                                                  imageMemoryAlloc,
                                                  imageMemoryFree,
                                                  imageMemoryLock,
                                                  imageMemoryUnlock};
}

void* getImageEffectSuite(const int version)
{
    if(version == 1)
        return &gImageEffectSuite;
    return NULL;
}
}
}
}
}
