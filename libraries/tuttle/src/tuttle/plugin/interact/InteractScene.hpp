#ifndef _TUTTLE_PLUGIN_INTERACTSCENE_HPP_
#define _TUTTLE_PLUGIN_INTERACTSCENE_HPP_

#include "InteractInfos.hpp"
#include "InteractObject.hpp"
#include "IsActiveFunctor.hpp"
#include "Color.hpp"
#include "SelectionManipulator.hpp"

#include <tuttle/plugin/image/gil/globals.hpp>

#include <ofxsParam.h>

#include <boost/ptr_container/ptr_vector.hpp>
#include <utility>

namespace tuttle {
namespace plugin {
namespace interact {

/**
 * @brief To create a group of overlay/interact objects.
 * Inherit from InteractObject, because a group of overlay/interact objects is itself an overlay/interact object.
 */
class InteractScene : public InteractObject
{
public:
	typedef double Scalar;
	typedef boost::gil::point2<Scalar> Point2;
	typedef boost::ptr_vector<InteractObject> InteractObjectsVector;
	typedef boost::ptr_vector<IsActiveFunctor> IsActiveFunctorVector;
	typedef boost::ptr_vector<IColor> ColorVector;

	typedef std::pair< InteractObject*, Point2 > SelectedObject;
	typedef std::vector< SelectedObject > SelectedObjectVector;

public:
	InteractScene( OFX::ParamSet& params, const InteractInfos& infos );
	virtual ~InteractScene();
	
private:
	OFX::ParamSet& _params;
	const InteractInfos& _infos;
	bool _mouseDown;
	MotionType _motionType;

	InteractObjectsVector _objects;
	IsActiveFunctorVector _isActive;
	ColorVector _colors;

	Point2 _beginPenPosition;

	bool _multiSelectionEnabled;
	bool _creatingSelection;
	InteractObject* _manipulator;
	IColor* _manipulatorColor;
	bool _hasSelection;
	SelectedObjectVector _selected;
	OfxRectD _selectionRect;
	
public:
	InteractObjectsVector&       getObjects()       { return _objects; }
	const InteractObjectsVector& getObjects() const { return _objects; }
	
	void push_back( InteractObject* obj, IsActiveFunctor* isActive = new AlwaysActiveFunctor<>(), IColor* color = new Color() )
	{
		_objects.push_back( obj );
		_isActive.push_back( isActive );
		_colors.push_back( color );
	}
	void setManipulator( InteractObject* obj, IColor* color = new Color() )
	{
		_manipulator = obj;
		_manipulatorColor = color;
	}


	bool draw( const OFX::DrawArgs& args );

	bool penMotion( const OFX::PenArgs& args );

	bool penDown( const OFX::PenArgs& args );

	bool penUp( const OFX::PenArgs& args );

private:
	SelectedObjectVector&       getSelectedObjects()       { return _selected; }
	const SelectedObjectVector& getSelectedObjects() const { return _selected; }

	bool drawSelection( const OFX::DrawArgs& args );

	void translate( const Point2& vec );
	void rotate( const Point2& center, const Point2& from, const Point2& vec );
	void scale( const Point2& center, const Point2& factor );
};

}
}
}

#endif
