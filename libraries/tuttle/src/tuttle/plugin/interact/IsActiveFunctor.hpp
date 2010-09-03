#ifndef _TUTTLE_PLUGIN_ISACTIVEFUNCTOR_HPP_
#define _TUTTLE_PLUGIN_ISACTIVEFUNCTOR_HPP_

#include <tuttle/common/utils/global.hpp>
#include <ofxsParam.h>

namespace tuttle {
namespace plugin {
namespace interact {

struct IsActiveFunctor
{
	virtual ~IsActiveFunctor()  = 0;
	virtual bool active() const = 0;
};

template<bool negate = false>
struct AlwaysActiveFunctor : public IsActiveFunctor
{
	bool active() const { return !negate; }
};

template<bool negate = false>
class IsActiveBooleanParamFunctor : public IsActiveFunctor
{
OFX::BooleanParam* _param;

public:
	IsActiveBooleanParamFunctor( OFX::BooleanParam* param )
		: _param( param )
	{}

	bool active() const { return _param->getValue() != negate; }
};

template<bool negate = false>
class IsActiveChoiceParamFunctor : public IsActiveFunctor
{
OFX::ChoiceParam* _param;

public:
	IsActiveChoiceParamFunctor( OFX::ChoiceParam* param )
		: _param( param )
	{}

	bool active() const { return _param->getValue() != negate; }
};

}
}
}

#endif

