/*
 * Software License :
 *
 * Copyright (c) 2007, The Open Effects Association Ltd. All rights reserved.
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
#include <iostream>
#include <fstream>
#include <math.h>
#include <time.h>
#include <cstring>

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
#include <tuttle/host/ofx/ofxhImageEffect.h>
#include <tuttle/host/ofx/ofxhImageEffectAPI.h>

// my host
#include "HostDescriptor.hpp"
#include "EffectInstance.hpp"
#include "ClipInstance.hpp"

// boost
#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/cstdint.hpp>

// utilities
#include "tuttle/common/utils/global.hpp"
#include "tuttle/common/image/gilGlobals.hpp"

/// @todo TUTTLE_TODO : clean using namespace...
using namespace tuttle::host;
using namespace tuttle::host::ofx::imageEffect;
using namespace boost::gil;
using namespace boost;

namespace tuttle {
namespace host {
namespace core {
Image::Image( ClipImgInstance& clip, const OfxRectD& bounds, OfxTime time )
	: tuttle::host::ofx::imageEffect::Image( clip ),
	/// this ctor will set basic props on the image
	_data( NULL )
{
	size_t memlen = 0;
	size_t rowlen = 0;

	_ncomp = 0;
	// Set rod in canonical & pixel coord.
	OfxRectI ibounds;
	double par = clip.getPixelAspectRatio();
	ibounds.x1 = int(bounds.x1 / par);
	ibounds.x2 = int(bounds.x2 / par);
	ibounds.y1 = int(bounds.y1);
	ibounds.y2 = int(bounds.y2);

	OfxPointI dimensions = { ibounds.x2 - ibounds.x1, ibounds.y2 - ibounds.y1 };

	if( clip.getComponents() == kOfxImageComponentRGBA )
	{
		_ncomp = 4;
	}
	else if( clip.getComponents() == kOfxImageComponentAlpha )
	{
		_ncomp = 1;
	}

	// make some memory according to the bit depth
	if( clip.getPixelDepth() == kOfxBitDepthByte )
	{
		memlen = _ncomp * dimensions.x * dimensions.y;
		rowlen = _ncomp * dimensions.x;
	}
	else if( clip.getPixelDepth() == kOfxBitDepthShort )
	{
		memlen = _ncomp * dimensions.x * dimensions.y * sizeof( uint16_t );
		rowlen = _ncomp * dimensions.x * sizeof( uint16_t );
	}
	else if( clip.getPixelDepth() == kOfxBitDepthFloat )
	{
		memlen = int(_ncomp * dimensions.x * dimensions.y * sizeof( float ) );
		rowlen = int(_ncomp * dimensions.x * sizeof( float ) );
	}

	_data = new uint8_t[memlen];
	// now blank it
	memset( _data, 0, memlen );

	// render scale x and y of 1.0
	setDoubleProperty( kOfxImageEffectPropRenderScale, 1.0, 0 );
	setDoubleProperty( kOfxImageEffectPropRenderScale, 1.0, 1 );

	// data ptr
	setPointerProperty( kOfxImagePropData, _data );

	// bounds and rod
	setIntProperty( kOfxImagePropBounds, ibounds.x1, 0 );
	setIntProperty( kOfxImagePropBounds, ibounds.y1, 1 );
	setIntProperty( kOfxImagePropBounds, ibounds.x2, 2 );
	setIntProperty( kOfxImagePropBounds, ibounds.y2, 3 );

	/// @todo the same for bounds and rod, no tiles for the moment !
	setIntProperty( kOfxImagePropRegionOfDefinition, ibounds.x1, 0 );
	setIntProperty( kOfxImagePropRegionOfDefinition, ibounds.y1, 1 );
	setIntProperty( kOfxImagePropRegionOfDefinition, ibounds.x2, 2 );
	setIntProperty( kOfxImagePropRegionOfDefinition, ibounds.y2, 3 );

	// row bytes
	setIntProperty( kOfxImagePropRowBytes, rowlen );
}

Image::~Image()
{
	delete [] _data;
}

uint8_t* Image::pixel( int x, int y ) const
{
	OfxRectI bounds = getBounds();

	if( ( x >= bounds.x1 ) && ( x < bounds.x2 ) && ( y >= bounds.y1 ) && ( y < bounds.y2 ) )
	{
		int rowBytes = getIntProperty( kOfxImagePropRowBytes );
		int offset   = ( y = bounds.y1 ) * rowBytes + ( x - bounds.x1 ) * _ncomp;
		return &( _data[offset] );
	}
	return 0;
}

template < class VIEW_T >
VIEW_T Image::gilViewFromImage( Image* img )
{
	OfxRectI bounds = img->getBounds();

	typedef typename VIEW_T::value_type value_t;
	return interleaved_view( std::abs( bounds.x2 - bounds.x1 ),
	                         std::abs( bounds.y2 - bounds.y1 ),
	                         (value_t*)( img->getPixelData() ),
	                         img->getRowBytes() );
}

// @todo: put this in gilGlobals.hpp
template < class D_VIEW, class S_VIEW >
void Image::copy( D_VIEW& dst, S_VIEW& src, const OfxPointI& dstCorner,
                  const OfxPointI& srcCorner, const OfxPointI& count )
{
	if( src.width() >= ( count.x - srcCorner.x ) &&
	    src.height() >= ( count.y - srcCorner.y ) &&
	    dst.width() >= ( count.x - dstCorner.x ) &&
	    dst.height() >= ( count.y - dstCorner.y ) )
	{
		S_VIEW subSrc = subimage_view( src, srcCorner.x, srcCorner.y, count.x, count.y );
		D_VIEW subDst = subimage_view( dst, dstCorner.x, dstCorner.y, count.x, count.y );
		copy_and_convert_pixels( subSrc, subDst );
	}
}

/// Copy from gil image view to Image
template < class S_VIEW >
void Image::copy( Image* dst, S_VIEW& src, const OfxPointI& dstCorner,
                  const OfxPointI& srcCorner, const OfxPointI& count )
{
	// Create destination
	switch( dst->getComponentsType() )
	{
		case ofx::imageEffect::ePixelComponentRGBA:
			switch( dst->getBitDepth() )
			{
				case ofx::imageEffect::eBitDepthUByte:
				{
					rgba8_view_t dView = gilViewFromImage<rgba8_view_t >( dst );
					Image::copy( dView, src, dstCorner, srcCorner, count );
					break;
				}
				case ofx::imageEffect::eBitDepthUShort:
				{
					rgba16_view_t dView = gilViewFromImage<rgba16_view_t >( dst );
					Image::copy( dView, src, dstCorner, srcCorner, count );
					break;
				}
				case ofx::imageEffect::eBitDepthFloat:
				{
					rgba32f_view_t dView = gilViewFromImage<rgba32f_view_t >( dst );
					Image::copy( dView, src, dstCorner, srcCorner, count );
					break;
				}
				default:
					break;
			}
			break;
		case ofx::imageEffect::ePixelComponentAlpha:
			switch( dst->getBitDepth() )
			{
				case ofx::imageEffect::eBitDepthUByte:
				{
					gray8_view_t dView = gilViewFromImage<gray8_view_t >( dst );
					Image::copy( dView, src, dstCorner, srcCorner, count );
					break;
				}
				case ofx::imageEffect::eBitDepthUShort:
				{
					gray16_view_t dView = gilViewFromImage<gray16_view_t >( dst );
					Image::copy( dView, src, dstCorner, srcCorner, count );
					break;
				}
				case ofx::imageEffect::eBitDepthFloat:
				{
					gray32f_view_t dView = gilViewFromImage<gray32f_view_t >( dst );
					Image::copy( dView, src, dstCorner, srcCorner, count );
					break;
				}
				default:
					break;
			}
			break;
		default:
			break;
	}
}

/// Copy from Image to Image
void Image::copy( Image* dst, Image* src, const OfxPointI& dstCorner,
                  const OfxPointI& srcCorner, const OfxPointI& count )
{
	switch( src->getComponentsType() )
	{
		case ofx::imageEffect::ePixelComponentRGBA:
			switch( src->getBitDepth() )
			{
				case ofx::imageEffect::eBitDepthUByte:
				{
					rgba8_view_t sView = gilViewFromImage<rgba8_view_t >( src );
					Image::copy( dst, sView, dstCorner, srcCorner, count );
					break;
				}
				case ofx::imageEffect::eBitDepthUShort:
				{
					rgba16_view_t sView = gilViewFromImage<rgba16_view_t >( src );
					Image::copy( dst, sView, dstCorner, srcCorner, count );
					break;
				}
				case ofx::imageEffect::eBitDepthFloat:
				{
					rgba32f_view_t sView = gilViewFromImage<rgba32f_view_t >( src );
					Image::copy( dst, sView, dstCorner, srcCorner, count );
					break;
				}
				default:
					break;
			}
			break;
		case ofx::imageEffect::ePixelComponentAlpha:
			switch( src->getBitDepth() )
			{
				case ofx::imageEffect::eBitDepthUByte:
				{
					gray8_view_t sView = gilViewFromImage<gray8_view_t >( src );
					Image::copy( dst, sView, dstCorner, srcCorner, count );
					break;
				}
				case ofx::imageEffect::eBitDepthUShort:
				{
					gray16_view_t sView = gilViewFromImage<gray16_view_t >( src );
					Image::copy( dst, sView, dstCorner, srcCorner, count );
					break;
				}
				case ofx::imageEffect::eBitDepthFloat:
				{
					gray32f_view_t sView = gilViewFromImage<gray32f_view_t >( src );
					Image::copy( dst, sView, dstCorner, srcCorner, count );
					break;
				}
				default:
					break;
			}
			break;
		default:
			break;
	}
}

ClipImgInstance::ClipImgInstance( EffectInstance& effect, const tuttle::host::ofx::attribute::ClipImageDescriptor& desc )
	: tuttle::host::ofx::attribute::ClipImageInstance( effect, desc ),
	_effect( effect ),
	_inputImage( NULL ),
	_outputImage( NULL )
{
	_frameRange = _effect.getEffectFrameRange();
}

ClipImgInstance::~ClipImgInstance()
{
	if( _inputImage )
		if( _inputImage->releaseReference() )
			delete _inputImage;
	if( _outputImage )
		if( _outputImage->releaseReference() )
			delete _outputImage;
}

/// Return the rod on the clip cannoical coords!
OfxRectD ClipImgInstance::getRegionOfDefinition( OfxTime time ) const
{
	OfxRectD rod;
	OfxPointD renderScale;

	// Rule: default is project size
	_effect.getProjectOffset( rod.x1, rod.y1 );
	_effect.getProjectSize( rod.x2, rod.y2 );
	_effect.getRenderScaleRecursive( renderScale.x, renderScale.y );

	/* @OFX_TODO: Tres etrange: ca bug avec les plugins du commerce: debordement de pile.
	 * Property::PropSpec inStuff[] = {
	 *      { kOfxPropTime, Property::eDouble, 1, true, "0" },
	 *      { kOfxImageEffectPropRenderScale, Property::eDouble, 2, true, "0" },
	 *      { 0 }
	 * };
	 *
	 * Property::PropSpec outStuff[] = {
	 *      { kOfxImageEffectPropRegionOfDefinition, Property::eDouble, 4, false, "0" },
	 *      { 0 }
	 * };
	 *
	 * Property::Set inArgs(inStuff);
	 * Property::Set outArgs(outStuff);
	 *
	 * inArgs.setDoubleProperty(kOfxPropTime,time);
	 *
	 * inArgs.setDoublePropertyN(kOfxImageEffectPropRenderScale, &renderScale.x, 2);
	 *
	 * OfxStatus stat = _effect->mainEntry(kOfxImageEffectActionGetRegionOfDefinition,
	 *                                                                      _effect->getHandle(), &inArgs, &outArgs);
	 *
	 * if(stat == kOfxStatOK)
	 *      outArgs.getDoublePropertyN(kOfxImageEffectPropRegionOfDefinition, &rod.x1, 4);
	 */
	return rod;
}

