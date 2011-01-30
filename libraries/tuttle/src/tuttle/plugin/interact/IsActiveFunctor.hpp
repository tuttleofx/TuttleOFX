#ifndef _TUTTLE_PLUGIN_ISACTIVEFUNCTOR_HPP_
#define _TUTTLE_PLUGIN_ISACTIVEFUNCTOR_HPP_

#include <tuttle/common/utils/global.hpp>

#include <ofxsParam.h>

#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/foreach.hpp>

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
class IsEnableParamFunctor : public IsActiveFunctor
{
OFX::Param* _param;

public:
	IsEnableParamFunctor( OFX::Param* param )
		: _param( param )
	{}
	bool active() const { return _param->getIsEnable() != negate; }
};

template<bool negate = false>
class IsNotSecretParamFunctor : public IsActiveFunctor
{
OFX::Param* _param;

public:
	IsNotSecretParamFunctor( OFX::Param* param )
		: _param( param )
	{}
	bool active() const { return _param->getIsSecret() == negate; }
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

template<bool negate = false>
class AndActiveFunctor : public IsActiveFunctor
{
	boost::ptr_vector<IsActiveFunctor> _list;
public:
	void push_back( IsActiveFunctor* f )
	{
		_list.push_back(f);
	}

	bool active() const
	{
		BOOST_FOREACH( const IsActiveFunctor& f, _list )
		{
			if( ! f.active() )
			{
				return negate;
			}
		}
		return !negate;
	}
};

template<bool negate = false>
class OrActiveFunctor : public IsActiveFunctor
{
	boost::ptr_vector<IsActiveFunctor> _list;
public:
	void push_back( IsActiveFunctor* f )
	{
		_list.push_back(f);
	}

	bool active() const
	{
		BOOST_FOREACH( const IsActiveFunctor& f, _list )
		{
			if( f.active() )
			{
				return !negate;
			}
		}
		return negate;
	}
};



}
}
}

#endif

