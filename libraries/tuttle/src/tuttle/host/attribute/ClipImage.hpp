#ifndef _TUTTLE_HOST_CORE_CLIPIMAGE_HPP_
#define _TUTTLE_HOST_CORE_CLIPIMAGE_HPP_

#include "Attribute.hpp"

#include <tuttle/host/attribute/Image.hpp>
#include <tuttle/host/INode.hpp>
#include <tuttle/host/memory/IMemoryCache.hpp>
#include <tuttle/host/ofx/attribute/OfxhClipImage.hpp>

#include <boost/cstdint.hpp>

#define SOFXCLIPLENGTH 1

namespace tuttle {
namespace host {
namespace attribute {

/**
 *
 *
 */
class ClipImage : public Attribute
	, public ofx::attribute::OfxhClipImage
{
friend class INode;

protected:
	std::string _name;
	bool _isConnected;
	bool _continuousSamples;
	memory::IMemoryCache& _memoryCache;

	const ClipImage* _connectedClip; ///< @warning HACK ! to force connection (only for test) @todo remove this !!!!

public:
	ClipImage( INode& effect, const ofx::attribute::OfxhClipImageDescriptor& desc );

	~ClipImage();

	ClipImage* clone() const { return new ClipImage( *this ); }

	const std::string& getName() const { return ofx::attribute::OfxhAttributeAccessor::getName(); }

	/// @warning HACK ! to force connection (only for test)
	/// @todo remove this !!!!
	void setConnectedClip( const ClipImage& other )
	{
		if( isOutput() )
		{
			BOOST_THROW_EXCEPTION( exception::Logic()
			    << exception::user( "You can't connect an output Clip !" ) );
		}
		if( !other.isOutput() )
		{
			BOOST_THROW_EXCEPTION( exception::Logic()
			    << exception::user( "You can't connect to an input Clip !" ) );
		}
		_connectedClip = &other;
		setConnected();

		getEditableProperties().setStringProperty( "TuttleFullName", getFullName() );
		getEditableProperties().setStringProperty( "TuttleIdentifier", getIdentifier() );
	}

	void setUnconnected() { _connectedClip = NULL; setConnected( false ); }

	std::string getFullName() const;

	std::string getConnectedClipFullName() const
	{
		if( isOutput() || !isConnected() || _connectedClip->getFullName().size() == 0 )
		{
			BOOST_THROW_EXCEPTION( exception::Logic()
			    << exception::user( "Input clip " + getFullName() + " is not connected !" ) );
		}
		return _connectedClip->getFullName();
	}

	std::string getIdentifier() const
	{
		if( isOutput() )
			return getFullName();
		else
			return getConnectedClipFullName();
	}

	/// @todo tuttle: this is really bad...
	ClipImage& getConnectedClip()
	{
		return const_cast<ClipImage&>( *_connectedClip );
	}

	const ClipImage& getConnectedClip() const
	{
		return *_connectedClip;
	}

	/**
	 * @brief Get the Raw Unmapped Pixel Depth from the host
	 *  @returns
	 *     - kOfxBitDepthNone (implying a clip is unconnected image)
	 *     - kOfxBitDepthByte
	 *     - kOfxBitDepthShort
	 *     - kOfxBitDepthFloat
	 */
	const std::string& getUnmappedBitDepth() const;

	/**
	 * @brief Get the Raw Unmapped Components from the host
	 * @returns
	 *      - kOfxImageComponentNone (implying a clip is unconnected, not valid for an image)
	 *      - kOfxImageComponentRGBA
	 *      - kOfxImageComponentRGB
	 *      - kOfxImageComponentAlpha
	 */
	const std::string& getUnmappedComponents() const;

	#if 0
	/**
	 * @brief PreMultiplication
	 *      - kOfxImageOpaque - the image is opaque and so has no premultiplication state
	 *      - kOfxImagePreMultiplied - the image is premultiplied by it's alpha
	 *      - kOfxImageUnPreMultiplied - the image is unpremultiplied
	 */
	const std::string& getPremult() const { return getNode().getOutputPreMultiplication(); }

	/**
	 * @brief Field Order - Which spatial field occurs temporally first in a frame.
	 * @returns
	 *  - kOfxImageFieldNone - the clip material is unfielded
	 *  - kOfxImageFieldLower - the clip material is fielded, with image rows 0,2,4.... occuring first in a frame
	 *  - kOfxImageFieldUpper - the clip material is fielded, with image rows line 1,3,5.... occuring first in a frame
	 */
	const std::string& getFieldOrder() const
	{
		/// our clip is pretending to be progressive PAL SD, so return kOfxImageFieldNone
		static const std::string v( kOfxImageFieldNone );

		return v;
	}

	/**
	 * @brief Continuous Samples
	 *  0 if the images can only be sampled at discreet times (eg: the clip is a sequence of frames),
	 *  1 if the images can only be sampled continuously (eg: the clip is infact an animating roto spline and can be rendered anywhen).
	 */
	const bool getContinuousSamples() const                         { return _continuousSamples; }
	void       setContinuousSamples( const bool continuousSamples ) { _continuousSamples = continuousSamples; }

	#endif

	/**
	 * @brief Frame Rate
	 * The frame rate of a clip or instance's project.
	 */
	double getFrameRate() const;

	/**
	 * @brief Frame Range (startFrame, endFrame)
	 * The frame range over which a clip has images.
	 */
	void getFrameRange( double& startFrame, double& endFrame ) const;

	/**
	 * @brief Unmapped Frame Rate
	 * The unmaped frame range over which an output clip has images.
	 */
	const double getUnmappedFrameRate() const;

	/**
	 * @brief Unmapped Frame Range -
	 * The unmaped frame range over which an output clip has images.
	 */
	void getUnmappedFrameRange( double& unmappedStartFrame, double& unmappedEndFrame ) const;

	const bool isConnected() const { return _isConnected; }

	/**
	 * @brief Connected
	 * Says whether the clip is actually connected at the moment.
	 */
	void setConnected( const bool isConnected = true ) { _isConnected = isConnected; }

	/**
	 * @brief override this to fill in the image at the given time.
	 * The bounds of the image on the image plane should be
	 * appropriate', typically the value returned in getRegionsOfInterest
	 * on the effect instance. Outside a render call, the optionalBounds should
	 * be 'appropriate' for the.
	 * If bounds is not null, fetch the indicated section of the canonical image plane.
	 */
	tuttle::host::ofx::imageEffect::OfxhImage* getImage( const OfxTime time, const OfxRectD* optionalBounds = NULL );

	/**
	 * @brief override this to return the rod on the clip
	 */
	OfxRectD fetchRegionOfDefinition( const OfxTime time ) const;
};

}
}
}

#endif

