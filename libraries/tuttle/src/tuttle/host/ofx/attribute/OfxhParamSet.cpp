#include "OfxhParamSet.hpp"

#include <boost/functional/hash.hpp>

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/foreach.hpp>

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace attribute
{

OfxhParamSet::OfxhParamSet()
{
}

OfxhParamSet::OfxhParamSet(const OfxhParamSet& other)
{
    operator=(other);
}

void OfxhParamSet::initMapFromList()
{
    BOOST_FOREACH(OfxhParam& p, _paramVector)
    {
        _params[p.getName()] = &p;
        _paramsByScriptName[p.getScriptName()] = &p;
    }
}

OfxhParamSet::~OfxhParamSet()
{
}

OfxhParamSet& OfxhParamSet::operator=(const OfxhParamSet& other)
{
    _paramVector = other._paramVector.clone();
    initMapFromList();
    return *this;
}

void OfxhParamSet::copyParamsValues(const OfxhParamSet& other)
{
    if(_paramVector.size() != other._paramVector.size())
    {
        BOOST_THROW_EXCEPTION(exception::Bug()
                              << exception::dev("You try to copy parameters values, but the two lists are not identical."));
    }

    ParamVector::const_iterator oit = other._paramVector.begin(), oitEnd = other._paramVector.end();
    for(ParamVector::iterator it = _paramVector.begin(), itEnd = _paramVector.end(); it != itEnd && oit != oitEnd;
        ++it, ++oit)
    {
        OfxhParam& p = *it;
        const OfxhParam& op = *oit;
        if(p.getName() != op.getName())
        {
            BOOST_THROW_EXCEPTION(
                exception::Bug() << exception::dev(
                    "You try to copy parameters values, but it is not the same parameters in the two lists."));
        }
        p.copy(op);
    }
    initMapFromList();
}

std::size_t OfxhParamSet::getHashAtTime(const OfxTime time) const
{
    std::size_t seed = 0;
    BOOST_FOREACH(const OfxhParam& param, getParamVector())
    {
        // TUTTLE_LOG_VAR( TUTTLE_INFO, param.getName() );
        if(param.paramTypeHasData() && param.getEvaluateOnChange())
        {
            boost::hash_combine(seed, param.getHashAtTime(time));
        }
    }
    return seed;
}

// void OfxhParamSet::referenceParam( const std::string& name, OfxhParam* instance ) OFX_EXCEPTION_SPEC
//{
//	if( _allParams.find( name ) != _allParams.end() )
//	{
//		BOOST_THROW_EXCEPTION( OfxhException( kOfxStatErrExists, "Trying to reference a new parameter which already
// exists." ) );
//	}
//	_allParams[name] = instance;
//}

OfxhParam& OfxhParamSet::getParam(const std::string& name)
{
    ParamMap::iterator it = _params.find(name);
    if(it == _params.end())
    {
        BOOST_THROW_EXCEPTION(exception::BadIndex() << exception::user() + "Param name \"" + name + "\" not found.");
    }
    return *it->second;
}

OfxhParam& OfxhParamSet::getParamByScriptName(const std::string& scriptName, const bool acceptPartialName)
{
    ParamMap::iterator it = _paramsByScriptName.find(scriptName);
    if(it != _paramsByScriptName.end())
        return *it->second;

    std::vector<std::string> matches;
    OfxhParam* res = NULL;
    if(acceptPartialName)
    {
        BOOST_FOREACH(ParamMap::value_type& p, _paramsByScriptName)
        {
            if(boost::algorithm::starts_with(p.first, scriptName))
            {
                matches.push_back(p.first);
                res = p.second;
            }
        }
        if(matches.size() > 1)
        {
            BOOST_THROW_EXCEPTION(exception::Value() << exception::user() + "Ambiguous partial param script name \"" +
                                                            scriptName + "\". Possible values are: " +
                                                            boost::algorithm::join(matches, ", ") + ".");
        }
    }

    if(matches.size() == 0)
    {
        BOOST_THROW_EXCEPTION(exception::Value()
                              << exception::user() + "Param script name \"" + scriptName + "\" not found.");
    }
    return *res;
}
OfxhParam* OfxhParamSet::getParamPtrByScriptName(const std::string& name, const bool acceptPartialName)
{
    try
    {
        return &this->getParamByScriptName(name, acceptPartialName);
    }
    catch(...)
    {
        return NULL;
    }
}
const OfxhParam* OfxhParamSet::getParamPtrByScriptName(const std::string& name, const bool acceptPartialName) const
{
    try
    {
        return &this->getParamByScriptName(name, acceptPartialName);
    }
    catch(...)
    {
        return NULL;
    }
}

// get the param
OfxhParam& OfxhParamSet::getParam(const std::size_t index)
{
    if(index > _paramVector.size())
        BOOST_THROW_EXCEPTION(exception::BadIndex() << exception::user() + "Param not found, index out of range. (index=" +
                                                           index + ", nb params=" + _paramVector.size() + ")");

    return _paramVector[index];
}

// get the param
OfxhParam& OfxhParamSet::getChildParam(const std::size_t index)
{
    if(index > _childParamVector.size())
        BOOST_THROW_EXCEPTION(exception::BadIndex() << exception::user() + "Param not found, index out of range. (index=" +
                                                           index + ", nb params=" + _paramVector.size() + ")");

    return *_childParamVector[index];
}

void OfxhParamSet::addParam(OfxhParam* instance) OFX_EXCEPTION_SPEC
{
    if(_params.find(instance->getName()) != _params.end())
    {
        BOOST_THROW_EXCEPTION(OfxhException(kOfxStatErrExists, "Trying to add a new parameter which already exists."));
    }
    _paramVector.push_back(instance);
    _params[instance->getName()] = instance;
    _paramsByScriptName[instance->getScriptName()] = instance;
    //	referenceParam( name, instance );
}

void OfxhParamSet::declareChildParam(OfxhParam& childParam)
{
    childParam.setParamSetInstance(this);
    _childParamVector.push_back(&childParam);
    _childParams[childParam.getName()] = &childParam;
}
}
}
}
}
