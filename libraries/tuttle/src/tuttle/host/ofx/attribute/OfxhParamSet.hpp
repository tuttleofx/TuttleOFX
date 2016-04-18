#ifndef _TUTTLE_HOST_OFX_PARAM_PARAMSET_HPP_
#define _TUTTLE_HOST_OFX_PARAM_PARAMSET_HPP_

#include "OfxhParamSetAccessor.hpp"
#include "OfxhParam.hpp"

#include <tuttle/host/ofx/OfxhIObject.hpp>

#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/foreach.hpp>

#include <map>

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace attribute
{

/// A set of parameters
///
/// As we are the owning object we delete the params inside ourselves. It was tempting
/// to make params autoref objects and have shared ownership with the client code
/// but that adds complexity for no strong gain.
class OfxhParamSet : public OfxhParamSetAccessor, virtual public OfxhIObject
{
public:
    typedef OfxhParamSet This;
    typedef std::map<std::string, OfxhParam*> ParamMap;
    typedef boost::ptr_vector<OfxhParam> ParamVector;

protected:
    ParamMap _params;             ///< params by name
    ParamMap _paramsByScriptName; ///< params by script name
    ParamVector _paramVector;     ///< params list

    /// @group Link to child parameters (no ownership)
    /// @{
    ParamMap _childParams;                     ///< child params by name
    std::vector<OfxhParam*> _childParamVector; ///< child params list
                                               /// @}

public:
    /// The propery set being passed in belongs to the owning
    /// plugin instance.
    explicit OfxhParamSet();

    explicit OfxhParamSet(const OfxhParamSet& other);

    virtual ~OfxhParamSet() = 0;

    OfxhParamSet& operator=(const OfxhParamSet& other);

    void copyParamsValues(const OfxhParamSet& other);

    bool operator==(const This& other) const { return _paramVector == other._paramVector; }

    bool operator!=(const This& other) const { return !This::operator==(other); }

    std::size_t getHashAtTime(const OfxTime time) const;

    /// obtain a handle on this set for passing to the C api
    OfxParamSetHandle getParamSetHandle() const { return (OfxParamSetHandle) this; }

    const ParamMap& getParamsByName() const { return _params; }
    ParamMap& getParamsByName() { return _params; }

    const ParamMap& getChildParamsByName() const { return _childParams; }
    ParamMap& getChildParamsByName() { return _childParams; }

    const ParamMap& getParamsByScriptName() const { return _paramsByScriptName; }
    ParamMap& getParamsByScriptName() { return _paramsByScriptName; }

    const ParamVector& getParamVector() const { return _paramVector; }
    ParamVector& getParamVector() { return _paramVector; }

    const std::vector<OfxhParam*>& getChildParamVector() const { return _childParamVector; }
    std::vector<OfxhParam*>& getChildParamVector() { return _childParamVector; }

    std::size_t getNbParams() const { return _params.size(); }
    std::size_t getNbChildParams() const { return _childParams.size(); }

    OfxhParam& getParam(const std::string& name);
    const OfxhParam& getParam(const std::string& name) const { return const_cast<This*>(this)->getParam(name); }

    OfxhParam& getParamByScriptName(const std::string& scriptName, const bool acceptPartialName = false);
    const OfxhParam& getParamByScriptName(const std::string& name, const bool acceptPartialName = false) const
    {
        return const_cast<This*>(this)->getParamByScriptName(name, acceptPartialName);
    }
    OfxhParam* getParamPtrByScriptName(const std::string& name, const bool acceptPartialName = false);
    const OfxhParam* getParamPtrByScriptName(const std::string& name, const bool acceptPartialName = false) const;

    // get the param
    OfxhParam& getParam(const std::size_t index);
    const OfxhParam& getParam(const std::size_t index) const { return const_cast<This*>(this)->getParam(index); }
    OfxhParam& getChildParam(const std::size_t index);
    const OfxhParam& getChildParam(const std::size_t index) const { return const_cast<This*>(this)->getChildParam(index); }

#ifndef SWIG
    /// The inheriting plugin instance needs to set this up to deal with
    /// plug-ins changing their own values.
    virtual void paramChanged(const attribute::OfxhParam& param, const EChange change) = 0;

    /// Triggered when the plug-in calls OfxParameterSuiteV1::paramEditBegin
    ///
    /// Client host code needs to implement this
    virtual void editBegin(const std::string& name) OFX_EXCEPTION_SPEC = 0;

    /// Triggered when the plug-in calls OfxParameterSuiteV1::paramEditEnd
    ///
    /// Client host code needs to implement this
    virtual void editEnd() OFX_EXCEPTION_SPEC = 0;

    /// reference a param
    //	virtual void referenceParam( const std::string& name, OfxhParam* instance ) OFX_EXCEPTION_SPEC;
    void declareChildParam(OfxhParam& childParam);

protected:
    /// add a param
    virtual void addParam(OfxhParam* instance) OFX_EXCEPTION_SPEC;

    void reserveParameters(const std::size_t size) { _paramVector.reserve(size); }

private:
    void initMapFromList();
#endif
};
}
}
}
}

#endif
