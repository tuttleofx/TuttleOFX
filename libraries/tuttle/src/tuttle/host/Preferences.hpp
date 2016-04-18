#ifndef _TUTTLE_HOST_PREFERENCES_HPP_
#define _TUTTLE_HOST_PREFERENCES_HPP_

#include <boost/filesystem/path.hpp>

#include <string>

namespace tuttle
{
namespace host
{

class Preferences
{
private:
    boost::filesystem::path _home;
    boost::filesystem::path _temp;

public:
    Preferences();

    void setTuttleHomePath(const boost::filesystem::path& home) { _home = home; }
    boost::filesystem::path getTuttleHomePath() const { return _home; }
    std::string getTuttleHomeStr() const { return getTuttleHomePath().string(); }

    boost::filesystem::path getTuttleTempPath() const { return _temp; }

    boost::filesystem::path buildTuttleTestPath() const;

private:
    boost::filesystem::path buildTuttleHome() const;
    boost::filesystem::path buildTuttleTemp() const;
};
}
}

#endif
