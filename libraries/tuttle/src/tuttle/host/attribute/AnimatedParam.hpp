#ifndef _TUTTLE_HOST_CORE_ATTRIBUTE_ANIMATED_PARAM_HPP_
#define _TUTTLE_HOST_CORE_ATTRIBUTE_ANIMATED_PARAM_HPP_

/* Template class for an animated parameter.

Used for ParamDouble and ParamInteger */

#include "Param.hpp"
#include "expression.hpp"

#include <tuttle/host/INode.hpp>
#include <tuttle/host/ofx/attribute/OfxhParamDouble.hpp>
#include <tuttle/host/ofx/attribute/OfxhParamInteger.hpp>
#include <tuttle/host/attribute/ValueInterpolator.hpp>

#include <boost/scoped_ptr.hpp>
#include <boost/functional/hash.hpp>

#include <vector>
#include <algorithm>

namespace tuttle
{
namespace host
{
namespace attribute
{

template <typename T, typename OFX_PARAM>
class AnimatedParam : public Param, public OFX_PARAM
{
protected:
    T _value;

    /* An ordered list of key frames. Used when this param is
       animated.  This must stay sorted */
    std::vector<TimeValue<T> > _key_frames;

    /* The class that interpolates key frames when animating this param
       All transitions yare animated with the same interpolator. A more
       advanced version of this class might allow any of the interpolators
       in ValueInterpolator.hpp to be used between any adjacent key frames. */
    boost::scoped_ptr<Interpolator<T> > _interpolator;

public:
    typedef AnimatedParam<T, OFX_PARAM> This;
    typedef typename std::vector<TimeValue<T> >::iterator TimeValueTIterator;
    typedef typename std::vector<TimeValue<T> >::const_iterator TimeValueTConstIterator;

    AnimatedParam(INode& effect, const std::string& name, const ofx::attribute::OfxhParamDescriptor& descriptor,
                  const std::size_t index, const T value)
        : Param(effect)
        , OFX_PARAM(descriptor, name, effect.getParamSet(), index)
        , _value(value)
        , _interpolator(new LinearInterpolator<T>())
    {
    }

    AnimatedParam(const AnimatedParam<T, OFX_PARAM>& other)
        : Param(other)
        , OFX_PARAM(other)
        , _value(other._value)
        , _interpolator(other._interpolator->clone())
    {
    }

    ~AnimatedParam() {}

    void setInterpolator(const ofx::attribute::EInterpolatorType interpolatorType) OFX_EXCEPTION_SPEC
    {
        switch(interpolatorType)
        {
            case ofx::attribute::eSmoothInterpolator:
                _interpolator.reset(new SmoothInterpolator<T>());
                break;
            case ofx::attribute::eFastInterpolator:
                _interpolator.reset(new FastInterpolator<T>());
                break;
            case ofx::attribute::eSlowInterpolator:
                _interpolator.reset(new SlowInterpolator<T>());
                break;
            case ofx::attribute::eLinearInterpolator:
                _interpolator.reset(new LinearInterpolator<T>());
                break;
        }
    }

    void getValue(T& v) const OFX_EXCEPTION_SPEC { v = _value; }

    void getValueAtTime(const OfxTime time, T& v) const OFX_EXCEPTION_SPEC
    {
        if(_key_frames.size() == 0)
            v = _value;
        else
        {

            /* Find the key frames surrounding this time

               Set prev to the first key frame before "time" or the first one
               after "time" if there's not one before.

               Set next to the first key frame after "time" or the first one
               before "time" if there's not one after */

            TimeValue<T> temp, prev, next;
            TimeValueTConstIterator it;

            // Find the first item at or before time
            temp.time = time;
            it = std::lower_bound(_key_frames.begin(), _key_frames.end(), temp);

            if(it->time == time || it == _key_frames.begin())
            {
                // There's a key frame at exactly this time
                // or this is the first key frame. Return this key frame value
                v = it->value;
                return;
            }
            else if(it == _key_frames.end())
            {
                // There's no key frame at or after this time
                // Return the last key frame value
                it--;
                v = it->value;
                return;
            }
            else
            {
                // The time is between two values.
                // Get the previous key frame and interpolate
                next = *it;
                it--;
                prev = *it;
                _interpolator->getValue(prev, next, (const OfxTime)time, v);
            }
        }
    }