/// Get the Raw Unmapped Pixel Depth from the host.

const std::string& ClipImgInstance::getUnmappedBitDepth() const
{
	static const std::string v( _effect.getProjectBitDepth() );

	return v;
}

/// Get the Raw Unmapped Components from the host.

const std::string& ClipImgInstance::getUnmappedComponents() const
{
	static const std::string v( _effect.getProjectPixelComponentsType() );

	return v;
}

// PreMultiplication -
//
//  kOfxImageOpaque - the image is opaque and so has no premultiplication state
//  kOfxImagePreMultiplied - the image is premultiplied by it's alpha
//  kOfxImageUnPreMultiplied - the image is unpremultiplied

const std::string& ClipImgInstance::getPremult() const
{
	return _effect.getOutputPreMultiplication();
}

// Frame Rate -

double ClipImgInstance::getFrameRate() const
{
	/// our clip is pretending to be progressive PAL SD by default
	double val = _effect.getFrameRate();

	return val;
}

// Frame Range (startFrame, endFrame) -
//
//  The frame range over which a clip has images.

void ClipImgInstance::getFrameRange( double& startFrame, double& endFrame ) const
{
	startFrame = 0.0;
	endFrame   = 1.0;
}

/// Field Order - Which spatial field occurs temporally first in a frame.
/// \returns
///  - kOfxImageFieldNone - the clip material is unfielded
///  - kOfxImageFieldLower - the clip material is fielded, with image rows 0,2,4.... occuring first in a frame
///  - kOfxImageFieldUpper - the clip material is fielded, with image rows line 1,3,5.... occuring first in a frame

