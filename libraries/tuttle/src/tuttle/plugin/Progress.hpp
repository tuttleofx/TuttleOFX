/**
 * @brief This file provides a set of member function that
 *        are needed to manage algorithm progression.
 * @author Eloi Du Bois.
 * @date   2009/06/15
 */
#ifndef _TUTTLE_PLUGIN_PROGRESS_HPP
#define _TUTTLE_PLUGIN_PROGRESS_HPP

#include <tuttle/common/utils/global.hpp>

#ifndef WITHOUT_OFX
 #include "PluginException.hpp"

 #include <ofxsImageEffect.h>
 #include <ofxsMultiThread.h>
#endif

namespace tuttle {
namespace plugin {

class Progress
{
#ifndef WITHOUT_OFX

private:
	OFX::ImageEffect& _effect; ///< Used to access Ofx progress bar
	OFX::MultiThread::Mutex _mutex;
	Progress& operator=( const Progress& p );
#endif

protected:
	double _stepSize; ///< Step size of progess bar
	double _counter; ///< Current position in [0; 1]

public:
	#ifndef WITHOUT_OFX
	Progress( OFX::ImageEffect& effect ) : _effect( effect ),
		_mutex( 0 ) {}
	#else
	Progress() {}
	#endif

	~Progress() {}

	void progressBegin( const int numSteps, const char* msg );
	void progressEnd();
	bool progressForward( const int nSteps = 1 ); //throw(PluginException);
};

}
}

#endif // PROGRESS_HPP
