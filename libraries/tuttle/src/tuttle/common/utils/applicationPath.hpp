#ifndef _TUTTLE_COMMON_APPLICATIONPATH_HPP_
#define _TUTTLE_COMMON_APPLICATIONPATH_HPP_

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

namespace tuttle {
namespace common {


boost::filesystem::path applicationFilepath( const std::string& argv0, const boost::filesystem::path& currentPath = boost::filesystem::current_path() );


inline boost::filesystem::path canonicalApplicationFilepath( const std::string& argv0, const boost::filesystem::path& currentPath = boost::filesystem::current_path() )
{
        return boost::filesystem::canonical( applicationFilepath( argv0, currentPath ), currentPath );
}


inline boost::filesystem::path applicationFolder( const std::string& argv0, const boost::filesystem::path& currentPath = boost::filesystem::current_path() )
{
	return applicationFilepath( argv0, currentPath ).parent_path();
}


inline boost::filesystem::path canonicalApplicationFolder( const std::string& argv0, const boost::filesystem::path& currentPath = boost::filesystem::current_path() )
{
	return canonicalApplicationFilepath( argv0, currentPath ).parent_path();
}


}
}

#endif
