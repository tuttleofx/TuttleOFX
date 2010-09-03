#ifndef _TUTTLE_HOST_OFX_PARAM_PARAMGROUP_HPP_
#define _TUTTLE_HOST_OFX_PARAM_PARAMGROUP_HPP_

#include "OfxhParam.hpp"
#include "OfxhParamSet.hpp"
#include "OfxhParamDescriptor.hpp"

namespace tuttle {
namespace host {
namespace ofx {
namespace attribute {

class OfxhParamGroup
	: public OfxhParam
	, public OfxhParamSet
{
public:
	OfxhParamGroup( const OfxhParamDescriptor& descriptor, const std::string& name, OfxhParamSet& setInstance ) : OfxhParam( descriptor, name, setInstance ) {}
	virtual ~OfxhParamGroup() {}

	void deleteChildrens()
	{
		_paramList.clear();
	}

	void          setChildrens( const OfxhParamSet* childrens );
	OfxhParamSet* getChildrens() const;
	void          addChildren( OfxhParam* children );

	property::OfxhSet& getParamSetProps()
	{
		return _paramSetInstance->getParamSetProps();
	}

	/// The inheriting plugin instance needs to set this up to deal with
	/// plug-ins changing their own values.
	virtual void paramChanged( const attribute::OfxhParam& param, const EChange change )
	{
		_paramSetInstance->paramChanged( param, change );
	}

	virtual OfxhParam* newParam( const OfxhParamDescriptor& descriptor ) OFX_EXCEPTION_SPEC
	{
		return _paramSetInstance->newParam( descriptor );
	}

	/// Triggered when the plug-in calls OfxParameterSuiteV1::paramEditBegin
	virtual void editBegin( const std::string& name ) OFX_EXCEPTION_SPEC
	{
		return _paramSetInstance->editBegin( name );
	}

	/// Triggered when the plug-in calls OfxParameterSuiteV1::paramEditEnd
	virtual void editEnd() OFX_EXCEPTION_SPEC
	{
		return _paramSetInstance->editEnd();
	}

};

}
}
}
}

#endif

