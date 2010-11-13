#include "NoProgress.hpp"

namespace tuttle {
namespace plugin {

/**
 * @brief Start the algorithm progress bar.
 *
 * @param[in]       numSteps   number of steps
 *
 */
void NoProgress::progressBegin( const int numSteps, const std::string& msg )
{
	_counter = 0.0;
	_stepSize = 1.0 / static_cast<double>( numSteps );
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
bool NoProgress::progressForward( const int nSteps )
{
	_counter += _stepSize * static_cast<double>( nSteps );
	return false;
}

/**
 * @brief Ends the algorithm progress bar.
 *
 */
void NoProgress::progressEnd()
{
}

}
}


