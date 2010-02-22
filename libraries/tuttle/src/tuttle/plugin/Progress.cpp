#include "Progress.hpp"

#ifndef WITHOUT_OFX
 #include <ofxsMultiThread.h>
#endif

namespace tuttle {
namespace plugin {

/**
 * @brief Start the algorithm progress bar.
 *
 * @param[in]       numSteps   number of steps
 *
 */
void Progress::progressBegin( const int numSteps, const std::string& msg )
{
	_counter = 0.0;
	#ifndef WITHOUT_OFX
	_stepSize = 1.0 / static_cast<double>( numSteps * OFX::MultiThread::getNumCPUs() );
	_effect.progressStart( msg );
	#else
	_stepSize = 1.0 / static_cast<double>( numSteps );
	#endif
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
bool Progress::progressForward( const int nSteps /* = 1*/ ) //throw(PluginException)
{
	#ifndef WITHOUT_OFX
	_mutex.lock();
	_counter += _stepSize * static_cast<double>( nSteps );
	if( _effect.abort() )
	{
		_mutex.unlock();
		_effect.progressEnd();
		//        throw(ExceptionAbort( ) );
		return true;
	}
	_effect.progressUpdate( _counter );
	_mutex.unlock();
	return false;
	#else
	_counter += _stepSize * static_cast<double>( nSteps );
	COUT( "progress: " << _counter );
	return false;
	#endif
}

/**
 * @brief Ends the algorithm progress bar.
 *
 */
void Progress::progressEnd()
{
	#ifndef WITHOUT_OFX
	// Wait for the end
	_mutex.lock();
	_mutex.unlock();
	_effect.progressEnd();
	#endif
}

#ifndef WITHOUT_OFX
Progress& Progress::operator=( const Progress& p )
{
	if( this == &p )
		return *this;                                                                                                                                                                                                                                                                                               // Gracefully handle self assignment
	_counter  = p._counter;
	_stepSize = p._stepSize;
	return *this;
}

#endif

}
}

