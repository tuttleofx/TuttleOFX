#ifndef _TUTTLE_PLUGIN_INTERACT_SELECTIONMANIPULATOR_HPP_
#define	_TUTTLE_PLUGIN_INTERACT_SELECTIONMANIPULATOR_HPP_

#include "InteractObject.hpp"
#include "InteractInfos.hpp"

namespace tuttle {
namespace plugin {
namespace interact {

class SelectionManipulator : public InteractObject
{
public:
	SelectionManipulator( const InteractInfos& infos );
	virtual ~SelectionManipulator();

	MotionType intersect( const OFX::PenArgs& args )
	{
		const Point2 mouse = ofxToGil( args.penPosition );
		const Point2 p = _selectionPos;

		const double margeCanonical = getMarge() * args.pixelScale.x;
		MotionType m;
		m._mode = eMotionTranslate;
		m._axis = clicPoint( p, mouse, margeCanonical );
		return m;
	}

	double getMarge() const { return _infos._marge * _infos._projectSize.x; }
	
	Point2 getDistance( const Point2& p ) const { return Point2(0.0, 0.0); }

	void setPosition( const Point2& p )
	{
		_selectionPos = p;
	}
	void setPositionX( const Scalar x )
	{
		_selectionPos.x = x;
		_selectionPos.y = getPosition().y;
	}
	void setPositionY( const Scalar y )
	{
		_selectionPos.x = getPosition().x;
		_selectionPos.y = y;
	}

	void beginMove( const Point2& penPosition )
	{
	}
	void endMove( const Point2& penPosition )
	{
	}

protected:
	const InteractInfos& _infos;
private:
	Point2 _selectionPos;
};

}
}
}

#endif

