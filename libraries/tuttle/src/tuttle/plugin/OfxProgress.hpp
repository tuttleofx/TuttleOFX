/**
 * @brief This file provides a set of member function that
 *        are needed to manage algorithm progression.
 */
#ifndef _TUTTLE_PLUGIN_OFXPROGRESS_HPP_
#define _TUTTLE_PLUGIN_OFXPROGRESS_HPP_

#include "IProgress.hpp"
#include "exceptions.hpp"

#include <tuttle/common/utils/global.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <string>

namespace tuttle {
namespace plugin {

class OfxProgress : public IProgress
{
private:
	OFX::ImageEffect& _effect; ///< Used to access Ofx progress bar
	OFX::MultiThread::Mutex _mutex;
	OfxProgress& operator=( const OfxProgress& p );

protected:
	double _stepSize; ///< Step size of progess bar
	double _counter; ///< Current position in [0; 1]

public:
	OfxProgress( OFX::ImageEffect& effect ) : _effect( effect )
		, _mutex( 0 ) {}

	virtual ~OfxProgress() {}

	void progressBegin( const int numSteps, const std::string& msg = "" );
	void progressEnd();
	bool progressForward( const int nSteps = 1 );
};

}
}

#endif
