#ifndef _TUTTLE_HOST_OFX_PARAM_HPP_
#define _TUTTLE_HOST_OFX_PARAM_HPP_

#include "OfxhAttribute.hpp"
#include "OfxhParamAccessor.hpp"

/* Definitions used by setInterpolator() for animated params
   These are TuttleOFX specific */
#include <tuttle/host/attribute/ValueInterpolator.hpp>

#include <tuttle/host/ofx/OfxhCore.hpp>
#include <tuttle/host/ofx/OfxhException.hpp>
#include <tuttle/host/ofx/OfxhIObject.hpp>

#include <tuttle/host/ofx/property/OfxhSet.hpp>
#include <tuttle/host/ofx/property/OfxhGetHook.hpp>
#include <tuttle/host/ofx/property/OfxhNotifyHook.hpp>

#include <string>

#define TUTTLE_DEFINE_OFXHPARAM_ACCESSORS(NAME, TYPE)                                                                       \
protected:                                                                                                                  \
    inline virtual void getValue(TYPE&) const OFX_EXCEPTION_SPEC                                                            \
    {                                                                                                                       \
        BOOST_THROW_EXCEPTION(OfxhException(kOfxStatErrBadHandle, "\"" + this->getName() + "\"" + " is not a " #NAME        \
                                                                                                  " parameter (" +          \
                                                                      this->getParamType() + ")."));                        \
    }                                                                                                                       \
    inline virtual void getValueAtTime(const OfxTime, TYPE&) const OFX_EXCEPTION_SPEC                                       \
    {                                                                                                                       \
        BOOST_THROW_EXCEPTION(OfxhException(kOfxStatErrBadHandle, "\"" + this->getName() + "\"" + " is not a " #NAME        \
                                                                                                  " parameter (" +          \
                                                                      this->getParamType() + ")."));                        \
    }                                                                                                                       \
    inline virtual void getValueAtIndex(const std::size_t, TYPE&) const OFX_EXCEPTION_SPEC                                  \
    {                                                                                                                       \
        BOOST_THROW_EXCEPTION(OfxhException(kOfxStatErrBadHandle, "\"" + this->getName() + "\"" +                           \
                                                                      " is not a " #NAME " multidimentional parameter (" +  \
                                                                      this->getParamType() + ")."));                        \
    }                                                                                                                       \
    inline virtual void getValueAtTimeAndIndex(const OfxTime, const std::size_t, TYPE&) const OFX_EXCEPTION_SPEC            \
    {                                                                                                                       \
        BOOST_THROW_EXCEPTION(OfxhException(kOfxStatErrBadHandle, "\"" + this->getName() + "\"" +                           \
                                                                      " is not a " #NAME " multidimentional parameter (" +  \
                                                                      this->getParamType() + ")."));                        \
    }                                                                                                                       \
                                                                                                                            \
