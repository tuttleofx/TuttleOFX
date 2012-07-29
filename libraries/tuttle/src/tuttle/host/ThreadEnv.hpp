#ifndef _TUTTLE_HOST_CORE_THREADENV_HPP_
#define _TUTTLE_HOST_CORE_THREADENV_HPP_

#include <tuttle/host/NodeListArg.hpp>
#include <tuttle/host/ComputeOptions.hpp>
#include <tuttle/host/memory/MemoryCache.hpp>

#include <boost/thread/thread.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/tss.hpp>
#include <boost/atomic.hpp>
#include <boost/bind.hpp>


namespace tuttle {
namespace host {

class Graph;

class ThreadEnv
{
public:
	typedef ThreadEnv This;
	
	ThreadEnv( const bool asynchronous = true )
	: _asynchronous( asynchronous )
	, _result(false)
	{}
	
	ComputeOptions& getComputeOptions() { return _options; }
	const ComputeOptions& getComputeOptions() const { return _options; }
	
	This& setAsynchronous( const bool v = true )
	{
		_asynchronous = v;
		return *this;
	}
	
	/// @brief Main functions
	/// @{
	
	/**
	 * @brief Launch the graph computation in a synchrone or asynchrone way.
	 */
	void compute( Graph& graph, const NodeListArg& nodes = NodeListArg() );
	
	/**
	 * @brief The application would like to abort the process (from another thread).
	 */
	void abort() { _options.abort(); }
	
	void join() { _thread.join(); }
	
	bool getResult() const { return _result.load( boost::memory_order_relaxed ); }
	
	/// @}
	
private:
	static void runProcessFunc( ThreadEnv* threadEnv, Graph* graph, const std::list<std::string>& nodes, const ComputeOptions* const options );
	
	void setResult( const bool res ) { _result.store( res, boost::memory_order_relaxed ); }
	
private:
	boost::thread _thread;
	bool _asynchronous;
	memory::MemoryCache _memoryCache;
	ComputeOptions _options;
	
	boost::atomic_bool _result;
};

}
}

#endif
