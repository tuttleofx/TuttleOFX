#ifndef _TUTTLE_HOST_CORE_COMPUTEOPTIONS_HPP_
#define _TUTTLE_HOST_CORE_COMPUTEOPTIONS_HPP_

#include <ofxCore.h>

#include <boost/atomic.hpp>

#include <list>

namespace tuttle {
namespace host {

struct TimeRange
{
	TimeRange()
		: _begin( 0 )
		, _end( 0 )
		, _step( 1 )
	{}
	TimeRange( const int frame )
		: _begin( frame )
		, _end( frame )
		, _step( 1 )
	{}
	TimeRange( const int begin, const int end, const int step = 1 )
		: _begin( begin )
		, _end( end )
		, _step( step )
	{}
	TimeRange( const OfxRangeD& range, const int step = 1 );
	
	int _begin;
	int _end;
	int _step;
};

enum EVerboseLevel
{
	eVerboseLevelNone,
	eVerboseLevelError,
	eVerboseLevelWarning,
	eVerboseLevelDebug
};

class ComputeOptions
{
public:
	typedef ComputeOptions This;
	
	ComputeOptions()
	: _abort( false )
	{
		init();
	}
	explicit
	ComputeOptions( const int frame )
	: _abort( false )
	{
		init();
		_timeRanges.push_back( TimeRange( frame, frame ) );
	}
	ComputeOptions( const int begin, const int end, const int step = 1 )
	: _abort( false )
	{
		init();
		_timeRanges.push_back( TimeRange( begin, end, step ) );
	}
	ComputeOptions( const ComputeOptions& options )
	: _abort( false )
	{
		*this = options;
	}
	ComputeOptions& operator=( const ComputeOptions& other )
	{
		_timeRanges = other._timeRanges;

		_renderScale = other._renderScale;
		_continueOnError = other._continueOnError;
		_forceIdentityNodesProcess = other._forceIdentityNodesProcess;
		_returnBuffers = other._returnBuffers;
		_verboseLevel = other._verboseLevel;
		_isInteractive = other._isInteractive;

		// don't modify the abort status?
		//_abort.store( false, boost::memory_order_relaxed );
		
		return *this;
	}
	
private:
	void init()
	{
		setRenderScale( 1.0, 1.0 );
		setContinueOnError( false );
		setReturnBuffers( true );
		setVerboseLevel( eVerboseLevelError );
		setIsInteractive( false );
		setForceIdentityNodesProcess( false );
	}
	
public:
	const std::list<TimeRange>& getTimeRanges() const { return _timeRanges; }
	
	This& setTimeRange( const int begin, const int end, const int step = 1 )
	{
		_timeRanges.clear();
		addTimeRange( begin, end, step );
		return *this;
	}
	This& setTimeRange( const TimeRange& timeRange )
	{
		_timeRanges.clear();
		_timeRanges.push_back( timeRange );
		return *this;
	}
	This& addTimeRange( const int begin, const int end, const int step = 1 )
	{
		addTimeRange( TimeRange(begin, end, step) );
		return *this;
	}
	This& addTimeRange( const TimeRange& timeRange )
	{
		_timeRanges.push_back( timeRange );
		return *this;
	}
	
	/**
	 * @brief To get a preview of the result, you could set a renderscale.
	 */
	This& setRenderScale( const double x, const double y )
	{
		_renderScale.x = x;
		_renderScale.y = y;
		return *this;
	}
	const OfxPointD& getRenderScale() const { return _renderScale; }
	
	/**
	 * @brief Continue as much as possible after an error.
	 * If an image file inside an image sequence failed to be loaded, we continue to process other images of the sequence.
	 */
	This& setContinueOnError( const bool v = true )
	{
		_continueOnError = v;
		return *this;
	}
	bool getContinueOnError() const { return _continueOnError; }
	
	/**
	 * @brief To get output buffer of all output nodes.
	 */
	This& setReturnBuffers( const bool v = true )
	{
		_returnBuffers = v;
		return *this;
	}
	bool getReturnBuffers() const { return _returnBuffers; }
	
	/**
	 * @brief Set the verbose level of the process.
	 */
	This& setVerboseLevel( const EVerboseLevel v )
	{
		_verboseLevel = v;
		return *this;
	}
	EVerboseLevel getVerboseLevel() const { return _verboseLevel; }
	
	/**
	 * @brief Inform plugins about the kind of process: batch or interactive.
	 */
	This& setIsInteractive( const bool v = true )
	{
		_isInteractive = v;
		return *this;
	}
	bool getIsInteractive() const { return _isInteractive; }
	
	/**
	 * @brief For debug puposes only, you could force to call the process on all identity nodes.
	 * This case should never happens to the plugin, so it may fail to do it.
	 */
	This& setForceIdentityNodesProcess( const bool v = true )
	{
		_forceIdentityNodesProcess = v;
		return *this;
	}
	bool getForceIdentityNodesProcess() const { return _forceIdentityNodesProcess; }
	
	/**
	 * @brief The application would like to abort the process (from another thread).
	 */
	This& abort()
	{
		_abort.store( true, boost::memory_order_relaxed );
		return *this;
	}
	/**
	 * @brief Has someone asked to abort the process?
	 */
	bool getAbort() const { return _abort.load( boost::memory_order_relaxed ); }
	
private:
	std::list<TimeRange> _timeRanges;
	
	OfxPointD _renderScale;
	bool _continueOnError;
	bool _forceIdentityNodesProcess;
	bool _returnBuffers;
	EVerboseLevel _verboseLevel;
	bool _isInteractive;
	
	boost::atomic_bool _abort;
};

}
}

#endif
