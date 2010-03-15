#include "ParamPointInteract.hpp"

namespace tuttle {
namespace plugin {
namespace interact {

ParamPointInteract::ParamPointInteract( const InteractInfos& infos, OFX::Double2DParam* param, bool normalized )
: PointInteract( infos, normalized )
, _param( param )
{
	
}

ParamPointInteract::~ParamPointInteract()
{
}


}
}
}
