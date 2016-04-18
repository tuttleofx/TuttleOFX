/*
 * Software License :
 *
 * Copyright (c) 2007-2009, The Open Effects Association Ltd. All rights reserved.
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

// ofx host
#include "OfxhBinary.hpp"
#include "OfxhImageEffectNode.hpp"
#include "property/OfxhSet.hpp"
#include "attribute/OfxhClip.hpp"

// ofx
#include <ofxCore.h>

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace imageEffect
{

std::ptrdiff_t OfxhImage::_count = 0;

static property::OfxhPropSpec imageStuffs[] = {
    {kOfxPropType, property::ePropTypeString, 1, false, kOfxTypeImage},
    {kOfxImageEffectPropPixelDepth, property::ePropTypeString, 1, true, kOfxBitDepthNone},
    {kOfxImageEffectPropComponents, property::ePropTypeString, 1, true, kOfxImageComponentNone},
    {kOfxImageEffectPropPreMultiplication, property::ePropTypeString, 1, true, kOfxImageOpaque},
    {kOfxImageEffectPropRenderScale, property::ePropTypeDouble, 2, true, "1.0"},
    {kOfxImagePropPixelAspectRatio, property::ePropTypeDouble, 1, true, "1.0"},
    {kOfxImagePropData, property::ePropTypePointer, 1, true, NULL},
    {kOfxImagePropBounds, property::ePropTypeInt, 4, true, "0"},
    {
        kOfxImagePropRegionOfDefinition, property::ePropTypeInt, 4, true, "0",
    },
    {
        kOfxImagePropRowBytes, property::ePropTypeInt, 1, true, "0",
    },
    {
        kOfxImagePropField, property::ePropTypeString, 1, true, "",
    },
    {kOfxImagePropUniqueIdentifier, property::ePropTypeString, 1, true, ""},
    {0}};

OfxhImage::OfxhImage()
    : property::OfxhSet(imageStuffs)
    , _id(_count++)
    , _clipName("No clip !")
    , _time(0)
{
    TUTTLE_LOG_TRACE("[Ofxh Image] create clip:"
                     << getClipName() << ", time:" << getTime() << ", id:" << getId()
                     << ", ref host:" << getReferenceCount(ofx::imageEffect::OfxhImage::eReferenceOwnerHost)
                     << ", ref plugins:" << getReferenceCount(ofx::imageEffect::OfxhImage::eReferenceOwnerPlugin));
}

/**
 * make an image from a clip instance
 */
OfxhImage::OfxhImage(attribute::OfxhClip& instance, const OfxTime time)
    : property::OfxhSet(imageStuffs)
    , _id(_count++)
    , _clipName(instance.getFullName())
    , _time(time)
{
    TUTTLE_LOG_TRACE("[Ofxh Image] create clip:"
                     << getClipName() << ", time:" << getTime() << ", id:" << getId()
                     << ", ref host:" << getReferenceCount(ofx::imageEffect::OfxhImage::eReferenceOwnerHost)
                     << ", ref plugins:" << getReferenceCount(ofx::imageEffect::OfxhImage::eReferenceOwnerPlugin));
    initClipBits(instance);
}

/**
 * construction based on clip instance
 *
OfxhImage::OfxhImage( attribute::OfxhClip& instance,
                                          OfxTime              time,
                      double               renderScaleX,
                      double               renderScaleY,
                      void*                data,
                      const OfxRectI&      bounds,
                      const OfxRectI&      rod,
                      int                  rowBytes,
                      const std::string&   field,
                      const std::string&   uniqueIdentifier )
        : property::OfxhSet( imageStuffs )
        , _id( _count++ )
        , _clipName( instance.getName() )
        , _time( time )
{
        TUTTLE_LOG_INFO( "++ OfxhImage, clipName:" << getClipName()
                << ", time:" << getTime()
                << ", id:" << getId()
                << ", ref host:" << getReferenceCount( ofx::imageEffect::OfxhImage::eReferenceOwnerHost )
                << ", ref plugins:" << getReferenceCount( ofx::imageEffect::OfxhImage::eReferenceOwnerPlugin ) );
        initClipBits( instance );

        // set other data
        setDoubleProperty( kOfxImageEffectPropRenderScale, renderScaleX, 0 );
        setDoubleProperty( kOfxImageEffectPropRenderScale, renderScaleY, 1 );
        setPointerProperty( kOfxImagePropData, data );
        setIntProperty( kOfxImagePropBounds, bounds.x1, 0 );
        setIntProperty( kOfxImagePropBounds, bounds.y1, 1 );
        setIntProperty( kOfxImagePropBounds, bounds.x2, 2 );
        setIntProperty( kOfxImagePropBounds, bounds.y2, 3 );
        setIntProperty( kOfxImagePropRegionOfDefinition, rod.x1, 0 );
        setIntProperty( kOfxImagePropRegionOfDefinition, rod.y1, 1 );
        setIntProperty( kOfxImagePropRegionOfDefinition, rod.x2, 2 );
        setIntProperty( kOfxImagePropRegionOfDefinition, rod.y2, 3 );
        setIntProperty( kOfxImagePropRowBytes, rowBytes );

        setStringProperty( kOfxImagePropField, field );
        setStringProperty( kOfxImageClipPropFieldOrder, field );
        setStringProperty( kOfxImagePropUniqueIdentifier, uniqueIdentifier );
}
*/

