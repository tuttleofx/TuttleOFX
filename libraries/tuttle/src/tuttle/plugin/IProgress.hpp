#ifndef _TUTTLE_PLUGIN_IPROGRESS_HPP_
#define _TUTTLE_PLUGIN_IPROGRESS_HPP_

#include <string>

namespace tuttle {
namespace plugin {

class IProgress
{
public:
	virtual ~IProgress() = 0;

	virtual void progressBegin( const int numSteps, const std::string& msg = "" ) = 0;
	virtual void progressEnd() = 0;
	virtual bool progressForward( const int nSteps = 1 ) = 0;
	
};

}
}


#endif
