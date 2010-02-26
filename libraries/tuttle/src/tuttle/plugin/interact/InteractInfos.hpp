#ifndef _TUTTLE_PLUGIN_INTERACTINFOS_HPP_
#define	_TUTTLE_PLUGIN_INTERACTINFOS_HPP_

#include <ofxsCore.h>
#include <ofxsParam.h>

#include <boost/gil/utilities.hpp>

namespace tuttle {
namespace plugin {
namespace interact {

class InteractInfos
{
	typedef boost::gil::point2<double> Point2;
public:
    InteractInfos( OFX::ImageEffect* effect )
    : _effect(effect)
	, _focus(false)
	, _lastPenPos(0,0)
    , _penDown(false)
	, _imgSize(0,0)
    {
    }
	OFX::ImageEffect* _effect;
    bool _focus;
    Point2 _lastPenPos;
    bool _penDown;
    Point2 _imgSize;
	static const double _margeIntersect = 0.03;
	static const double _margeDisplay = 0.1;
};

}
}
}

#endif

