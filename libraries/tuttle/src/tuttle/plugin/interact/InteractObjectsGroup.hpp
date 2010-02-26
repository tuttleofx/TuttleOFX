#ifndef _TUTTLE_PLUGIN_INTERACTOBJECTS_HPP_
#define	_TUTTLE_PLUGIN_INTERACTOBJECTS_HPP_

#include "InteractObject.hpp"

#include <boost/ptr_container/ptr_vector.hpp>

namespace tuttle {
namespace plugin {
namespace interact {

/**
 * @brief To create a group of overlay/interact objects.
 * Inherit from InteractObject, because a group of overlay/interact objects is itself an overlay/interact object.
 */
class InteractObjectsGroup : public InteractObject
{
	typedef boost::ptr_vector<InteractObject> InteractObjectsVector;
public:
	InteractObjectsGroup( );
	~InteractObjectsGroup( );
private:
	InteractObjectsVector _objects;

public:
	InteractObjectsVector& getObjects() { return _objects; }
	const InteractObjectsVector& getObjects() const { return _objects; }
	
	void push_back( InteractObject* obj ) { _objects.push_back(obj); }
	
	bool draw( const OFX::DrawArgs& args )
	{
		bool result = false;
		for( InteractObjectsVector::iterator it = _objects.begin(), itEnd = _objects.end();
		     it != itEnd;
		     ++it )
		{
			result |= it->draw(args);
		}
		return result;
	}

	bool penMotion( const OFX::PenArgs& args )
	{
		bool result = false;
		for( InteractObjectsVector::iterator it = _objects.begin(), itEnd = _objects.end();
		     it != itEnd;
		     ++it )
		{
			result |= it->penMotion(args);
		}
		return result;
	}

	bool penDown( const OFX::PenArgs& args )
	{
		for( InteractObjectsVector::iterator it = _objects.begin(), itEnd = _objects.end();
		     it != itEnd;
		     ++it )
		{
			if( it->penDown(args) )
				return true;
		}
		// if( _multiselectionEnabled )
		// {
		// }
		return false;
	}

	/**
	 * nedded ?
	 */
	bool penUp( const OFX::PenArgs& args )
	{
		bool result = false;
		for( InteractObjectsVector::iterator it = _objects.begin(), itEnd = _objects.end();
		     it != itEnd;
		     ++it )
		{
			result |= it->penUp(args);
		}
		return result;
	}
};

}
}
}

#endif
