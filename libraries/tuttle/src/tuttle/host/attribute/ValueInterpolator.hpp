#ifndef VALUE_INTERPOLATOR_HPP
#define VALUE_INTERPOLATOR_HPP

/* Helper classes for animated parameters */

#include <ofxCore.h>

namespace tuttle
{
namespace host
{
namespace attribute
{

/* A "key frame" for an animated param */
template <typename T>
class TimeValue
{
public:
    OfxTime time;
    T value;

    bool operator<(const TimeValue<T>& t) const { return time < t.time; }

    bool operator==(const TimeValue<T>& t) const { return time == t.time; }
};

/* An Interpolator determines the shape of the curve between key frames in an animation */
template <typename T>
class Interpolator
{
    typedef Interpolator<T> This;
    typedef TimeValue<T> TimeValueT;

public:
    virtual ~Interpolator() = 0;

    virtual This* clone() const = 0;

    /* Given two key frames and a time between them, get the appropriate value for that time */
    virtual OfxStatus getValue(const TimeValueT& prev, const TimeValueT& next, const OfxTime t, T& v) = 0;
};

template <typename T>
Interpolator<T>::~Interpolator()
{
}

/* Reduces interpolation to a value between 0 and 1 */

/* Extend with a custom interpolate() function to provide different behaviors.*/
template <typename T>
class GenericInterpolator : public Interpolator<T>
{
    typedef TimeValue<T> TimeValueT;

public:
    virtual ~GenericInterpolator() = 0;

    OfxStatus getValue(const TimeValueT& prev, const TimeValueT& next, const OfxTime t, T& v)
    {

        /* If the time is before the first key frame or both key frames are the same,
           use the first key frame's value */
        if(t <= prev.time || prev.time == next.time)
        {
            v = prev.value;

            /* If the time is after the last keyframe, use the last keyframe's value */
        }
        else if(t >= next.time)
        {
            v = next.value;

            /* If the time is between the two values, interpolate */
        }
        else
        {
            v = (T)(prev.value +
                    (next.value - prev.value) * interpolate((double)(t - prev.time) / (double)(next.time - prev.time)));
        }
        return kOfxStatOK;
    }

protected:
    /* given a position on the timeline between 0.0 and 1.0 return the
       interpolated value. */
    virtual double interpolate(const double pos) const = 0;
};

template <typename T>
GenericInterpolator<T>::~GenericInterpolator()
{
}

/* No smoothing */
template <typename T>
class LinearInterpolator : public GenericInterpolator<T>
{
public:
    typedef LinearInterpolator<T> This;
    This* clone() const { return new This(*this); };

protected:
    double interpolate(const double pos) const { return pos; }
};

/* Ease in and ease out */
template <typename T>
class SmoothInterpolator : public GenericInterpolator<T>
{
public:
    typedef SmoothInterpolator<T> This;
    This* clone() const { return new This(*this); };

protected:
    double interpolate(const double pos) const { return -2 * pos * pos * pos + 3 * pos * pos; }
};

/* Ease out */
template <typename T>
class FastInterpolator : public GenericInterpolator<T>
{
public:
    typedef FastInterpolator<T> This;
    This* clone() const { return new This(*this); };

protected:
    double interpolate(const double pos) const { return -1 * pos * pos + 2 * pos; }
};

/* Ease in */
template <typename T>
class SlowInterpolator : public GenericInterpolator<T>
{
public:
    typedef SlowInterpolator<T> This;
    This* clone() const { return new This(*this); };

protected:
    double interpolate(const double pos) const { return pos * pos; }
};
}
}
}

#endif
