#ifndef OFXH_PLUGIN_HPP
#define OFXH_PLUGIN_HPP

#include "OfxhPluginDesc.hpp"
#include "OfxhPluginAPICache.hpp"

namespace tuttle
{
namespace host
{
namespace ofx
{

class OfxhPluginBinary;

/**
 * class that we use to manipulate a plugin.
 *
 * Owned by the PluginBinary it lives inside.
 * Plugins can only be pass about either by pointer or reference.
 */
class OfxhPlugin : public OfxhPluginDesc
{
public:
    typedef OfxhPlugin This;

private:
    OfxhPlugin(const This&);            ///< hidden
    OfxhPlugin& operator=(const This&); ///< hidden

protected:
    OfxhPluginBinary* _binary; ///< the file I live inside
    int _index;                ///< where I live inside that file
    bool _isSupported;         ///< Indicates if the plugin can be loaded (actions: Load and Describe)

public:
    OfxhPlugin()
        : _binary(NULL)
        , _index(-1)
        , _isSupported(false)
    {
    }

    /**
     * construct this based on the struct returned by the getNthPlugin() in the binary
     */
    OfxhPlugin(OfxhPluginBinary& bin, int idx, OfxPlugin& o)
        : OfxhPluginDesc(o)
        , _binary(&bin)
        , _index(idx)
        , _isSupported(false)
    {
    }

    /**
     * construct me from the cache
     */
    OfxhPlugin(OfxhPluginBinary& bin, int idx, const std::string& api, int apiVersion, const std::string& identifier,
               const std::string& rawIdentifier, int majorVersion, int minorVersion)
        : OfxhPluginDesc(api, apiVersion, identifier, rawIdentifier, majorVersion, minorVersion)
        , _binary(&bin)
        , _index(idx)
        , _isSupported(false)
    {
    }

    virtual ~OfxhPlugin() = 0;

    bool operator==(const This& other) const;
    bool operator!=(const This& other) const { return !This::operator==(other); }

    void setBinary(OfxhPluginBinary& binary) { _binary = &binary; }
    OfxhPluginBinary& getBinary() { return *_binary; }
    const OfxhPluginBinary& getBinary() const { return *_binary; }

    void setIsSupported(bool isSupported) { _isSupported = isSupported; }
    bool isSupported() const { return _isSupported; }

    int getIndex() const { return _index; }

    bool trumps(OfxhPlugin& other)
    {
        int myMajor = getVersionMajor();
        int theirMajor = other.getVersionMajor();

        int myMinor = getVersionMinor();
        int theirMinor = other.getVersionMinor();

        if(myMajor > theirMajor)
        {
            return true;
        }

        if(myMajor == theirMajor && myMinor > theirMinor)
        {
            return true;
        }

        return false;
    }

#ifndef SWIG
    virtual void setApiHandler(APICache::OfxhPluginAPICacheI&) = 0;
    virtual APICache::OfxhPluginAPICacheI& getApiHandler() = 0;
    virtual const APICache::OfxhPluginAPICacheI& getApiHandler() const = 0;

private:
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar& BOOST_SERIALIZATION_BASE_OBJECT_NVP(OfxhPluginDesc);
        //		ar & BOOST_SERIALIZATION_NVP(_binary); // just a link, don't save
        ar& BOOST_SERIALIZATION_NVP(_index);
        ar& BOOST_SERIALIZATION_NVP(_isSupported);
    }

#endif
};
}
}
}

// BOOST_SERIALIZATION_ASSUME_ABSTRACT(tuttle::host::ofx::OfxhPlugin)

#endif
