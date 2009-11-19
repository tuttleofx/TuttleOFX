#ifndef TUTTLE_HOSTTUTTLE_HPP_
#define TUTTLE_HOSTTUTTLE_HPP_

#include <boost/filesystem.hpp>

namespace tuttle {
namespace host {
namespace core {

class HostTuttle
{
	public:
		HostTuttle();

		boost::filesystem::path p;
};


}
}
}

#endif
