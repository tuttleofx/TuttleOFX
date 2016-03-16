#include "DiskCacheTranslator.hpp"

#include <boost/filesystem/operations.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

#include <iostream>
#include <iomanip>

namespace tuttle
{
namespace host
{

boost::filesystem::path DiskCacheTranslator::keyToAbsolutePath(const KeyType key) const
{
    return relativePathToAbsolutePath(keyToRelativePath(key));
}

boost::filesystem::path DiskCacheTranslator::keyToRelativePath(const KeyType key) const
{
    const std::string keyStr = (boost::format("%1%") % boost::io::group(std::setfill('0'), std::setw(s_digits), key)).str();
    BOOST_ASSERT(keyStr.size() == s_digits);

    boost::filesystem::path relativePath;

    // fixed size split
    for(std::size_t i = 0; i < keyStr.size(); i += s_nbCharSplit)
    {
        relativePath /= keyStr.substr(i, s_nbCharSplit);
    }

    return relativePath;
}

DiskCacheTranslator::KeyType DiskCacheTranslator::relativePathToKey(const boost::filesystem::path& filepath) const
{
    std::string keyStr;
    BOOST_FOREACH(const boost::filesystem::path& p, filepath)
    {
        keyStr += p.string();
    }
    return boost::lexical_cast<KeyType>(keyStr);
}

boost::filesystem::path DiskCacheTranslator::absolutePathToRelativePath(const boost::filesystem::path& absoluteKeypath) const
{
    boost::filesystem::path diffPath;

    boost::filesystem::path tmpPath = absoluteKeypath;
    while(tmpPath != _rootDir)
    {
        diffPath = tmpPath.stem() / diffPath;
        tmpPath = tmpPath.parent_path();
    }

    return diffPath;
}

DiskCacheTranslator::KeyType DiskCacheTranslator::absolutePathToKey(const boost::filesystem::path& filepath) const
{
    return relativePathToKey(absolutePathToRelativePath(filepath));
}

bool DiskCacheTranslator::contains(const KeyType key) const
{
    boost::system::error_code error;
    return boost::filesystem::exists(keyToAbsolutePath(key), error);
}

bool DiskCacheTranslator::contains(const KeyType key, std::time_t& lastWriteTime) const
{
    boost::filesystem::path fullKeyPath = keyToAbsolutePath(key);
    boost::system::error_code error;
    if(!boost::filesystem::exists(fullKeyPath, error))
        return false;

    lastWriteTime = boost::filesystem::last_write_time(fullKeyPath);
    return true;
}

bool DiskCacheTranslator::contains(const boost::filesystem::path& keypath) const
{
    boost::system::error_code error;
    if(keypath.is_absolute())
        return boost::filesystem::exists(keypath, error);
    return boost::filesystem::exists(relativePathToAbsolutePath(keypath), error);
}

bool DiskCacheTranslator::contains(const boost::filesystem::path& keyPath, std::time_t& lastWriteTime) const
{
    boost::filesystem::path fullKeyPath = keyPath.is_absolute() ? keyPath : relativePathToAbsolutePath(keyPath);

    boost::system::error_code error;
    if(!boost::filesystem::exists(fullKeyPath, error))
        return false;

    lastWriteTime = boost::filesystem::last_write_time(keyPath);
    return true;
}

boost::filesystem::path DiskCacheTranslator::get(const KeyType key) const
{
    const boost::filesystem::path p = keyToAbsolutePath(key);
    if(!contains(p))
        BOOST_THROW_EXCEPTION(std::logic_error("Key not found in DiskCacheTranslator."));
    return p;
}

boost::filesystem::path DiskCacheTranslator::create(const KeyType key)
{
    if(_rootDir.empty())
        BOOST_THROW_EXCEPTION(std::logic_error("DiskCacheTranslator base directory is not set!"));

    const boost::filesystem::path p = keyToAbsolutePath(key);
    boost::filesystem::create_directories(p.parent_path());
    return p;
}
}
}
