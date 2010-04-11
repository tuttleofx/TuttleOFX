#include "ParamGroup.hpp"

namespace tuttle {
namespace host {
namespace core {

ParamGroup::ParamGroup( ImageEffectNode&                                   effect,
					  const std::string&                                 name,
					  const ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhParamGroup( descriptor, name, effect ),
	_effect( effect )
{}

}
}
}

