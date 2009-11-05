#ifndef VALUE_INTERPOLATOR_HPP
#define VALUE_INTERPOLATOR_HPP

#include <vector>

#include <ofxCore.h>

template<typename T>
struct TimeValue {
    OfxTime time;
    T value;
};

template<typename T>
class Interpolator {
    typedef TimeValue<T> TimeValueT;
    public:
        Interpolator() { };
        virtual ~Interpolator() { }
        OfxStatus getValue(const TimeValueT & prev, const TimeValueT & next, TimeValueT & unknownValue ) = 0;
};


template<typename T>
class LinearInterpolator : public Interpolator<T> {
    typedef TimeValue<T> TimeValueT;
    public:
        LinearInterpolator();
        virtual ~LinearInterpolator();
        OfxStatus getValue(const TimeValueT & prev, const TimeValueT & next, TimeValueT & unknownValue ) {
            return kOfxStatOK;
        }
};

template<class I>
class ValueInterpolator {
    typedef TimeValue<I> TimeValueT;
    protected:
        std::vector<TimeValueT> _timeValues;
        I _interpolator;
    public:
        ValueInterpolator(const std::vector<TimeValueT> & timeValues) {
            _timeValues = timeValues;
        }
        ValueInterpolator() {
        }
        virtual ~ValueInterpolator() { };
};

#endif //VALUE_INTERPOLATOR_HPP
