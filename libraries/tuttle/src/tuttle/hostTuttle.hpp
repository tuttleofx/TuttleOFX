/*
 * hostTuttle.hpp
 *
 *  Created on: 07 oct. 2009
 *      Author: Nicolas Rondaud
 */

#ifndef TUTTLE_HOSTTUTTLE_HPP_
#define TUTTLE_HOSTTUTTLE_HPP_

#include <boost/filesystem.hpp>

namespace tuttle {

class HostTuttle
{
	public:
		HostTuttle();

		boost::filesystem::path p;
};


} // namespace tuttle

#endif
