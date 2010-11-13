#ifndef _TUTTLE_PLUGIN_NOPROGRESS_HPP_
#define _TUTTLE_PLUGIN_NOPROGRESS_HPP_

#include "IProgress.hpp"

namespace tuttle {
namespace plugin {

class NoProgress : public IProgress
{
public:
	void progressBegin( const int numSteps, const std::string& msg = "" );
	void progressEnd();
	bool progressForward( const int nSteps = 1 );

protected:
	double _stepSize; ///< Step size of progess bar
	double _counter; ///< Current position in [0; 1]
};

}
}

#endif

