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
	
	This& setRenderScale( const double x, const double y )
	{
		_renderScale.x = x;
		_renderScale.y = y;
		return *this;
	}
	This& setContinueOnError( const bool v = true )
	{
		_continueOnError = v;
		return *this;
	}
	This& setReturnBuffers( const bool v = true )
	{
		_returnBuffers = v;
		return *this;
	}
	This& setVerboseLevel( const EVerboseLevel v )
	{
		_verboseLevel = v;
		return *this;
	}
	This& setIsInteractive( const bool v = true )
	{
		_interactive = v;
		return *this;
	}
	This& setForceIdentityNodesProcess( const bool v = true )
	{
		_forceIdentityNodesProcess = v;
		return *this;
	}
	This& abort()
	{
		_abort = true;
		return *this;
	}
	
	bool getAbort() const { return _abort; }
	
	std::list<TimeRange> _timeRanges;
	
	OfxPointD _renderScale;
	bool _continueOnError;
	bool _forceIdentityNodesProcess;
	bool _returnBuffers;
	EVerboseLevel _verboseLevel;
	bool _interactive;
	
//	std::atomic_bool _abort; /// @todo
	bool _abort;
};

}
}

#endif
