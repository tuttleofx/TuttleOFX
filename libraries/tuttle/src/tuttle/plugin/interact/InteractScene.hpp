#ifndef _TUTTLE_PLUGIN_INTERACTSCENE_HPP_
#define _TUTTLE_PLUGIN_INTERACTSCENE_HPP_

#include "InteractInfos.hpp"
#include "InteractObject.hpp"
#include "IsActiveFunctor.hpp"
#include <tuttle/plugin/image/gil/globals.hpp>

#include <ofxsParam.h>
#include <boost/ptr_container/ptr_vector.hpp>

namespace tuttle {
namespace plugin {
namespace interact {

/**
 * @brief To create a group of overlay/interact objects.
 * Inherit from InteractObject, because a group of overlay/interact objects is itself an overlay/interact object.
 */
class InteractScene : public InteractObject
{
typedef boost::gil::point2<double> Point2;
typedef boost::ptr_vector<InteractObject> InteractObjectsVector;
typedef boost::ptr_vector<IsActiveFunctor> IsActiveFunctorVector;
typedef std::vector<InteractObject*> InteractObjectsVectorLink;

public:
	InteractScene( OFX::ParamSet& params, const InteractInfos& infos );
	virtual ~InteractScene();

private:
	OFX::ParamSet& _params;
	const InteractInfos& _infos;
	bool _multiSelectionEnabled;
	bool _hasSelection;

	InteractObjectsVector _objects;
	IsActiveFunctorVector _isActive;

	InteractObjectsVectorLink _selected;
	EMoveType _moveType;
	Point2 _moveOffset;
	bool _mouseDown;
	bool _beginSelection;
	OfxRectD _selectionRect;

public:
	InteractObjectsVector&       getObjects()       { return _objects; }
	const InteractObjectsVector& getObjects() const { return _objects; }

	void push_back( InteractObject* obj, IsActiveFunctor* isActive ) { _objects.push_back( obj ); _isActive.push_back( isActive ); }

	bool draw( const OFX::DrawArgs& args );

	bool penMotion( const OFX::PenArgs& args );

	bool penDown( const OFX::PenArgs& args );

	bool penUp( const OFX::PenArgs& args );
};

}
}
}

#endif
