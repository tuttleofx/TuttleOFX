#ifndef _TUTTLE_HOST_OFX_PARAM_PARAMPAGE_HPP_
#define _TUTTLE_HOST_OFX_PARAM_PARAMPAGE_HPP_

#include "OfxhParam.hpp"
#include "OfxhParamDescriptor.hpp"
#include "OfxhParamSet.hpp"

#include <map>

namespace tuttle {
namespace host {
namespace ofx {
namespace attribute {

class OfxhParamPage : public OfxhParam
{
public:
	OfxhParamPage( const OfxhParamDescriptor& descriptor, const std::string& name, OfxhParamSet& setInstance ) : OfxhParam( descriptor, name, setInstance ) {}
	const std::map<int, OfxhParam*>& getChildren() const;

protected:
	mutable std::map<int, OfxhParam*> _children; // if set in a notify hook, this need not be mutable
};

}
}
}
}


#endif

