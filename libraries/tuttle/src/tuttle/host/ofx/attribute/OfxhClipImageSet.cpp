#include "OfxhClipImageSet.hpp"

#include <tuttle/host/ofx/OfxhImageEffectNodeDescriptor.hpp>

namespace tuttle {
namespace host {
namespace ofx {
namespace attribute {

OfxhClipImageSet::OfxhClipImageSet()
	: _clipPrefsDirty( true )
{}

OfxhClipImageSet::OfxhClipImageSet( const OfxhClipImageSet& other )
	: _clipsByOrder( other._clipsByOrder.clone() )
{
	initMapFromList();
}

void OfxhClipImageSet::initMapFromList()
{
	for( ClipImageVector::iterator it = _clipsByOrder.begin(), itEnd = _clipsByOrder.end();
	     it != itEnd;
	     ++it )
	{
		_clips[it->getName()] = &( *it );
	}
}

OfxhClipImageSet::~OfxhClipImageSet()
{}

bool OfxhClipImageSet::operator==( const This& other ) const
{
	return _clipsByOrder == other._clipsByOrder;
}

void OfxhClipImageSet::copyClipsValues( const OfxhClipImageSet& other )
{
	if( _clipsByOrder.size() != other._clipsByOrder.size() )
		BOOST_THROW_EXCEPTION( core::exception::LogicError( "You try to copy clips values, but the two lists are not identical." ) );

	ClipImageVector::const_iterator oit = other._clipsByOrder.begin(), oitEnd = other._clipsByOrder.end();
	for( ClipImageVector::iterator it = _clipsByOrder.begin(), itEnd = _clipsByOrder.end();
	     it != itEnd && oit != oitEnd;
	     ++it, ++oit )
	{
		OfxhClipImage& c = *it;
		const OfxhClipImage& oc = *oit;
		if( c.getName() != oc.getName() )
			BOOST_THROW_EXCEPTION( core::exception::LogicError( "You try to copy clips values, but it is not the same clips in the two lists." ) );
		c.copyValues(oc);
	}
}

void OfxhClipImageSet::populateClips( const imageEffect::OfxhImageEffectNodeDescriptor& descriptor ) OFX_EXCEPTION_SPEC
{
	const imageEffect::OfxhImageEffectNodeDescriptor::ClipImageDescriptorVector& clips = descriptor.getClipsByOrder();

	_clipsByOrder.reserve( clips.size() );
	/// @todo tuttle don't manipulate clip here, delegate to ClipInstanceSet
	for( imageEffect::OfxhImageEffectNodeDescriptor::ClipImageDescriptorVector::const_iterator it = clips.begin(), itEnd = clips.end();
	     it != itEnd;
	     ++it )
	{
		const std::string& name = it->getName();
		// foreach clip descriptor make a ClipImageInstance
		OfxhClipImage* instance = newClipImage( *it ); //( this, *it, counter );
		if( !instance )
			BOOST_THROW_EXCEPTION( OfxhException( "Error on ClipImage creation." ) );

		_clipsByOrder.push_back( instance );
		_clips[name] = instance;
	}
}


}
}
}
}


