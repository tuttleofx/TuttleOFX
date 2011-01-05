#ifndef _TUTTLE_HOST_OFX_ATTRIBUTE_CLIP_HPP_
#define _TUTTLE_HOST_OFX_ATTRIBUTE_CLIP_HPP_

#include "OfxhAttribute.hpp"
#include "OfxhClipAccessor.hpp"

#include <tuttle/host/ofx/OfxhUtilities.hpp>
#include <tuttle/host/ofx/property/OfxhSet.hpp>
#include <tuttle/host/ofx/property/OfxhGetHook.hpp>
#include <tuttle/host/ofx/property/OfxhNotifyHook.hpp>

namespace tuttle {
namespace host {
namespace ofx {
namespace attribute {

class OfxhClipDescriptor;

/**
 * a clip instance
 */
class OfxhClip
	: public OfxhAttribute
	, protected property::OfxhGetHook
	, protected property::OfxhNotifyHook
	, virtual public OfxhClipAccessor
	, private boost::noncopyable
{
public:
	typedef OfxhClip This;

protected:
	OfxhClip( const OfxhClip& other ) : OfxhAttribute( other ) {}

public:
	OfxhClip( const OfxhClipDescriptor& desc );
	virtual ~OfxhClip() = 0;

	/// clone this clip
	virtual OfxhClip* clone() const = 0;

	virtual bool operator==( const This& other ) const
	{
		if( OfxhAttribute::operator!=( other ) )
			return false;
		return true;
	}

	bool operator!=( const This& other ) const { return !This::operator==( other ); }

	void initHook( const property::OfxhPropSpec* propSpec );

	/// notify override properties

	void notify( const std::string& name, bool isSingle, int indexOrN ) OFX_EXCEPTION_SPEC
	{
		TUTTLE_COUT_WITHINFOS( "What we should do here?" );
		BOOST_THROW_EXCEPTION( OfxhException( kOfxStatErrMissingHostFeature ) );
	}

	// don't know what to do

	void reset( const std::string& name ) OFX_EXCEPTION_SPEC
	{
		TUTTLE_COUT_WITHINFOS( "What we should do here?" );
		BOOST_THROW_EXCEPTION( OfxhException( kOfxStatErrMissingHostFeature ) );
	}

	// Connected -
	//
	//  Says whether the clip is actually connected at the moment.
	virtual const bool isConnected() const = 0;

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize( Archive& ar, const unsigned int version )
	{
		ar& BOOST_SERIALIZATION_BASE_OBJECT_NVP( OfxhAttribute );
	}

};

#ifndef SWIG
/**
 * @brief to make ClipInstance clonable (for use in boost::ptr_container)
 */
inline OfxhClip* new_clone( const OfxhClip& a )
{
	return a.clone();
}

#endif

}
}
}
}

// force boost::is_virtual_base_of value (used by boost::serialization)
namespace boost {
template<>
struct is_virtual_base_of<tuttle::host::ofx::attribute::OfxhAttribute, tuttle::host::ofx::attribute::OfxhClip>: public mpl::true_ {};
}

#endif