const std::string& ClipImgInstance::getFieldOrder() const
{
	/// our clip is pretending to be progressive PAL SD, so return kOfxImageFieldNone
	static const std::string v( kOfxImageFieldNone );

	return v;
}

// Connected -
//
//  Says whether the clip is actually connected at the moment.

bool ClipImgInstance::getConnected() const
{
	return true;
}

// Unmapped Frame Rate -
//
//  The unmaped frame range over which an output clip has images.

double ClipImgInstance::getUnmappedFrameRate() const
{
	return _effect.getFrameRate();
}

// Unmapped Frame Range -
//
//  The unmaped frame range over which an output clip has images.
// this is applicable only to hosts and plugins that allow a plugin to change frame rates

void ClipImgInstance::getUnmappedFrameRange( double& unmappedStartFrame, double& unmappedEndFrame ) const
{
	unmappedStartFrame = 0;
	unmappedEndFrame   = 1;
}

// Continuous Samples -
//
//  0 if the images can only be sampled at discreet times (eg: the clip is a sequence of frames),
//  1 if the images can only be sampled continuously (eg: the clip is infact an animating roto spline and can be rendered anywhen).

bool ClipImgInstance::getContinuousSamples() const
{
	return false;
}

/// override this to fill in the image at the given time.
/// The bounds of the image on the image plane should be
/// 'appropriate', typically the value returned in getRegionsOfInterest
/// on the effect instance. Outside a render call, the optionalBounds should
/// be 'appropriate' for the.
/// If bounds is not null, fetch the indicated section of the canonical image plane.

tuttle::host::ofx::imageEffect::Image* ClipImgInstance::getImage( OfxTime time, OfxRectD* optionalBounds )
{
	OfxRectD bounds;

	if( optionalBounds )
	{
		bounds.x1 = optionalBounds->x1;
		bounds.y1 = optionalBounds->y1;
		bounds.x2 = optionalBounds->x2;
		bounds.y2 = optionalBounds->y2;
	}
	else
		bounds = getRegionOfDefinition( time );

	if( isOutput() )
	{
		if( !_outputImage )
		{
			// make a new ref counted image
			_outputImage = new Image( *this, bounds, time );
		}

		// add another reference to the member image for this fetch
		// as we have a ref count of 1 due to construction, this will
		// cause the output image never to delete by the plugin
		// when it releases the image
		_outputImage->addReference();

		return _outputImage;
	}
	else
	{
		if( !_inputImage )
		{
			// make a new ref counted image
			_inputImage = new Image( *this, bounds, time );
		}

		// add another reference to the member image for this fetch
		// as we have a ref count of 1 due to construction, this will
		// cause the output image never to delete by the plugin
		// when it releases the image
		_inputImage->addReference();

		return _inputImage;
	}
}

}
}
}