    void setValue(const T& v, const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
    {
        /* Setting a single value for this param clears the animation */
        _key_frames.clear();
        _value = v;
        this->paramChanged(change);
    }

    void setValueAtTime(const OfxTime time, const T& v, const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
    {
        TimeValue<T> new_tv;
        TimeValueTIterator it;

        new_tv.time = time;
        new_tv.value = v;
        it = find(_key_frames.begin(), _key_frames.end(), new_tv);
        if(it == _key_frames.end())
        {
            _key_frames.push_back(new_tv);
            std::sort(_key_frames.begin(), _key_frames.end());
        }
        else
        {
            (*it).value = v;
        }
        this->paramChanged(change);
    }

    void setValueFromExpression(const std::string& value, const ofx::attribute::EChange change) OFX_EXCEPTION_SPEC
    {
        _value = extractValueFromExpression<T>(value);
        this->paramChanged(change);
    }

    void derive(const OfxTime time, T&) const OFX_EXCEPTION_SPEC
    {
        BOOST_THROW_EXCEPTION(ofx::OfxhException(kOfxStatErrMissingHostFeature));
    }

    void integrate(const OfxTime time1, const OfxTime time2, T&) const OFX_EXCEPTION_SPEC
    {
        BOOST_THROW_EXCEPTION(ofx::OfxhException(kOfxStatErrMissingHostFeature));
    }

    void copy(const AnimatedParam<T, OFX_PARAM>& p) OFX_EXCEPTION_SPEC
    {
        _value = p._value;
        _key_frames = p._key_frames;
        //	paramChanged( ofx::attribute::eChangeUserEdited );
    }

    void copy(const ofx::attribute::OfxhParam& p) OFX_EXCEPTION_SPEC
    {
        const AnimatedParam<T, OFX_PARAM>& param = dynamic_cast<const AnimatedParam<T, OFX_PARAM>&>(p);

        copy(param);
    }

    This* clone() const { return new This(*this); }

    /* ======= BEGIN OfxhKeyframeParam functions =======

    Since this implementation is backed by a set, indexes may change and
    they are no longer valid after a key frame is set or deleted.  */
    void getNumKeys(unsigned int& outNumKeys) const OFX_EXCEPTION_SPEC { outNumKeys = _key_frames.size(); }

    void getKeyTime(const int nth, OfxTime& outTime) const OFX_EXCEPTION_SPEC
    {
        if(nth >= 0 && (size_t)nth < _key_frames.size())
            outTime = _key_frames[nth].time;
        else
            BOOST_THROW_EXCEPTION(ofx::OfxhException(kOfxStatErrBadIndex));
    }

    void getKeyIndex(const OfxTime time, const int direction, int& outIndex) const OFX_EXCEPTION_SPEC
    {
        TimeValue<T> temp;
        TimeValueTConstIterator it;

        // Find the first item at or before time
        temp.time = time;
        it = std::lower_bound(_key_frames.begin(), _key_frames.end(), temp);

        if(it == _key_frames.end())
            BOOST_THROW_EXCEPTION(ofx::OfxhException(kOfxStatFailed));

        if(direction == 0)
        {
            // Find an exact match or fail
            if(it->time == time)
            {
                outIndex = it - _key_frames.begin();
            }
            else
            {
                BOOST_THROW_EXCEPTION(ofx::OfxhException(kOfxStatFailed));
            }
        }
        else if(direction < 0)
        {
            // Find the key frame directly before the match. If the match is the first element, fail.
            if(it == _key_frames.begin())
                BOOST_THROW_EXCEPTION(ofx::OfxhException(kOfxStatFailed));
            else
            {
                outIndex = it - _key_frames.begin() - 1;
            }
        }
        else
        { // direction > 0
            if(it->time > time)
            {
                // The match is greater than time. Success.
                outIndex = it - _key_frames.begin();
            }
            else
            { // (it->time == time)
                // The match is exact. If there's a next element, use that.
                it++;
                if(it != _key_frames.end())
                    outIndex = it - _key_frames.begin();
                else
                    BOOST_THROW_EXCEPTION(ofx::OfxhException(kOfxStatFailed));
            }
        }
    }

    void deleteKey(const OfxTime time) OFX_EXCEPTION_SPEC
    {
        TimeValue<T> temp;
        TimeValueTIterator it;

        temp.time = time;
        it = find(_key_frames.begin(), _key_frames.end(), temp);
        if(it != _key_frames.end())
            _key_frames.erase(it);
        else
            BOOST_THROW_EXCEPTION(ofx::OfxhException(kOfxStatErrBadIndex));
    }

    void deleteAllKeys() OFX_EXCEPTION_SPEC { _key_frames.clear(); }

    /* ======= END OfxhKeyframeParam functions ======= */

    bool paramTypeHasData() const { return true; }

    std::size_t getHash() const
    {
        std::size_t seed = 0;
        BOOST_FOREACH(const TimeValue<T>& tv, _key_frames)
        {
            boost::hash_combine(seed, tv.time);
            boost::hash_combine(seed, tv.value);
        }
        return seed;
    }

    std::size_t getHashAtTime_valueChangeToEnd(const OfxTime time) const
    {
        T valueAtTime = 0;
        getValueAtTime(time, valueAtTime);
        std::size_t seed = 0;
        BOOST_FOREACH(const TimeValue<T>& tv, _key_frames)
        {
            if(tv.time >= time)
            {
                boost::hash_combine(seed, tv.time);
                boost::hash_combine(seed, tv.value);
            }
        }
        return seed;
    }

    std::size_t getHashAtTime(const OfxTime time) const
    {
        const std::string cacheInvalidation = getCacheInvalidation();
        if(cacheInvalidation == kOfxParamInvalidateAll)
        {
            std::size_t seed = getHash();
            boost::hash_combine(seed, time);
            return seed;
        }
        else if(cacheInvalidation == kOfxParamInvalidateValueChangeToEnd)
        {
            return getHashAtTime_valueChangeToEnd(time);
        }
        else
        {
            BOOST_ASSERT(cacheInvalidation == kOfxParamInvalidateValueChange);

            // Standard case
            T valueAtTime = 0;
            getValueAtTime(time, valueAtTime);
            return boost::hash_value(valueAtTime);
        }
    }
};
}
}
}

#endif
