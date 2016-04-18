#ifndef OFXH_PLUGINBINARY_HPP
#define OFXH_PLUGINBINARY_HPP

#include "OfxhPlugin.hpp"
#include "OfxhBinary.hpp"
#include "OfxhCore.hpp"

#include <boost/ptr_container/serialize_ptr_vector.hpp>
#include <boost/serialization/string.hpp>

namespace tuttle
{
namespace host
{
namespace ofx
{

class OfxhPluginLoadGuard;
class OfxhPluginCache;

/**
 * class that represents a binary file which holds plugins.
 * Has a set of plugins inside it and which it owns
 * These are owned by a PluginCache
 */
class OfxhPluginBinary
{
public:
    typedef OfxhPluginBinary This;
    typedef boost::ptr_vector<OfxhPlugin> PluginVector;

    friend class OfxhPluginLoadGuard;

protected:
    OfxhBinary _binary;           ///< our binary object, abstracted layer ontop of OS calls, defined in OfxhBinary.hpp
    std::string _filePath;        ///< full path to the file
    std::string _bundlePath;      ///< path to the .bundle directory
    PluginVector _plugins;        ///< my plugins
    time_t _fileModificationTime; ///< used as a time stamp to check modification times, used for caching
    size_t _fileSize;             ///< file size last time we check, used for caching
    bool _binaryChanged; ///< whether the timestamp/filesize in this cache is different from that in the actual binary

private:
    explicit OfxhPluginBinary()
        : _fileModificationTime(0)
        , _fileSize(0)
        , _binaryChanged(false)
    {
    }

public:
    /// create one from the cache.  this will invoke the Binary() constructor which
    /// will stat() the file.

    explicit OfxhPluginBinary(const std::string& file, const std::string& bundlePath, time_t mtime, size_t size)
        : _binary(file)
        , _filePath(file)
        , _bundlePath(bundlePath)
        , _fileModificationTime(mtime)
        , _fileSize(size)
        , _binaryChanged(false)
    {
        checkBinaryChanged();
    }

    /// constructor which will open a library file, call things inside it, and then
    /// create Plugin objects as appropriate for the plugins exported therefrom

    explicit OfxhPluginBinary(const std::string& file, const std::string& bundlePath, OfxhPluginCache* cache)
        : _binary(file)
        , _filePath(file)
        , _bundlePath(bundlePath)
        , _binaryChanged(false)
    {
        loadPluginInfo(cache);
    }

    /// dtor
    virtual ~OfxhPluginBinary();

private:
    void checkBinaryChanged()
    {
        if(_fileModificationTime != _binary.getTime() || _fileSize != _binary.getSize())
        {
            _binaryChanged = true;
        }
    }

public:
    bool operator==(const This& other) const
    {
        if(_binary != other._binary || _filePath != other._filePath || _bundlePath != other._bundlePath ||
           _plugins != other._plugins || _fileModificationTime != other._fileModificationTime ||
           _fileSize != other._fileSize || _binaryChanged != other._binaryChanged)
            return false;
        return true;
    }

    bool operator!=(const This& other) const { return !This::operator==(other); }

    time_t getFileModificationTime() const { return _fileModificationTime; }

    size_t getFileSize() const { return _fileSize; }

    const std::string& getFilePath() const { return _filePath; }

    const std::string& getBundlePath() const { return _bundlePath; }

    const std::string getResourcesPath() const { return _bundlePath + "/Contents/Resources"; }

    bool hasBinaryChanged() const { return _binaryChanged; }

    bool isLoaded() const { return _binary.isLoaded(); }

#ifndef SWIG
    void addPlugin(OfxhPlugin* pe) { _plugins.push_back(pe); }
    void loadPluginInfo(OfxhPluginCache*);
#endif

    /// how many plugins?
    int getNPlugins() const { return (int)_plugins.size(); }

    /// get plugins
    PluginVector& getPlugins() { return _plugins; }

    /// get plugins
    const PluginVector& getPlugins() const { return _plugins; }

    /// get a plugin
    OfxhPlugin& getPlugin(int idx) { return _plugins[idx]; }

    /// get a plugin
    const OfxhPlugin& getPlugin(int idx) const { return _plugins[idx]; }

private:
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar& BOOST_SERIALIZATION_NVP(_filePath);
        ar& BOOST_SERIALIZATION_NVP(_bundlePath);
        ar& BOOST_SERIALIZATION_NVP(_plugins);
        ar& BOOST_SERIALIZATION_NVP(_fileModificationTime);
        ar& BOOST_SERIALIZATION_NVP(_fileSize);

        if(typename Archive::is_loading())
        {
            _binary.init(_filePath);
            checkBinaryChanged();

            for(PluginVector::iterator it = getPlugins().begin(), itEnd = getPlugins().end(); it != itEnd; ++it)
            {
                it->setBinary(*this);
            }
        }
    }
};
}
}
}

#endif
