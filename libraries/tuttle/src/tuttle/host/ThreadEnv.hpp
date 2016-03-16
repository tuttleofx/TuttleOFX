#ifndef _TUTTLE_HOST_CORE_THREADENV_HPP_
#define _TUTTLE_HOST_CORE_THREADENV_HPP_

#include <tuttle/host/NodeListArg.hpp>
#include <tuttle/host/ComputeOptions.hpp>
#include <tuttle/host/memory/MemoryCache.hpp>

#include <boost/thread/thread.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/tss.hpp>
#include <tuttle/common/atomic.hpp>
#include <boost/bind.hpp>

#include <boost/signals2.hpp>

namespace tuttle
{
namespace host
{

class Graph;

/**
 * Utility class to compute a graph inside a thread.
 */
class ThreadEnv
{
public:
    typedef ThreadEnv This;
    typedef boost::signals2::signal<void()> SignalType;

    ThreadEnv(const bool asynchronous = true)
        : _asynchronous(asynchronous)
        , _isRunning(false)
        , _result(false)
    {
        // By default the Thread doesn't return a buffer.
        _options.setReturnBuffers(false);
    }

    inline memory::MemoryCache& getImageCache() { return _imageCache; }
    inline const memory::MemoryCache& getImageCache() const { return _imageCache; }

    inline ComputeOptions& getComputeOptions() { return _options; }
    inline const ComputeOptions& getComputeOptions() const { return _options; }

    inline This& setAsynchronous(const bool v = true)
    {
        _asynchronous = v;
        return *this;
    }

    /// @brief Main functions
    /// @{

    /**
     * @brief Is this ThreadEnv executing a compute?
     * Thread Safe
     */
    inline bool isRunning() { return _isRunning.load(boost::memory_order_relaxed); }

    /**
     * @brief Launch the graph computation in a synchrone or asynchrone way.
     */
    void compute(Graph& graph, const NodeListArg& nodes = NodeListArg());

    /**
     * @brief The application would like to abort the process (from another thread).
     * Thread Safe
     */
    inline void abort() { _options.abort(); }

    inline void join() { _thread.join(); }

    /**
     * @brief Result status of the lastest compute.
     * Thread Safe
     */
    inline bool getResult() const { return _result.load(boost::memory_order_relaxed); }

    inline SignalType& getSignalEnd() { return _signalEnd; }

    /// @}

private:
    static void runProcessFunc(ThreadEnv* threadEnv, Graph& graph, const std::list<std::string>& nodes);

    /**
     * @brief Result status of the lastest compute.
     * Thread Safe
     */
    void setResult(const bool res) { _result.store(res, boost::memory_order_relaxed); }

    /**
     * @brief Is this ThreadEnv executing a compute?
     * Thread Safe
     */
    void setIsRunning(const bool res) { _isRunning.store(res, boost::memory_order_relaxed); }

private:
    boost::thread _thread;

    bool _asynchronous;
    memory::MemoryCache _imageCache;
    ComputeOptions _options;

    boost::atomic_bool _isRunning;
    boost::atomic_bool _result;

    SignalType _signalEnd;
};
}
}

#endif
