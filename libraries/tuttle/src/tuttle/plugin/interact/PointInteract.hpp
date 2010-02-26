#ifndef _TUTTLE_PLUGIN_POINTINTERACT_HPP_
#define	_TUTTLE_PLUGIN_POINTINTERACT_HPP_

#include "InteractInfos.hpp"
#include "InteractObject.hpp"
#include <ofxsParam.h>

namespace tuttle {
namespace plugin {
namespace interact {

class PointInteract : public InteractObject
{
public:
	PointInteract( const InteractInfos& infos, OFX::Double2DParam*, bool normalized = true );
	~PointInteract( );
	
private:
	const InteractInfos& _infos;
	OFX::Double2DParam* _param;
	bool _normalized;

public:
	bool draw( const OFX::DrawArgs& args );
};

}
}
}

#endif

