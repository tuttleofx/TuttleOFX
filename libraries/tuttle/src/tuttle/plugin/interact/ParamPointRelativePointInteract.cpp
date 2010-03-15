#include "ParamPointRelativePointInteract.hpp"

#include "interact.hpp"
#include "overlay.hpp"

#include <tuttle/common/utils/global.hpp>
#include <tuttle/common/image/gilGlobals.hpp>

namespace tuttle {
namespace plugin {
namespace interact {

ParamPointRelativePointInteract::ParamPointRelativePointInteract( const InteractInfos& infos, OFX::Double2DParam* param, OFX::Double2DParam* relativeParam, bool normalized )
: PointInteract( infos, normalized )
, _param( param )
, _relativeParam( relativeParam )
{
}

ParamPointRelativePointInteract::~ParamPointRelativePointInteract( ) { }

}
}
}
