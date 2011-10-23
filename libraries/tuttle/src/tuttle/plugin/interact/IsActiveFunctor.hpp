#ifndef _TUTTLE_PLUGIN_ISACTIVEFUNCTOR_HPP_
#define _TUTTLE_PLUGIN_ISACTIVEFUNCTOR_HPP_

#include <tuttle/plugin/global.hpp>

#include <ofxsParam.h>

#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/foreach.hpp>

namespace tuttle {
namespace plugin {
namespace interact {

struct IsActiveFunctor
{
	typedef IsActiveFunctor This;
	virtual ~IsActiveFunctor()  = 0;
	virtual bool active() const = 0;
	virtual This* clone() const = 0;

};

inline IsActiveFunctor* new_clone( const IsActiveFunctor& a )
{
    return a.clone();
}

template<bool negate = false>
struct AlwaysActiveFunctor : public IsActiveFunctor
{
	typedef AlwaysActiveFunctor<negate> This;

	bool active() const { return !negate; }
	This* clone() const { return new This(*this); }
};

template<bool negate = false>
class IsEnableParamFunctor : public IsActiveFunctor
{
	typedef IsEnableParamFunctor<negate> This;
	OFX::Param* _param;

public:
	IsEnableParamFunctor( OFX::Param* param )
		: _param( param )
	{}
	bool active() const { return _param->getIsEnable() != negate; }
	This* clone() const { return new This(*this); }
};

template<bool negate = false>
class IsNotSecretParamFunctor : public IsActiveFunctor
{
	typedef IsNotSecretParamFunctor<negate> This;
	OFX::Param* _param;

public:
	IsNotSecretParamFunctor( OFX::Param* param )
		: _param( param )
	{}
	bool active() const { return _param->getIsSecret() == negate; }
	This* clone() const { return new This(*this); }
};

template<bool negate = false>
class IsActiveBooleanParamFunctor : public IsActiveFunctor
{
	typedef IsActiveBooleanParamFunctor<negate> This;
	OFX::BooleanParam* _param;

public:
	IsActiveBooleanParamFunctor( OFX::BooleanParam* param )
		: _param( param )
	{}

	bool active() const { return _param->getValue() != negate; }
	This* clone() const { return new This(*this); }
};

template<bool negate = false>
class IsActiveChoiceParamFunctor : public IsActiveFunctor
{
	typedef IsActiveChoiceParamFunctor<negate> This;
	OFX::ChoiceParam* _param;

public:
	IsActiveChoiceParamFunctor( OFX::ChoiceParam* param )
		: _param( param )
	{}

	bool active() const { return _param->getValue() != negate; }
	This* clone() const { return new This(*this); }
};

template<bool negate = false>
class AndActiveFunctor : public IsActiveFunctor
{
	typedef AndActiveFunctor<negate> This;
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
	This* clone() const { return new This(*this); }
};

template<bool negate = false>
class OrActiveFunctor : public IsActiveFunctor
{
	typedef OrActiveFunctor<negate> This;
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
	This* clone() const { return new This(*this); }
};



}
}
}

#endif