public:                                                                                                                     \
    inline TYPE get##NAME##ValueAtIndex(const std::size_t index) const OFX_EXCEPTION_SPEC                                   \
    {                                                                                                                       \
        TYPE dst;                                                                                                           \
        getValueAtIndex(index, dst);                                                                                        \
        return dst;                                                                                                         \
    }                                                                                                                       \
    inline TYPE get##NAME##ValueAtTimeAndIndex(const OfxTime time, const std::size_t index) const OFX_EXCEPTION_SPEC        \
    {                                                                                                                       \
        TYPE dst;                                                                                                           \
        getValueAtTimeAndIndex(time, index, dst);                                                                           \
        return dst;                                                                                                         \
    }                                                                                                                       \
    inline TYPE get##NAME##Value() const OFX_EXCEPTION_SPEC                                                                 \
    {                                                                                                                       \
        TYPE dst;                                                                                                           \
        getValue(dst);                                                                                                      \
        return dst;                                                                                                         \
    }                                                                                                                       \
    inline TYPE get##NAME##ValueAtTime(const OfxTime time) const OFX_EXCEPTION_SPEC                                         \
    {                                                                                                                       \
        TYPE dst;                                                                                                           \
        getValueAtTime(time, dst);                                                                                          \
        return dst;                                                                                                         \
    }                                                                                                                       \
    inline virtual void setValue(const TYPE& value, const EChange change) OFX_EXCEPTION_SPEC                                \
    {                                                                                                                       \
        BOOST_THROW_EXCEPTION(                                                                                              \
            OfxhException(kOfxStatErrBadHandle, "\"" + this->getName() + "\"" + " is not a " #NAME " parameter (" +         \
                                                    this->getParamType() + ", " + mapChangeEnumToString(change) + ")."));   \
    }                                                                                                                       \
    inline virtual void setValue(const TYPE& value) OFX_EXCEPTION_SPEC { setValue(value, eChangeUserEdited); }              \
    inline virtual void setValueAtTime(const OfxTime time, const TYPE& value, const EChange change) OFX_EXCEPTION_SPEC      \
    {                                                                                                                       \
        BOOST_THROW_EXCEPTION(                                                                                              \
            OfxhException(kOfxStatErrBadHandle, "\"" + this->getName() + "\"" + " is not a " #NAME " parameter (" +         \
                                                    this->getParamType() + ", " + mapChangeEnumToString(change) + ")."));   \
    }                                                                                                                       \
    inline virtual void setValueAtTime(const OfxTime time, const TYPE& value) OFX_EXCEPTION_SPEC                            \
    {                                                                                                                       \
        setValueAtTime(time, value, eChangeUserEdited);                                                                     \
    }                                                                                                                       \
    inline virtual void setValueAtIndex(const std::size_t index, const TYPE& value, const EChange change)                   \
        OFX_EXCEPTION_SPEC                                                                                                  \
    {                                                                                                                       \
        BOOST_THROW_EXCEPTION(                                                                                              \
            OfxhException(kOfxStatErrBadHandle, "\"" + this->getName() + "\"" + " is not a multi-" #NAME " parameter (" +   \
                                                    this->getParamType() + ", " + mapChangeEnumToString(change) + ")."));   \
    }                                                                                                                       \
    inline virtual void setValueAtIndex(const std::size_t index, const TYPE& value) OFX_EXCEPTION_SPEC                      \
    {                                                                                                                       \
        setValueAtIndex(index, value, eChangeUserEdited);                                                                   \
    }                                                                                                                       \
    inline virtual void setValueAtTimeAndIndex(const OfxTime time, const std::size_t index, const TYPE& value,              \
                                               const EChange change) OFX_EXCEPTION_SPEC                                     \
    {                                                                                                                       \
        BOOST_THROW_EXCEPTION(                                                                                              \
            OfxhException(kOfxStatErrBadHandle, "\"" + this->getName() + "\"" + " is not a multi-" #NAME " parameter (" +   \
                                                    this->getParamType() + ", " + mapChangeEnumToString(change) + ")."));   \
    }                                                                                                                       \
    inline virtual void setValueAtTimeAndIndex(const OfxTime time, const std::size_t index, const TYPE& value)              \
        OFX_EXCEPTION_SPEC                                                                                                  \
    {                                                                                                                       \
        setValueAtTimeAndIndex(time, index, value, eChangeUserEdited);                                                      \
    }                                                                                                                       \
                                                                                                                            \
    inline virtual void setValue(const std::vector<TYPE>& values, const EChange change) OFX_EXCEPTION_SPEC                  \
    {                                                                                                                       \
        BOOST_THROW_EXCEPTION(                                                                                              \
            OfxhException(kOfxStatErrBadHandle, "\"" + this->getName() + "\"" + " is not a multi-" #NAME " parameter (" +   \
                                                    this->getParamType() + ", " + mapChangeEnumToString(change) + ")."));   \
    }                                                                                                                       \
    inline virtual void setValue(const std::vector<TYPE>& values) OFX_EXCEPTION_SPEC                                        \
    {                                                                                                                       \
        setValue(values, eChangeUserEdited);                                                                                \
    }                                                                                                                       \
    inline virtual void setValueAtTime(const OfxTime time, const std::vector<TYPE>& values, const EChange change)           \
        OFX_EXCEPTION_SPEC                                                                                                  \
    {                                                                                                                       \
        BOOST_THROW_EXCEPTION(                                                                                              \
            OfxhException(kOfxStatErrBadHandle, "\"" + this->getName() + "\"" + " is not a multi-" #NAME " parameter (" +   \
                                                    this->getParamType() + ", " + mapChangeEnumToString(change) + ")."));   \
    }                                                                                                                       \
    inline virtual void setValueAtTime(const OfxTime time, const std::vector<TYPE>& values) OFX_EXCEPTION_SPEC              \
    {                                                                                                                       \
        setValueAtTime(time, values, eChangeUserEdited);                                                                    \
    }                                                                                                                       \
//

