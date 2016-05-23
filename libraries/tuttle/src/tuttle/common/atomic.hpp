#ifndef _TUTTLE_ATOMIC_HPP_
#define _TUTTLE_ATOMIC_HPP_

#ifndef WITHOUT_BOOST_ATOMIC

#include <boost/atomic/atomic.hpp>

#else
// For compatibility with boost versions without boost.atomic (added in 1.53).
// Use mutex instead of atomic.

#define BOOST_ATOMIC_HPP
#include <boost/thread/mutex.hpp>
#include <boost/memory_order.hpp>

namespace boost
{

// Fake atomic class using mutex to work without boost.atomic.
template <typename T>
class atomic
{
public:
    typedef T value_type;
    atomic(const T v)
        : _value(v)
    {
    }

    void store(const T v, const memory_order unused)
    {
        boost::mutex::scoped_lock locker(_mutex);
        _value = v;
    }

    value_type load(const memory_order unused) const
    {
        boost::mutex::scoped_lock locker(_mutex);
        return _value;
    }

private:
    T _value;
    mutable boost::mutex _mutex;
};

typedef atomic<bool> atomic_bool;
}

#endif

#endif
