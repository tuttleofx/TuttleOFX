#ifndef _TUTTLEOFX_HOST_DISKCACHETRANSLATOR_HPP_
#define _TUTTLEOFX_HOST_DISKCACHETRANSLATOR_HPP_

#include <boost/filesystem/path.hpp>

#include <cstddef>
#include <limits>

namespace tuttle
{
namespace host
{

/**
 * @brief An helper to cache any kind of files on your HDD.
 * It allows to:
 *  - translate a key into a disk path
 *  - check if the files exists
 *  - create the sub-directories when needed
 *
 * Paths used here are only paths inside the cache.
 */
class DiskCacheTranslator
{
public:
    typedef std::size_t KeyType;

private:
    static const std::size_t s_digits = std::numeric_limits<KeyType>::digits;
    static const std::size_t s_nbCharSplit = 8;

public:
    /**
     * @brief Set the base directory for all cached files.
     */
    void setRootDir(const boost::filesystem::path& rootDir) { _rootDir = rootDir; }

    /**
     * @brief Convert a @p key into a filepath.
     */
    boost::filesystem::path keyToAbsolutePath(const KeyType key) const;

    /**
     * @brief Convert a @p key into a filepath.
     */
    boost::filesystem::path keyToRelativePath(const KeyType key) const;

    /**
     * @brief Convert an absolute keypath into a relative keypath.
     */
    boost::filesystem::path absolutePathToRelativePath(const boost::filesystem::path& absoluteKeypath) const;
    /**
     * @brief Convert a relative keypath into an absolute keypath.
     */
    boost::filesystem::path relativePathToAbsolutePath(const boost::filesystem::path& relativeKeypath) const
    {
        return _rootDir / relativeKeypath;
    }

    /**
     * @brief Convert a @p filepath relative to basedir into a key.
     */
    KeyType relativePathToKey(const boost::filesystem::path& filepath) const;

    /**
     * @brief Convert an absolute @p filepath into a key.
     */
    KeyType absolutePathToKey(const boost::filesystem::path& filepath) const;

    /**
     * @brief Check if the @p key exists.
     */
    bool contains(const KeyType key) const;

    /**
     * @brief Check if the @p key exists.
     */
    bool contains(const KeyType key, std::time_t& lastWriteTime) const;

    /**
     * @brief Check if the @p keypath exists.
     * @param[in] keypath path inside the cache (absolute or relative path)
     */
    bool contains(const boost::filesystem::path& keypath) const;

    /**
     * @brief Check if the @p keypath exists (absolute or relative path).
     * @param[in] keypath path inside the cache (absolute or relative path)
     * @param[out] lastWriteTime last write time of the cached file
     */
    bool contains(const boost::filesystem::path& keypath, std::time_t& lastWriteTime) const;

    /**
     * @brief Retrieve an existing key.
     * Throw an error if the key doesn't exist.
     */
    boost::filesystem::path get(const KeyType key) const;

    /**
     * @brief Retrieve a absolute filepath from a @p key and create needed directories.
     * Throw an error if you can't create directories or if basedir is not setted.
     */
    boost::filesystem::path create(const KeyType key);

private:
    boost::filesystem::path _rootDir;
};
}
}

#endif