OfxhImage::~OfxhImage()
{
    TUTTLE_LOG_TRACE("[Ofxh Image] delete clip:" << getClipName() << ", time:" << getTime() << ", id:" << getId()
                                                 << ", ref host:" << getReferenceCount(eReferenceOwnerHost)
                                                 << ", ref plugins:" << getReferenceCount(eReferenceOwnerPlugin));
    // Ref count host/plugin, should be 0, but when an error occured
    // during the graph computation the ref count can be > 0.
    if(getReferenceCount(eReferenceOwnerHost))
    {
        TUTTLE_LOG_INFO("[Ofxh Image] The HOST has not properly released the image ref count ("
                        << getReferenceCount(eReferenceOwnerHost) << ") of clip " << quotes(getClipName()) << " at time "
                        << getTime() << ".");
    }
    if(getReferenceCount(eReferenceOwnerPlugin))
    {
        TUTTLE_LOG_INFO("[Ofxh Image] The PLUGIN has not properly released the image ref count ("
                        << getReferenceCount(eReferenceOwnerPlugin) << ") of clip " << quotes(getClipName()) << " at time "
                        << getTime() << ".");
    }
}

/**
 * called during ctor to get bits from the clip props into ours
 */
void OfxhImage::initClipBits(attribute::OfxhClip& instance)
{
    const property::OfxhSet& clipProperties = instance.getProperties();

    // get and set the clip instance pixel depth
    const std::string& depth = clipProperties.getStringProperty(kOfxImageEffectPropPixelDepth);

    setStringProperty(kOfxImageEffectPropPixelDepth, depth);

    // get and set the clip instance components
    const std::string& comps = clipProperties.getStringProperty(kOfxImageEffectPropComponents);
    setStringProperty(kOfxImageEffectPropComponents, comps);

    // get and set the clip instance premultiplication
    setStringProperty(kOfxImageEffectPropPreMultiplication,
                      clipProperties.getStringProperty(kOfxImageEffectPropPreMultiplication));

    // get and set the clip instance pixel aspect ratio
    setDoubleProperty(kOfxImagePropPixelAspectRatio, clipProperties.getDoubleProperty(kOfxImagePropPixelAspectRatio));
}

OfxRectI OfxhImage::getBounds() const
{
    OfxRectI bounds = {0, 0, 0, 0};
    getIntPropertyN(kOfxImagePropBounds, &bounds.x1, 4);
    return bounds;
}

OfxRectI OfxhImage::getROD() const
{
    OfxRectI rod = {0, 0, 0, 0};
    getIntPropertyN(kOfxImagePropRegionOfDefinition, &rod.x1, 4);
    return rod;
}

EBitDepth OfxhImage::getBitDepth() const
{
    std::string depth = getStringProperty(kOfxImageEffectPropPixelDepth);
    EBitDepth bitDepth = eBitDepthNone;

    if(depth == kOfxBitDepthByte)
    {
        bitDepth = eBitDepthUByte;
    }
    else if(depth == kOfxBitDepthShort)
    {
        bitDepth = eBitDepthUShort;
    }
    else if(depth == kOfxBitDepthFloat)
    {
        bitDepth = eBitDepthFloat;
    }
    return bitDepth;
}

EPixelComponent OfxhImage::getComponentsType() const
{
    std::string sType = getStringProperty(kOfxImageEffectPropComponents);
    EPixelComponent compType = ePixelComponentNone;

    if(sType == kOfxImageComponentRGBA)
    {
        compType = ePixelComponentRGBA;
    }
    else if(sType == kOfxImageComponentRGB)
    {
        compType = ePixelComponentRGB;
    }
    else if(sType == kOfxImageComponentAlpha)
    {
        compType = ePixelComponentAlpha;
    }
    return compType;
}

int OfxhImage::getRowBytes() const
{
    return getIntProperty(kOfxImagePropRowBytes);
}

int OfxhImage::getReferenceCount(const EReferenceOwner from) const
{
    RefMap::const_iterator it = _referenceCount.find(from);
    if(it == _referenceCount.end())
        return 0;
    return it->second;
}

void OfxhImage::addReference(const EReferenceOwner from, const std::size_t n)
{
    const std::ptrdiff_t refC = _referenceCount[from] = getReferenceCount(from) + n;
    TUTTLE_LOG_INFO("[Ofxh Image] add reference with degree " << n << ", clipName:" << getClipName() << ", time:"
                                                              << getTime() << ", id:" << getId() << ", ref:" << refC);
}

bool OfxhImage::releaseReference(const EReferenceOwner from)
{
    const std::ptrdiff_t refC = _referenceCount[from] = getReferenceCount(from) - 1;
    TUTTLE_LOG_INFO("[Ofxh Image] release reference, clipName:" << getClipName() << ", time:" << getTime()
                                                                << ", id:" << getId() << ", ref:" << refC);
    if(refC < 0)
        BOOST_THROW_EXCEPTION(std::logic_error("Try to release an undeclared reference to an Image."));
    return refC <= 0;
}
}
}
}
}
