#include "ParamPage.hpp"

namespace tuttle {
namespace host {
namespace core {

ParamPage::ParamPage( ImageEffectNode&                                   effect,
                                  const std::string&                                 name,
                                  const ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhParamPage( descriptor, name, effect ),
	_effect( effect )
{}

}
}
}

