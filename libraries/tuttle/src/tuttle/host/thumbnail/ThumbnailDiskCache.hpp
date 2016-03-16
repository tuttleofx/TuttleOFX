#ifndef _TUTTLEOFX_HOST_THUMBNAILDISKCACHE_HPP_
#define _TUTTLEOFX_HOST_THUMBNAILDISKCACHE_HPP_

#include <tuttle/host/diskCache/DiskCacheTranslator.hpp>

#include <boost/filesystem/path.hpp>

#include <string>
#include <cstddef>

namespace tuttle
{
namespace host
{
namespace attribute
{
class Image;
}

::boost::shared_ptr<attribute::Image> loadImage(const std::string& imagePath);

/**
 * @brief An helper to cache image thumbnails on your HDD.
 */
class ThumbnailDiskCache
{
public:
    static const std::string s_thumbnailExtension;
    static const int s_thumbnailMaxSize;
    typedef DiskCacheTranslator::KeyType KeyType;
    typedef ::boost::shared_ptr<attribute::Image> TImage;

public:
    ThumbnailDiskCache() {}

    std::string keyToThumbnailPath(const KeyType& key) const;

    std::string getThumbnailPath(const boost::filesystem::path& imagePath) const;
    std::string getThumbnailPath(const std::string& imagePath) const
    {
        return getThumbnailPath(boost::filesystem::path(imagePath));
    }

    /**
     * @brief Set the base directory for all cached files.
     */
    void setRootDir(const boost::filesystem::path& rootDir) { _diskCacheTranslator.setRootDir(rootDir); }
    void setRootDir(const std::string& rootDir) { setRootDir(boost::filesystem::path(rootDir)); }

    /**
     * @brief Check if the @p key exists in the cache.
     *
     * @warning There is no check if the associated thumbnail is up-to-date.
     * @see containsUpToDate
     */
    bool contains(const KeyType& key) const { return _diskCacheTranslator.contains(key); }

    /**
     * @brief Check if the @p imagePath has a thumbnail in the cache.
     */
    bool containsUpToDate(const boost::filesystem::path& imagePath) const;

    /**
     * @brief Retrieve an existing image directly from the cache without any check.
     * Throw an error if the key doesn't exist.
     *
     * @param[in] key cache key
     */
    TImage retrieveThumbnail(const KeyType key) const;

    inline TImage retrieveThumbnail(const boost::filesystem::path& imagePath) const
    {
        return retrieveThumbnail(buildKey(imagePath));
    }

    /**
     * @brief Create a thumbnail for an image path and the associated key.
     *
     * @param[out] key cache key
     * @param[in] imagePath path to the full image
     * @return thumbnail thumbnail generated from the @p imagePath file.
     */
    TImage create(KeyType& key, const boost::filesystem::path& imagePath);

    /**
     * @brief Tranform the original image path into a cache key.
     * @param[in] imagePath The path of an image file
     */
    KeyType buildKey(const boost::filesystem::path& imagePath) const;

    /**
     * @brief Get a thumbnail from an image filepath. If not in the cache,
     * creates and adds it into the cache.
     *
     * @param[out] key
     * @param[in] imagePath
     */
    TImage getThumbnail(KeyType& key, const boost::filesystem::path& imagePath);

    TImage getThumbnail(const boost::filesystem::path& imagePath)
    {
        KeyType ignoreKey;
        return getThumbnail(ignoreKey, imagePath);
    }

    TImage getThumbnail(const std::string& imagePath) { return getThumbnail(boost::filesystem::path(imagePath)); }

private:
    DiskCacheTranslator _diskCacheTranslator;
};
}
}

#endif