#define TUTTLE_DEFINE_OFXHPARAM_MULTIDIM_ACCESSORS(NAME, TYPE)                                                              \
    inline virtual void setValue(const TYPE& value1, const TYPE& value2, const EChange change) OFX_EXCEPTION_SPEC           \
    {                                                                                                                       \
        BOOST_THROW_EXCEPTION(OfxhException(kOfxStatErrBadHandle,                                                           \
                                            "\"" + this->getName() + "\"" + " is not a multi-" #NAME " parameter (2) (" +   \
                                                this->getParamType() + ", " + mapChangeEnumToString(change) + ")."));       \
    }                                                                                                                       \
    inline virtual void setValue(const TYPE& value1, const TYPE& value2) OFX_EXCEPTION_SPEC                                 \
    {                                                                                                                       \
        setValue(value1, value2, eChangeUserEdited);                                                                        \
    }                                                                                                                       \
    inline virtual void setValueAtTime(const OfxTime time, const TYPE& value1, const TYPE& value2, const EChange change)    \
        OFX_EXCEPTION_SPEC                                                                                                  \
    {                                                                                                                       \
        BOOST_THROW_EXCEPTION(OfxhException("\"" + this->getName() + "\"" + " is not a multi-" #NAME " parameter (2) (" +   \
                                            this->getParamType() + ", " + mapChangeEnumToString(change) + ")."));           \
    }                                                                                                                       \
    inline virtual void setValueAtTime(const OfxTime time, const TYPE& value1, const TYPE& value2) OFX_EXCEPTION_SPEC       \
    {                                                                                                                       \
        setValueAtTime(time, value1, value2, eChangeUserEdited);                                                            \
    }                                                                                                                       \
                                                                                                                            \
    inline virtual void setValue(const TYPE& value1, const TYPE& value2, const TYPE& value3, const EChange change)          \
        OFX_EXCEPTION_SPEC                                                                                                  \
    {                                                                                                                       \
        BOOST_THROW_EXCEPTION(OfxhException(kOfxStatErrBadHandle,                                                           \
                                            "\"" + this->getName() + "\"" + " is not a multi-" #NAME " parameter (3) (" +   \
                                                this->getParamType() + ", " + mapChangeEnumToString(change) + ")."));       \
    }                                                                                                                       \
    inline virtual void setValue(const TYPE& value1, const TYPE& value2, const TYPE& value3) OFX_EXCEPTION_SPEC             \
    {                                                                                                                       \
        setValue(value1, value2, value3, eChangeUserEdited);                                                                \
    }                                                                                                                       \
    inline virtual void setValueAtTime(const OfxTime time, const TYPE& value1, const TYPE& value2, const TYPE& value3,      \
                                       const EChange change) OFX_EXCEPTION_SPEC                                             \
    {                                                                                                                       \
        BOOST_THROW_EXCEPTION(                                                                                              \
            OfxhException(kOfxStatErrBadHandle, "\"" + this->getName() + "\"" + " is not a " #NAME " parameter (3) (" +     \
                                                    this->getParamType() + ", " + mapChangeEnumToString(change) + ")."));   \
    }                                                                                                                       \
    inline virtual void setValueAtTime(const OfxTime time, const TYPE& value1, const TYPE& value2, const TYPE& value3)      \
        OFX_EXCEPTION_SPEC                                                                                                  \
    {                                                                                                                       \
        setValueAtTime(time, value1, value2, value3, eChangeUserEdited);                                                    \
    }                                                                                                                       \
                                                                                                                            \
    inline virtual void setValue(const TYPE& value1, const TYPE& value2, const TYPE& value3, const TYPE& value4,            \
                                 const EChange change) OFX_EXCEPTION_SPEC                                                   \
    {                                                                                                                       \
        BOOST_THROW_EXCEPTION(OfxhException(kOfxStatErrBadHandle,                                                           \
                                            "\"" + this->getName() + "\"" + " is not a multi-" #NAME " parameter (4) (" +   \
                                                this->getParamType() + ", " + mapChangeEnumToString(change) + ")."));       \
    }                                                                                                                       \
    inline virtual void setValue(const TYPE& value1, const TYPE& value2, const TYPE& value3, const TYPE& value4)            \
        OFX_EXCEPTION_SPEC                                                                                                  \
    {                                                                                                                       \
        setValue(value1, value2, value3, value4, eChangeUserEdited);                                                        \
    }                                                                                                                       \
    inline virtual void setValueAtTime(const OfxTime time, const TYPE& value1, const TYPE& value2, const TYPE& value3,      \
                                       const TYPE& value4, const EChange change) OFX_EXCEPTION_SPEC                         \
    {                                                                                                                       \
        BOOST_THROW_EXCEPTION(                                                                                              \
            OfxhException(kOfxStatErrBadHandle, "\"" + this->getName() + "\"" + " is not a " #NAME " parameter (4) (" +     \
                                                    this->getParamType() + ", " + mapChangeEnumToString(change) + ")."));   \
    }                                                                                                                       \
    inline virtual void setValueAtTime(const OfxTime time, const TYPE& value1, const TYPE& value2, const TYPE& value3,      \
                                       const TYPE& value4) OFX_EXCEPTION_SPEC                                               \
    {                                                                                                                       \
        setValueAtTime(time, value1, value2, value3, value4, eChangeUserEdited);                                            \
    }                                                                                                                       \
