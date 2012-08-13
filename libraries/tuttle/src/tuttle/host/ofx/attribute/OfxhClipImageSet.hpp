#ifndef _TUTTLE_HOST_OFX_ATTRIBUTE_CLIPIMAGESET_HPP_
#define _TUTTLE_HOST_OFX_ATTRIBUTE_CLIPIMAGESET_HPP_

#include "OfxhClipImageAccessor.hpp"
#include "OfxhClipDescriptor.hpp"
#include "OfxhClipImage.hpp"

#include <tuttle/host/ofx/OfxhIObject.hpp>

#include <boost/foreach.hpp>

namespace tuttle {
namespace host {
namespace ofx {
namespace imageEffect {
class OfxhImageEffectNodeDescriptor;
}
namespace attribute {

class OfxhClipImageSet
	: virtual public OfxhIObject
	//, public ClipAccessorSet
{
public:
	typedef OfxhClipImageSet This;
	typedef std::map<std::string, OfxhClipImage*> ClipImageMap;
	typedef boost::ptr_vector<OfxhClipImage> ClipImageVector;

protected:
	ClipImageMap _clipImages; ///< clips by name
	ClipImageVector _clipsByOrder; ///< clips list
	bool _clipPrefsDirty; ///< do we need to re-run the clip prefs action

public:
	/// ctor
	///
	/// The propery set being passed in belongs to the owning
	/// plugin instance.
	explicit OfxhClipImageSet();

	explicit OfxhClipImageSet( const OfxhClipImageSet& other );

	/// dtor.
	virtual ~OfxhClipImageSet();

	bool operator==( const This& other ) const;
	bool operator!=( const This& other ) const { return !This::operator==( other ); }
	#ifndef SWIG
	void copyClipsValues( const OfxhClipImageSet& other );

	void populateClips( const imageEffect::OfxhImageEffectNodeDescriptor& descriptor ) OFX_EXCEPTION_SPEC;

	const ClipImageMap& getClips() const
	{
		return _clipImages;
	}

	ClipImageMap& getClips()
	{
		return _clipImages;
	}

	const ClipImageVector& getClipsByOrder() const
	{
		return _clipsByOrder;
	}

	ClipImageVector& getClipsByOrder()
	{
		return _clipsByOrder;
	}

	/**
	 * get the clip
	 */
	OfxhClipImage& getClip( const std::string& name, const bool acceptPartialName = false );
	
	const OfxhClipImage& getClip( const std::string& name, const bool acceptPartialName = false ) const
	{
		 return const_cast<This*>( this )->getClip( name, acceptPartialName );
	}

	const OfxhClipImage* getClipPtr( const std::string& name, const bool acceptPartialName = false ) const;
	
	OfxhClipImage* getClipPtr( const std::string& name, const bool acceptPartialName = false );


	/**
	 * add a clip
	 */
	void addClip( const std::string& name, OfxhClipImage* instance ) OFX_EXCEPTION_SPEC
	{
		if( _clipImages.find( name ) != _clipImages.end() )
			BOOST_THROW_EXCEPTION( OfxhException( kOfxStatErrExists ) );
		_clipImages[name] = instance;
		_clipsByOrder.push_back( instance );
	}

	/**
	 * make a clip instance
	 *
	 * Client host code needs to implement this
	 */
	virtual OfxhClipImage* newClipImage( const OfxhClipImageDescriptor& descriptor ) = 0;

	/**
	 * get the nth clip, in order of declaration
	 */
	OfxhClipImage& getNthClip( int index )
	{
		return _clipsByOrder[index];
	}

	/**
	 * get the nth clip, in order of declaration
	 */
	int getNClips() const
	{
		return int(_clipImages.size() );
	}

	/**
	 * are the clip preferences currently dirty
	 */
	bool areClipPrefsDirty() const
	{
		return _clipPrefsDirty;
	}

private:
	void initMapFromList();
	#endif
};

}
}
}
}

#endif

