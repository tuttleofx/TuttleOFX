#ifndef _TUTTLE_HOST_OFX_ATTRIBUTE_CLIPIMAGE_HPP_
#define _TUTTLE_HOST_OFX_ATTRIBUTE_CLIPIMAGE_HPP_

#include "OfxhClip.hpp"
#include "OfxhClipImageDescriptor.hpp"

#include <tuttle/host/ofx/OfxhImage.hpp>

#include <tuttle/common/ofx/imageEffect.hpp>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

namespace tuttle {
namespace host {
namespace ofx {

namespace imageEffect {
using namespace ::tuttle::ofx::imageEffect;
class OfxhImageEffectNode;
class OfxhImageEffectNodeDescriptor;
}
namespace attribute {

/**
 * @brief a clip image instance
 */
class OfxhClipImage
	: public attribute::OfxhClip
	, virtual public OfxhClipImageAccessor
{
public:
	typedef OfxhClipImage This;

public:
	OfxhClipImage( const attribute::OfxhClipImageDescriptor& desc );
	OfxhClipImage( const OfxhClipImage& other );

	virtual ~OfxhClipImage() {}

	bool operator==( const This& other ) const
	{
		if( OfxhClip::operator!=( other ) )
			return false;
		return true;
	}

	bool operator!=( const This& other ) const { return !This::operator==( other ); }

	virtual OfxhClipImage* clone() const                    = 0;
	virtual std::string    getFullName() const              = 0;
	virtual std::string    getConnectedClipFullName() const = 0; ///< @todo tuttle: remove this!
	virtual std::string    getIdentifier() const = 0;

	/**
	 * get a handle on the clip descriptor for the C api
	 */
	OfxImageClipHandle getOfxImageClipHandle() const
	{
		return ( OfxImageClipHandle ) this;
	}

	/**
	 * @brief fetch depth of all chromatic component in this clip
	 *
	 *   kOfxBitDepthNone (implying a clip is unconnected, not valid for an image)
	 *   kOfxBitDepthByte
	 *   kOfxBitDepthShort
	 *   kOfxBitDepthFloat
	 */
	const std::string& getBitDepthString() const
	{
		return getProperties().getStringProperty( kOfxImageEffectPropPixelDepth );
	}

	/**
	 * @brief fetch depth of all chromatic component in this clip
	 *
	 * 0 (implying a clip is unconnected, not valid for an image),
	 * 8,
	 * 16,
	 * 32
	 */
	imageEffect::EBitDepth getBitDepth() const
	{
		const std::string& s = getBitDepthString();

		return imageEffect::mapBitDepthStringToEnum( s );
	}

	/** set the current pixel depth
	 * called by clip preferences action
	 */
	void setBitDepthString( const std::string& s, const property::EModifiedBy modifiedBy = property::eModifiedByHost )
	{
		property::String& prop = getEditableProperties().fetchLocalStringProperty( kOfxImageEffectPropPixelDepth );

		prop.setValue( s, 0, modifiedBy );
	}

	void setBitDepth( const imageEffect::EBitDepth bitDepth, const property::EModifiedBy modifiedBy = property::eModifiedByHost )
	{
		setBitDepthString( mapBitDepthEnumToString( bitDepth ), modifiedBy );
	}

	void setBitDepthStringIfUpper( const std::string& s )
	{
		property::String& prop = getEditableProperties().fetchLocalStringProperty( kOfxImageEffectPropPixelDepth );

		if( ofx::imageEffect::mapBitDepthStringToEnum( s ) > getBitDepth() ) // we can increase the bit depth but not decrease
			prop.setValue( s );
	}

	void setBitDepthStringIfUpperAndNotModifiedByPlugin( const std::string& s )
	{
		property::String& prop = getEditableProperties().fetchLocalStringProperty( kOfxImageEffectPropPixelDepth );

		if( prop.getModifiedBy() != property::eModifiedByPlugin && // if not modified by plugin
		    ofx::imageEffect::mapBitDepthStringToEnum( s ) > getBitDepth() ) // we can increase the bit depth but not decrease
			prop.setValue( s );
	}

	/** Pixel Aspect Ratio
	 *  The pixel aspect ratio of a clip or image.
	 */
	const double getPixelAspectRatio() const
	{
		return getProperties().getDoubleProperty( kOfxImagePropPixelAspectRatio );
	}

	/**
	 * set the current pixel aspect ratio
	 * called by clip preferences action
	 */
	void setPixelAspectRatio( const double& s, const property::EModifiedBy modifiedBy = property::eModifiedByHost )
	{
		property::Double& prop = getEditableProperties().fetchLocalDoubleProperty( kOfxImagePropPixelAspectRatio );

		prop.setValue( s, 0, modifiedBy );
	}

	/** Components that can be fetched from this clip -
	 *
	 *  kOfxImageComponentNone (implying a clip is unconnected, not valid for an image)
	 *  kOfxImageComponentRGBA
	 *  kOfxImageComponentAlpha
	 *  and any custom ones you may think of
	 */
	const std::string& getComponentsString() const
	{
		return getProperties().getStringProperty( kOfxImageEffectPropComponents );
	}

	/** Components that can be fetched from this clip -
	 *
	 *  kOfxImageComponentNone (implying a clip is unconnected, not valid for an image)
	 *  kOfxImageComponentRGBA
	 *  kOfxImageComponentAlpha
	 *  and any custom ones you may think of
	 */
	const imageEffect::EPixelComponent getComponents() const
	{
		return imageEffect::mapPixelComponentStringToEnum( getComponentsString() );
	}

	/**
	 * Number of values for this Components.
	 */
	const std::size_t getNbComponents() const
	{
		return imageEffect::numberOfComponents( getComponents() );
	}

	/**
	 * set the current set of components
	 * called by clip preferences action
	 */
	void setComponents( const std::string& s, const property::EModifiedBy modifiedBy = property::eModifiedByHost )
	{
		property::String& prop = getEditableProperties().fetchLocalStringProperty( kOfxImageEffectPropComponents );

		prop.setValue( s, 0, modifiedBy );
	}

	void setComponentsIfNotModifiedByPlugin( const std::string& s )
	{
		property::String& prop = getEditableProperties().fetchLocalStringProperty( kOfxImageEffectPropComponents );

		if( prop.getModifiedBy() != property::eModifiedByPlugin )
			prop.setValue( s );
	}

	/** Get the Raw Unmapped Pixel Depth from the host for chromatic planes
	 *
	 *  @returns
	 *     - kOfxBitDepthNone (implying a clip is unconnected image)
	 *     - kOfxBitDepthByte
	 *     - kOfxBitDepthShort
	 *     - kOfxBitDepthFloat
	 */
	virtual const std::string& getUnmappedBitDepth() const //= 0;
	{
		return getProperties().getStringProperty( kOfxImageClipPropUnmappedPixelDepth );
	}

	/** Get the Raw Unmapped Components from the host
	 *
	 *  @returns
	 *      - kOfxImageComponentNone (implying a clip is unconnected, not valid for an image)
	 *      - kOfxImageComponentRGBA
	 *      - kOfxImageComponentAlpha
	 */
	virtual const std::string& getUnmappedComponents() const //= 0;
	{
		return getProperties().getStringProperty( kOfxImageClipPropUnmappedComponents );
	}

	/** PreMultiplication -
	 *
	 *  kOfxImageOpaque - the image is opaque and so has no premultiplication state
	 *  kOfxImagePreMultiplied - the image is premultiplied by it's alpha
	 *  kOfxImageUnPreMultiplied - the image is unpremultiplied
	 */
	virtual const std::string& getPremult() const //= 0;
	{
		return getProperties().getStringProperty( kOfxImageEffectPropPreMultiplication );
	}

	/** Frame Rate -
	 *
	 *  The frame rate of a clip or instance's project.
	 */
	virtual double getFrameRate() const //= 0;
	{
		return getProperties().getDoubleProperty( kOfxImageEffectPropFrameRate );
	}

	/** Frame Range (startFrame, endFrame) -
	 *
	 *  The frame range over which a clip has images.
	 */
	virtual void getFrameRange( double& startFrame, double& endFrame ) const //= 0;
	{
		startFrame = getProperties().getDoubleProperty( kOfxImageEffectPropFrameRange, 0 );
		endFrame   = getProperties().getDoubleProperty( kOfxImageEffectPropFrameRange, 1 );
	}

	/**  Field Order - Which spatial field occurs temporally first in a frame.
	 *  @returns
	 *   - kOfxImageFieldNone - the clip material is unfielded
	 *   - kOfxImageFieldLower - the clip material is fielded, with image rows 0,2,4.... occuring first in a frame
	 *   - kOfxImageFieldUpper - the clip material is fielded, with image rows line 1,3,5.... occuring first in a frame
	 */
	virtual const std::string& getFieldOrder() const //= 0;
	{
		return getProperties().getStringProperty( kOfxImageClipPropFieldOrder );
	}

	/**
	 * @todo tuttle: This function has been added here. Why was it not before?
	 */
	virtual const std::string& getFieldExtraction() const //= 0;
	{
		return getProperties().getStringProperty( kOfxImageClipPropFieldExtraction );
	}

	/** Connected -
	 *
	 *  Says whether the clip is actually connected at the moment.
	 */
	virtual const bool isConnected() const = 0;
	//	{
	//		return getProperties().getDoubleProperty( kOfxImageClipPropConnected );
	//	}

	/** Unmapped Frame Rate -
	 *
	 *  The unmaped frame range over which an output clip has images.
	 */
	virtual const double getUnmappedFrameRate() const //= 0;
	{
		return getProperties().getDoubleProperty( kOfxImageEffectPropUnmappedFrameRate );
	}

	/** Unmapped Frame Range -
	 *
	 *  The unmaped frame range over which an output clip has images.
	 */
	virtual void getUnmappedFrameRange( double& unmappedStartFrame, double& unmappedEndFrame ) const //= 0;
	{
		unmappedStartFrame = getProperties().getDoubleProperty( kOfxImageEffectPropUnmappedFrameRange, 0 );
		unmappedEndFrame   = getProperties().getDoubleProperty( kOfxImageEffectPropUnmappedFrameRange, 1 );
	}

	/** Continuous Samples -
	 *
	 *  0 if the images can only be sampled at discreet times (eg: the clip is a sequence of frames),
	 *  1 if the images can only be sampled continuously (eg: the clip is infact an animating roto spline and can be rendered anywhen).
	 */
	virtual const bool getContinuousSamples() const //= 0;
	{
		return getProperties().getDoubleProperty( kOfxImageClipPropContinuousSamples ) != 0;
	}

	/**  override this to fill in the image at the given time.
	 *  The bounds of the image on the image plane should be
	 *  'appropriate', typically the value returned in getRegionsOfInterest
	 *  on the effect instance. Outside a render call, the optionalBounds should
	 *  be 'appropriate' for the.
	 *  If bounds is not null, fetch the indicated section of the canonical image plane.
	 */
	virtual tuttle::host::ofx::imageEffect::OfxhImage* getImage( const OfxTime time, const OfxRectD* optionalBounds = NULL ) = 0;

	/// override this to return the rod on the clip
	virtual OfxRectD fetchRegionOfDefinition( const OfxTime time ) const = 0;

	/** given the colour component, find the nearest set of supported colour components
	 *  override this for extra wierd custom component depths
	 */
	virtual const std::string& findSupportedComp( const std::string& s ) const;
};

#ifndef SWIG
/**
 * @brief to make ClipImageInstance clonable (for use in boost::ptr_container)
 */
inline OfxhClipImage* new_clone( const OfxhClipImage& a )
{
	return a.clone();
}

#endif

}
}
}
}

#ifndef SWIG
// force boost::is_virtual_base_of value (used by boost::serialization)
namespace boost {
template<>
struct is_virtual_base_of<tuttle::host::ofx::attribute::OfxhClip, tuttle::host::ofx::attribute::OfxhClipImage>: public mpl::true_ {};
}
#endif

#endif
