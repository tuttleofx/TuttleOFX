#include "ParamPushButton.hpp"

namespace tuttle {
namespace host {
namespace core {

ParamPushButton::ParamPushButton( ImageEffectNode&                                   effect,
                                  const std::string&                                 name,
                                  const ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhParamPushButton( descriptor, name, effect ),
	_effect( effect )
{}


}
}
}