//

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace attribute
{

/* Specify the type of interpolator to use with animated params */
enum EInterpolatorType
{
    eLinearInterpolator,
    eSmoothInterpolator,
    eFastInterpolator,
    eSlowInterpolator,
};

class OfxhParamDescriptor;
class OfxhParamSet;

/// plugin parameter instance
class OfxhParam : public OfxhAttribute,
                  virtual public OfxhParamAccessor,
                  protected property::OfxhNotifyHook,
                  private boost::noncopyable
{
    OfxhParam();

public:
    typedef OfxhParam This;

protected:
    OfxhParamSet* _paramSetInstance;
    OfxhParam* _parentInstance;
    bool _avoidRecursion; ///< Avoid recursion when updating with paramChangedAction

protected:
    OfxhParam(const OfxhParam& other)
        : OfxhAttribute(other)
        , _paramSetInstance(other._paramSetInstance)
        , _parentInstance(other._parentInstance)
        , _avoidRecursion(false)
    {
        /// @todo tuttle : copy content, not pointer ?
    }

public:
    /// make a parameter, with the given type and name
    explicit OfxhParam(const OfxhParamDescriptor& descriptor, const std::string& name, OfxhParamSet& setInstance);

    virtual ~OfxhParam() = 0;

#ifndef SWIG
    /// clone this parameter
    virtual This* clone() const = 0;
#endif

    virtual bool paramTypeHasData() const = 0;

    virtual std::size_t getHashAtTime(const OfxTime time) const = 0;

    /**
     * @todo tuttle: check values !!!
     */
    bool operator==(const This& p) const { return true; }

    /// grab a handle on the parameter for passing to the C API
    OfxParamHandle getParamHandle() const { return (OfxParamHandle) this; }

    friend std::ostream& operator<<(std::ostream& os, const This& g);
    virtual std::ostream& displayValues(std::ostream& os) const { return os; }

#ifdef SWIG
    % extend
    {
        ofx::property::OfxhProperty& __getitem__(const std::string& name)
        {
            return self->getEditableProperties().fetchLocalProperty(name);
        }

        std::string __str__() const
        {
            std::stringstream s;

            s << *self;
            return s.str();
        }
    }
#endif

    void paramChanged(const EChange change);

#ifndef SWIG
    void changedActionBegin() { _avoidRecursion = true; }
    void changedActionEnd() { _avoidRecursion = false; }
    bool changedActionInProgress() const { return _avoidRecursion; }

    // get the param instance
    OfxhParamSet* getParamSetInstance() { return _paramSetInstance; }
    void setParamSetInstance(OfxhParamSet* instance) { _paramSetInstance = instance; }

    // set/get parent instance
    void setParentInstance(OfxhParam* instance);
    OfxhParam* getParentInstance();

    // copy one parameter to another
    virtual void copy(const OfxhParam& instance) OFX_EXCEPTION_SPEC = 0;

    // copy one parameter to another
    virtual void copy(const OfxhParam& instance, OfxTime offset) OFX_EXCEPTION_SPEC;

    // copy one parameter to another, with a range
    virtual void copy(const OfxhParam& instance, OfxTime offset, OfxRangeD range) OFX_EXCEPTION_SPEC;

    // callback which should set enabled state as appropriate
    virtual void setEnabled();

    // callback which should set secret state as appropriate
    virtual void setSecret();

    /// callback which should update label
    virtual void setLabel();

    /// callback which should set
    virtual void setDisplayRange();

// va list calls below turn the var args (oh what a mistake)
// suite functions into virtual function calls on instances
// they are not to be overridden by host implementors by
// by the various typeed param instances so that they can
// deconstruct the var args lists

#endif
    inline virtual std::size_t getSize() const { return 1; }

    TUTTLE_DEFINE_OFXHPARAM_ACCESSORS(String, std::string);
    TUTTLE_DEFINE_OFXHPARAM_ACCESSORS(Double, double);
    TUTTLE_DEFINE_OFXHPARAM_ACCESSORS(Int, int);
    TUTTLE_DEFINE_OFXHPARAM_ACCESSORS(Bool, bool);

#ifndef SWIG
    TUTTLE_DEFINE_OFXHPARAM_MULTIDIM_ACCESSORS(String, std::string);
    TUTTLE_DEFINE_OFXHPARAM_MULTIDIM_ACCESSORS(Double, double);
    TUTTLE_DEFINE_OFXHPARAM_MULTIDIM_ACCESSORS(Int, int);
    TUTTLE_DEFINE_OFXHPARAM_MULTIDIM_ACCESSORS(Bool, bool);
#endif

    virtual void setValueFromExpression(const std::string& value, const EChange change) OFX_EXCEPTION_SPEC
    {
        BOOST_THROW_EXCEPTION(OfxhException(kOfxStatErrMissingHostFeature,
                                            "\"" + this->getName() + "Can't set value from expression on parameter \"" +
                                                this->getName() + "\", it's not supported for " + this->getParamType() +
                                                " parameters (" + this->getParamType() + ", " +
                                                mapChangeEnumToString(change) + ")."));
    }
    inline void setValueFromExpression(const std::string& value) OFX_EXCEPTION_SPEC
    {
        setValueFromExpression(value, eChangeUserEdited);
    }

    inline void setValue(const char* value, const EChange change) OFX_EXCEPTION_SPEC
    {
        setValue(std::string(value), change);
    }
    inline void setValue(const char* value) OFX_EXCEPTION_SPEC { setValue(value, eChangeUserEdited); }
    inline void setValueAtTime(const OfxTime time, const char* value, const attribute::EChange change) OFX_EXCEPTION_SPEC
    {
        setValueAtTime(time, std::string(value), change);
    }
    inline void setValueAtTime(const OfxTime time, const char* value) OFX_EXCEPTION_SPEC
    {
        setValueAtTime(time, value, eChangeUserEdited);
    }

    /* TuttleOFX specific. This is not part of OFX.

       It must be in the OfxhParam base class in order to be exposed by the API. */
    virtual void setInterpolator(const enum EInterpolatorType etype) OFX_EXCEPTION_SPEC
    {
        BOOST_THROW_EXCEPTION(ofx::OfxhException(kOfxStatErrMissingHostFeature));
    }

#ifndef SWIG
    /// get a value, implemented by instances to deconstruct var args
    virtual void getV(va_list arg) const OFX_EXCEPTION_SPEC;

    /// get a value, implemented by instances to deconstruct var args
    virtual void getV(const OfxTime time, va_list arg) const OFX_EXCEPTION_SPEC;

    /// set a value, implemented by instances to deconstruct var args
    virtual void setV(va_list arg, const EChange change) OFX_EXCEPTION_SPEC;

    /// key a value, implemented by instances to deconstruct var args
    virtual void setV(const OfxTime time, va_list arg, const EChange change) OFX_EXCEPTION_SPEC;

    /// derive a value, implemented by instances to deconstruct var args
    virtual void deriveV(const OfxTime time, va_list arg) const OFX_EXCEPTION_SPEC;

    /// integrate a value, implemented by instances to deconstruct var args
    virtual void integrateV(const OfxTime time1, const OfxTime time2, va_list arg) const OFX_EXCEPTION_SPEC;

    /// overridden from Property::NotifyHook
    virtual void notify(const std::string& name, bool single, int num) OFX_EXCEPTION_SPEC;
#endif
};

#ifndef SWIG
/**
 * @brief to make ParamInstance clonable (for use in boost::ptr_container)
 */
inline OfxhParam* new_clone(const OfxhParam& a)
{
    return a.clone();
}

#endif
}
}
}
}

#ifndef SWIG
// force boost::is_virtual_base_of value (used by boost::serialization)
namespace boost
{
template <>
struct is_virtual_base_of<tuttle::host::ofx::attribute::OfxhAttribute, tuttle::host::ofx::attribute::OfxhParam>
    : public mpl::true_
{
};
}
#endif

#endif
