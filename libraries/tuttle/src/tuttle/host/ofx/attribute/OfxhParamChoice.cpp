#include "OfxhParamChoice.hpp"

#include <boost/numeric/conversion/cast.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/functional/hash.hpp>

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace attribute
{

int OfxhParamChoice::getIndexFor(const std::string& key) const
{
    typedef std::vector<std::string> StringVector;
    const StringVector& values = getChoiceKeys();

    if(key.empty())
    {
        BOOST_THROW_EXCEPTION(OfxhException("Empty key value for choice param \"" + this->getName() + "\".\n" +
                                            "Possible values are : [\n\"" + boost::algorithm::join(values, "\",\n\"") +
                                            "\" ]"));
    }
    if(values.empty())
    {
        BOOST_THROW_EXCEPTION(OfxhException("The key \"" + key + "\" doesn't exist for choice param \"" + this->getName() +
                                            "\".\n" + "The choice parameter is empty."));
    }

    StringVector partialMatches;
    StringVector::const_iterator strictMatch = values.end();
    StringVector::const_iterator partialMatch = values.end();
    for(StringVector::const_iterator it = values.begin(), itEnd = values.end(); it != itEnd; ++it)
    {
        if(it->size() == key.size()) // exact match
        {
            if(it->compare(key) == 0)
            {
                strictMatch = it;
                break; // don't need to keep searching
            }
        }
        else if(it->size() > key.size()) // partial key, the user only set the beginning of the choice value
        {
            if(it->compare(0, key.size(), key) == 0)
            {
                partialMatch = it;
                partialMatches.push_back(*it);
            }
        }
    }
    if(strictMatch != values.end())
    {
        return boost::numeric_cast<int>(std::distance(values.begin(), strictMatch));
    }
    if(partialMatch != values.end())
    {
        if(partialMatches.size() > 1)
        {
            BOOST_THROW_EXCEPTION(OfxhException("The key \"" + key + "\" has multiple matches for choice param \"" +
                                                this->getName() + "\".\n" + "Possible matches are : [\n\"" +
                                                boost::algorithm::join(partialMatches, "\",\n\"") + "\" ]"));
        }
        return boost::numeric_cast<int>(std::distance(values.begin(), partialMatch));
    }
    BOOST_THROW_EXCEPTION(OfxhException("The key \"" + key + "\" doesn't exist for choice param \"" + this->getName() +
                                        "\".\n" + "Possible values are : [\n\"" + boost::algorithm::join(values, "\",\n\"") +
                                        "\" ]"));
}

const std::string& OfxhParamChoice::getChoiceKeyAt(const int index) const
{
    const std::vector<std::string>& keys = getChoiceKeys();
    if(index < 0 || index >= keys.size())
    {
        BOOST_THROW_EXCEPTION(OfxhException(kOfxStatErrBadIndex) << exception::user() + "The index value \"" + index +
                                                                        "\" is out of range.\n" + "Choice keys are : [\n\"" +
                                                                        boost::algorithm::join(keys, "\",\n\"") + "\" ]");
    }
    return keys[index];
}

void OfxhParamChoice::getValue(std::string& outKey) const OFX_EXCEPTION_SPEC
{
    int index = 0;
    getValue(index);
    outKey = getChoiceKeyAt(index);
}

void OfxhParamChoice::getValueAtTime(const OfxTime time, std::string& outKey) const OFX_EXCEPTION_SPEC
{
    int index = 0;
    getValueAtTime(time, index);
    outKey = getChoiceKeyAt(index);
}

/**
 * implementation of var args function
 */
void OfxhParamChoice::getV(va_list arg) const OFX_EXCEPTION_SPEC
{
    int* value = va_arg(arg, int*);

    return getValue(*value);
}

/**
 * implementation of var args function
 */
void OfxhParamChoice::getV(const OfxTime time, va_list arg) const OFX_EXCEPTION_SPEC
{
    int* value = va_arg(arg, int*);

    return getValueAtTime(time, *value);
}

/**
 * implementation of var args function
 */
void OfxhParamChoice::setV(va_list arg, const EChange change) OFX_EXCEPTION_SPEC
{
    int value = va_arg(arg, int);

    return setValue(value, change);
}

/**
 * implementation of var args function
 */
void OfxhParamChoice::setV(const OfxTime time, va_list arg, const EChange change) OFX_EXCEPTION_SPEC
{
    int value = va_arg(arg, int);

    return setValueAtTime(time, value, change);
}

std::size_t OfxhParamChoice::getHashAtTime(const OfxTime time) const
{
    std::string
        value; // don't use the index here. The plugin could change the order of choices values in a minor version change.
    getValueAtTime(time, value);
    return boost::hash_value(value);
}
}
}
}
}
