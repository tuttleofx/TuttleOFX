#ifndef _TUTTLE_COMMON_APPLICATIONPATH_HPP_
#define _TUTTLE_COMMON_APPLICATIONPATH_HPP_

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

namespace tuttle {
namespace common {

boost::filesystem::path applicationFilepath( const std::string& argv0, const boost::filesystem::path& currentPath = boost::filesystem::current_path() );

}
}

#endif
