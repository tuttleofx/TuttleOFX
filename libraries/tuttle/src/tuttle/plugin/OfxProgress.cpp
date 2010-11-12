#include "OfxProgress.hpp"

#include <ofxsMultiThread.h>

namespace tuttle {
namespace plugin {

/**
 * @brief Start the algorithm progress bar.
 *
 * @param[in]       numSteps   number of steps
 *
 */
void OfxProgress::progressBegin( const int numSteps, const std::string& msg )
{
	_counter = 0.0;
	_stepSize = 1.0 / static_cast<double>( numSteps );
	_effect.progressStart( msg );
}

/**
 * @brief Put the progress bar forward.
 *
 * @param[in]       nSteps   Number of steps processed since last call.
 *
 * @return true = effect aborted,
 *         false = continu rendering
 *
 */
bool OfxProgress::progressForward( const int nSteps )
{
	_mutex.lock();
	
	_counter += _stepSize * static_cast<double>( nSteps );

	if( _effect.abort() )
	{
		_mutex.unlock();
		_effect.progressEnd();
		//        BOOST_THROW_EXCEPTION(ExceptionAbort( ) );
		return true;
	}
	_effect.progressUpdate( _counter );
	_mutex.unlock();
	return false;
}

/**
 * @brief Ends the algorithm progress bar.
 *
 */
void OfxProgress::progressEnd()
{
	// Wait for the end
	_mutex.lock();
	_mutex.unlock();
	_effect.progressEnd();
}

OfxProgress& OfxProgress::operator=( const OfxProgress& p )
{
	if( this == &p )
		return *this;                                                                                                                                                                                                                                                                                               // Gracefully handle self assignment
	_counter  = p._counter;
	_stepSize = p._stepSize;
	return *this;
}

}
}

