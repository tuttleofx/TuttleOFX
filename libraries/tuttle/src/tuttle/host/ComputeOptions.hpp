#ifndef _TUTTLE_HOST_CORE_COMPUTEOPTIONS_HPP_
#define _TUTTLE_HOST_CORE_COMPUTEOPTIONS_HPP_

#include <ofxCore.h>

#include <list>
//#include <atomic>

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
	{
		init();
	}
	explicit
	ComputeOptions( const int frame )
	{
		init();
		_timeRanges.push_back( TimeRange( frame, frame ) );
	}
	ComputeOptions( const int begin, const int end, const int step = 1 )
	{
		init();
		_timeRanges.push_back( TimeRange( begin, end, step ) );
	}
	
private:
	void init()
	{
		setRenderScale( 1.0, 1.0 );
		setContinueOnError( false );
		setReturnBuffers( false );
		setVerboseLevel( eVerboseLevelError );
		setIsInteractive( false );
		setForceIdentityNodesProcess( false );
		_abort = false;
	}
	
public:
	const std::list<TimeRange>& getTimeRanges() const { return _timeRanges; }
	
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
	 * @brief Has someone asked to abort the process?
	 */
	This& abort()
	{
		_abort = true;
		return *this;
	}
	/**
	 * @brief The application ask to abort the process (from another thread)
	 */
	bool getAbort() const { return _abort; }
	
private:
	std::list<TimeRange> _timeRanges;
	
	OfxPointD _renderScale;
	bool _continueOnError;
	bool _forceIdentityNodesProcess;
	bool _returnBuffers;
	EVerboseLevel _verboseLevel;
	bool _isInteractive;
	
//	std::atomic_bool _abort; /// @todo
	bool _abort;
};

}
}

#endif
